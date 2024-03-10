#include "belady.h"
#include <assert.h>

Belady::Belady(std::vector<Tenant> tenants, int total_buffer_size,
               std::vector<PageAccess> page_accesses_sequence)
    : tenants_(tenants), total_buffer_size_(total_buffer_size),
      page_accesses_sequence_(page_accesses_sequence), current_time_(-1) {

  while (!available_locations_.empty())
    available_locations_.pop();
  for (int i = 1; i <= total_buffer_size; i++)
    available_locations_.push(i);

  pages_sets_.clear();
  pages_sets_.resize(tenants.size());

  pages_locations_.clear();
  pages_locations_.resize(tenants.size());

  InitNextAccessesByIndex();
  InitNextAccessesByPage();
}

std::pair<int, bool> Belady::GetPage(PageAccess page_access) {
  UpdateCurrentTimeAndNextAccesses(page_access);

  auto found =
      pages_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_locations_[page_access.tenant_id - 1].end()) {
    auto page = *found->second;
    return {page.buffer_location, true};
  }

  return {0, false};
}

void Belady::UpdateAccessHistory(PageAccess page_access) {
  auto page_it =
      pages_locations_[page_access.tenant_id - 1][page_access.page_id];
  auto page = *page_it;

  page.next_access =
      next_accesses_by_page_[page_access.tenant_id - 1][page_access.page_id];

  pages_sets_[page_access.tenant_id - 1].erase(page_it);

  auto page_inserted = pages_sets_[page_access.tenant_id - 1].insert(page);
  assert(page_inserted.second);
  pages_locations_[page_access.tenant_id - 1][page_access.page_id] =
      page_inserted.first;
}

Belady::Page Belady::EvictPage(int tenant_id) {
  auto page_to_evict_it = pages_sets_[tenant_id - 1].begin();
  auto page_to_evict = *page_to_evict_it;

  pages_sets_[tenant_id - 1].erase(page_to_evict_it);
  pages_locations_[tenant_id - 1].erase(page_to_evict.page_id);

  return page_to_evict;
}

void Belady::AddPage(PageAccess page_access, int buffer_location) {
  Page page(
      page_access.page_id, buffer_location,
      next_accesses_by_page_[page_access.tenant_id - 1][page_access.page_id]);

  auto page_inserted = pages_sets_[page_access.tenant_id - 1].insert(page);
  assert(page_inserted.second);
  pages_locations_[page_access.tenant_id - 1][page_access.page_id] =
      page_inserted.first;
}

int Belady::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }

  int buffer_location = available_locations_.front();
  available_locations_.pop();
  return buffer_location;
}

void Belady::UpdateCurrentTimeAndNextAccesses(PageAccess page_access) {
  current_time_++;
  next_accesses_by_page_[page_access.tenant_id - 1][page_access.page_id] =
      next_accesses_by_index_[current_time_];
}

void Belady::InitNextAccessesByIndex() {
  next_accesses_by_index_.clear();
  next_accesses_by_index_.resize(page_accesses_sequence_.size());

  std::vector<std::unordered_map<int, int>> last_seen(tenants_.size());

  for (int index = page_accesses_sequence_.size() - 1; index >= 0; index--) {
    auto page_access = page_accesses_sequence_[index];

    if (last_seen[page_access.tenant_id - 1].find(page_access.page_id) ==
        last_seen[page_access.tenant_id - 1].end()) {
      next_accesses_by_index_[index] = INT_MAX;
    } else {
      next_accesses_by_index_[index] =
          last_seen[page_access.tenant_id - 1][page_access.page_id];
    }

    last_seen[page_access.tenant_id - 1][page_access.page_id] = index;
  }
}

void Belady::InitNextAccessesByPage() {
  next_accesses_by_page_.clear();
  next_accesses_by_page_.resize(tenants_.size());

  for (int index = 0; index < page_accesses_sequence_.size(); index++) {
    auto page_access = page_accesses_sequence_[index];

    if (next_accesses_by_page_[page_access.tenant_id - 1].find(
            page_access.page_id) ==
        next_accesses_by_page_[page_access.tenant_id - 1].end()) {
      next_accesses_by_page_[page_access.tenant_id - 1][page_access.page_id] =
          index;
    }
  }
}