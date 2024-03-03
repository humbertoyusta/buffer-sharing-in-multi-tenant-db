#include "mq.h"
#include <assert.h>

MQ::MQ(std::vector<Tenant> tenants, int total_buffer_size, int num_queues,
       double lifetime_length_multiplier,
       double retained_pages_period_length_multiplier)
    : total_buffer_size_(total_buffer_size), tenants_(tenants),
      num_queues_(num_queues),
      lifetime_length_multiplier_(lifetime_length_multiplier),
      retained_pages_period_length_multiplier_(
          retained_pages_period_length_multiplier),
      current_time_(0) {

  while (!available_locations_.empty())
    available_locations_.pop();
  for (int i = 1; i <= total_buffer_size; ++i) {
    available_locations_.push(i);
  }

  cache_locations_.clear();
  cache_locations_.resize(tenants.size());
  cache_queues_.clear();
  cache_queues_.resize(tenants.size());
  retained_pages_locations_.clear();
  retained_pages_locations_.resize(tenants.size());
  retained_pages_.clear();
  retained_pages_.resize(tenants.size());

  for (int i = 0; i < tenants.size(); ++i) {
    cache_queues_[i].resize(num_queues);
  }

  lifetimes_.clear();
  retained_periods_.clear();
  for (int i = 0; i < tenants.size(); ++i) {
    lifetimes_.push_back(
        int(double(tenants[i].base_buffer_size) * lifetime_length_multiplier_));
    retained_periods_.push_back(int(double(tenants[i].base_buffer_size) *
                                    retained_pages_period_length_multiplier_));
  }
}

std::pair<int, bool> MQ::GetPage(PageAccess page_access) {
  auto found =
      cache_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != cache_locations_[page_access.tenant_id - 1].end()) {
    auto page = found->second.second;
    return {page->buffer_location, true};
  }

  return {0, false};
}

MQ::Page MQ::EvictPage(int tenant_id) {
  for (int i = 0; i < num_queues_; ++i) {
    if (!cache_queues_[tenant_id - 1][i].empty()) {
      auto page = cache_queues_[tenant_id - 1][i].front();
      cache_queues_[tenant_id - 1][i].pop_front();

      cache_locations_[tenant_id - 1].erase(page.page_id);

      AddPageToRetainedPages(page, tenant_id);
      return page;
    }
  }
  throw std::runtime_error("No page to evict");
}

void MQ::UpdateAccessHistory(PageAccess page_access) {
  auto found =
      cache_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  assert(found != cache_locations_[page_access.tenant_id - 1].end());

  int current_queue = found->second.first;
  auto page_iterator = found->second.second;
  auto page = *page_iterator;

  cache_queues_[page_access.tenant_id - 1][current_queue].erase(page_iterator);

  page.reference_count++;
  page.expiration_time = current_time_ + lifetimes_[page_access.tenant_id - 1];

  int queue_to_move = GetQueueNumberBasedOnReferenceCount(page.reference_count);

  cache_queues_[page_access.tenant_id - 1][queue_to_move].push_back(page);
  cache_locations_[page_access.tenant_id - 1][page.page_id] = {
      queue_to_move,
      prev(cache_queues_[page_access.tenant_id - 1][queue_to_move].end())};
}

void MQ::AddPage(PageAccess page_access, int buffer_location) {

  auto retained_page_location =
      retained_pages_locations_[page_access.tenant_id - 1].find(
          page_access.page_id);

  int queue_to_move = 0;
  int reference_count = 1;

  if (retained_page_location !=
      retained_pages_locations_[page_access.tenant_id - 1].end()) {
    auto retained_page = retained_page_location->second;

    reference_count = retained_page->reference_count;
    queue_to_move = GetQueueNumberBasedOnReferenceCount(reference_count);

    retained_pages_locations_[page_access.tenant_id - 1].erase(
        retained_page_location);
    retained_pages_[page_access.tenant_id - 1].erase(retained_page);
  }

  Page page(page_access.page_id, buffer_location, reference_count,
            current_time_ + lifetimes_[page_access.tenant_id - 1]);

  cache_queues_[page_access.tenant_id - 1][queue_to_move].push_back(page);
  cache_locations_[page_access.tenant_id - 1][page_access.page_id] = {
      queue_to_move,
      prev(cache_queues_[page_access.tenant_id - 1][queue_to_move].end())};
}

void MQ::AdjustQueues() {
  current_time_++;

  for (int tenant = 0; tenant < tenants_.size(); ++tenant) {
    for (int i = 1; i < num_queues_; ++i) {
      if (!cache_queues_[tenant][i].empty() &&
          cache_queues_[tenant][i].front().expiration_time < current_time_) {
        auto page = cache_queues_[tenant][i].front();
        cache_queues_[tenant][i].pop_front();

        page.expiration_time = current_time_ + lifetimes_[tenant];

        cache_queues_[tenant][i - 1].push_back(page);
        cache_locations_[tenant][page.page_id] = {
            i - 1, prev(cache_queues_[tenant][i - 1].end())};
      }
    }
  }
}

void MQ::AddPageToRetainedPages(Page page, int tenant_id) {
  RetainedPage retained_page(tenant_id, page.page_id, page.reference_count);

  retained_pages_[tenant_id - 1].push_back(retained_page);
  retained_pages_locations_[tenant_id - 1][page.page_id] =
      prev(retained_pages_[tenant_id - 1].end());

  if (retained_pages_[tenant_id - 1].size() >
      retained_periods_[tenant_id - 1]) {
    auto deleted_page = retained_pages_[tenant_id - 1].front();

    retained_pages_[tenant_id - 1].pop_front();
    retained_pages_locations_[tenant_id - 1].erase(deleted_page.page_id);
  }
}

int MQ::GetQueueNumberBasedOnReferenceCount(int reference_count) {
  return std::min(31 - __builtin_clz(reference_count), num_queues_ - 1);
}

int MQ::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }

  int buffer_location = available_locations_.front();
  available_locations_.pop();
  return buffer_location;
}