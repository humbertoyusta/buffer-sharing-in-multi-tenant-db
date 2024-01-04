#include "lru_2.h"
#include <assert.h>
#include <iostream>

Lru2::Lru2(std::vector<Tenant> tenants, int total_buffer_size,
           double correlated_reference_period_length_multiplier_)
    : tenants_(tenants), total_buffer_size_(total_buffer_size),
      current_time_(0), correlated_reference_period_length_multiplier_(
                            correlated_reference_period_length_multiplier_) {
  for (int i = 1; i <= total_buffer_size; ++i) {
    available_locations_.push(i);
  }
  pages_sets_.resize(tenants.size());
  pages_maps_.resize(tenants.size());
  correlated_pages_lists_.resize(tenants.size());
  correlated_pages_maps_.resize(tenants.size());
  retained_pages_lists_.resize(tenants.size());
  retained_pages_maps_.resize(tenants.size());
  for (int i = 0; i < tenants.size(); ++i) {
    correlated_reference_period_length_.push_back(
        int((double)tenants[i].base_buffer_size *
            correlated_reference_period_length_multiplier_));
    retained_period_length_.push_back(tenants[i].base_buffer_size * 2);
  }
}

std::pair<int, bool> Lru2::GetPage(PageAccess page_access) {
  auto found_correlated =
      correlated_pages_maps_[page_access.tenant_id - 1].find(
          page_access.page_id);

  if (found_correlated !=
      correlated_pages_maps_[page_access.tenant_id - 1].end()) {
    return {found_correlated->second->buffer_location, true};
  }

  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_maps_[page_access.tenant_id - 1].end()) {
    return {found->second->buffer_location, true};
  }

  return {0, false};
}

Lru2::Page Lru2::EvictPage(int tenant_id) {
  if (!pages_sets_[tenant_id - 1].empty()) {
    auto page_to_evict_iterator = pages_sets_[tenant_id - 1].begin();
    auto page_to_evict = *page_to_evict_iterator;
    pages_maps_[tenant_id - 1].erase(page_to_evict.page_id);
    pages_sets_[tenant_id - 1].erase(page_to_evict_iterator);

    InsertToRetainedPages(page_to_evict.page_id, tenant_id,
                          page_to_evict.last_accessed);
    return page_to_evict;
  } else {
    assert(!correlated_pages_lists_[tenant_id - 1].empty());
    std::list<Page>::iterator page_to_evict_iterator =
        prev(correlated_pages_lists_[tenant_id - 1].end());
    auto page_to_evict = *page_to_evict_iterator;
    correlated_pages_maps_[tenant_id - 1].erase(page_to_evict.page_id);
    correlated_pages_lists_[tenant_id - 1].erase(page_to_evict_iterator);

    InsertToRetainedPages(page_to_evict.page_id, tenant_id,
                          page_to_evict.last_accessed);
    return page_to_evict;
  }
}

void Lru2::UpdateAccessHistory(PageAccess page_access) {
  ++current_time_;

  auto found_correlated =
      correlated_pages_maps_[page_access.tenant_id - 1].find(
          page_access.page_id);

  if (found_correlated !=
      correlated_pages_maps_[page_access.tenant_id - 1].end()) {
    auto page = *found_correlated->second;
    // correlated_pages_lists_[page_access.tenant_id - 1].erase(
    //     found_correlated->second);

    // page.second_last_accessed = page.last_accessed;
    page.last_accessed = current_time_;

    // correlated_pages_lists_[page_access.tenant_id - 1].push_front(page);
    // correlated_pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
    //     correlated_pages_lists_[page_access.tenant_id - 1].begin();
  } else {

    auto found =
        pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

    auto page = *found->second;
    pages_sets_[page_access.tenant_id - 1].erase(found->second);
    pages_maps_[page_access.tenant_id - 1].erase(found);

    page.second_last_accessed = page.last_accessed;
    page.last_accessed = current_time_;

    correlated_pages_lists_[page_access.tenant_id - 1].push_front(page);
    correlated_pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
        correlated_pages_lists_[page_access.tenant_id - 1].begin();

    if (correlated_pages_lists_[page_access.tenant_id - 1].size() >
        correlated_reference_period_length_[page_access.tenant_id - 1]) {
      MoveLastCorrelatedPageToMain(page_access.tenant_id);
    }
  }
}

void Lru2::AddPage(PageAccess page_access, int buffer_location) {
  ++current_time_;

  auto retained_access_found =
      retained_pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  int second_last_access = 0;
  if (retained_access_found !=
      retained_pages_maps_[page_access.tenant_id - 1].end()) {
    second_last_access = retained_access_found->second->second;

    retained_pages_lists_[page_access.tenant_id - 1].erase(
        retained_access_found->second);
    retained_pages_maps_[page_access.tenant_id - 1].erase(
        retained_access_found->first);
  }

  Page page = Page(page_access.page_id, buffer_location, current_time_,
                   second_last_access);

  correlated_pages_lists_[page_access.tenant_id - 1].push_front(page);
  correlated_pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      correlated_pages_lists_[page_access.tenant_id - 1].begin();

  if (correlated_pages_lists_[page_access.tenant_id - 1].size() >
      correlated_reference_period_length_[page_access.tenant_id - 1]) {
    MoveLastCorrelatedPageToMain(page_access.tenant_id);
  }
}

void Lru2::MoveLastCorrelatedPageToMain(int tenant_id) {
  std::list<Page>::iterator page_to_move_iterator =
      prev(correlated_pages_lists_[tenant_id - 1].end());
  auto page_to_move = *page_to_move_iterator;
  correlated_pages_maps_[tenant_id - 1].erase(page_to_move.page_id);
  correlated_pages_lists_[tenant_id - 1].erase(page_to_move_iterator);

  auto insert_result = pages_sets_[tenant_id - 1].insert(page_to_move);
  assert(insert_result.second);
  pages_maps_[tenant_id - 1][page_to_move.page_id] = insert_result.first;
}

void Lru2::InsertToRetainedPages(int page_id, int tenant_id,
                                 int last_accessed_time) {
  auto found = retained_pages_maps_[tenant_id - 1].find(page_id);
  if (found != retained_pages_maps_[tenant_id - 1].end()) {
    auto page = *found->second;
    retained_pages_lists_[tenant_id - 1].erase(found->second);
    retained_pages_lists_[tenant_id - 1].push_front(
        {page_id, last_accessed_time});
    retained_pages_maps_[tenant_id - 1][page_id] =
        retained_pages_lists_[tenant_id - 1].begin();
  } else {
    retained_pages_lists_[tenant_id - 1].push_front(
        {page_id, last_accessed_time});
    retained_pages_maps_[tenant_id - 1][page_id] =
        retained_pages_lists_[tenant_id - 1].begin();

    if (retained_pages_lists_[tenant_id - 1].size() >
        retained_period_length_[tenant_id - 1]) {
      auto page_to_evict_iterator =
          prev(retained_pages_lists_[tenant_id - 1].end());
      auto page_to_evict = *page_to_evict_iterator;

      retained_pages_maps_[tenant_id - 1].erase(page_to_evict.first);
      retained_pages_lists_[tenant_id - 1].erase(page_to_evict_iterator);
    }
  }
}

int Lru2::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }
  int available_location = available_locations_.front();
  available_locations_.pop();
  return available_location;
}