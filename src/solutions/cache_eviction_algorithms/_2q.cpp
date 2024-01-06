#include "_2q.h"
#include <assert.h>

_2Q::_2Q(std::vector<Tenant> tenants, int total_buffer_size,
         double a1_in_buffer_ratio_size, double a1_out_buffer_ratio_size)
    : tenants_(tenants), total_buffer_size_(total_buffer_size),
      a1_in_buffer_ratio_size_(a1_in_buffer_ratio_size),
      a1_out_buffer_ratio_size_(a1_out_buffer_ratio_size) {
  for (int i = 1; i <= total_buffer_size_; i++) {
    available_locations_.push(i);
  }
  a1_in_queue_.resize(tenants_.size());
  a1_in_maps_.resize(tenants_.size());
  a1_out_queue_.resize(tenants_.size());
  a1_out_maps_.resize(tenants_.size());
  am_queue_.resize(tenants_.size());
  am_maps_.resize(tenants_.size());
}

std::pair<int, bool> _2Q::GetPage(PageAccess page_access) {
  // Check if page is in Am queue
  auto found_in_am =
      am_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found_in_am != am_maps_[page_access.tenant_id - 1].end()) {
    int buffer_location = found_in_am->second->second;
    return {buffer_location, true};
  } else {
    // Check if page is in A1 in queue
    auto found_in_a1_in =
        a1_in_maps_[page_access.tenant_id - 1].find(page_access.page_id);

    if (found_in_a1_in != a1_in_maps_[page_access.tenant_id - 1].end()) {
      int buffer_location = found_in_a1_in->second->second;
      return {buffer_location, true};
    } else {
      // No need to check A1 out since that pages are not in the cache
      return {0, false};
    }
  }
}

std::pair<int, int> _2Q::EvictPage(int tenant_id) {
  assert(!a1_in_queue_[tenant_id - 1].empty() ||
         !am_queue_[tenant_id - 1].empty());

  if (((double)a1_in_queue_.size() >
           ((double)a1_in_queue_.size() + (double)am_queue_.size()) *
               a1_in_buffer_ratio_size_ &&
       !a1_in_queue_[tenant_id - 1].empty()) ||
      am_queue_[tenant_id - 1].empty()) {
    // Evict from A1 in queue
    auto page_to_evict = a1_in_queue_[tenant_id - 1].back();
    a1_in_queue_[tenant_id - 1].pop_back();
    a1_in_maps_[tenant_id - 1].erase(page_to_evict.first);

    AddPageToA1OutQueue(tenant_id, page_to_evict.first);

    return {page_to_evict.first, page_to_evict.second};
  } else {
    // Evict from Am queue
    auto page_to_evict = am_queue_[tenant_id - 1].back();
    am_queue_[tenant_id - 1].pop_back();
    am_maps_[tenant_id - 1].erase(page_to_evict.first);

    // Do not add page to A1 out queue since it is has not been accessed in a
    // while

    return {page_to_evict.first, page_to_evict.second};
  }
}

void _2Q::SetPageAsMostRecentlyUsed(PageAccess page_access) {
  // Check if page is in Am queue
  auto found_in_am =
      am_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found_in_am != am_maps_[page_access.tenant_id - 1].end()) {
    // Move page to the front of the queue
    int buffer_location = found_in_am->second->second;
    am_queue_[page_access.tenant_id - 1].erase(found_in_am->second);
    am_queue_[page_access.tenant_id - 1].push_front(
        {page_access.page_id, buffer_location});
    am_maps_[page_access.tenant_id - 1][page_access.page_id] =
        am_queue_[page_access.tenant_id - 1].begin();
  } else {
    // If it is not there it must be on A1 in queue, in this case
    // do nothing, since it is a FIFO queue
    assert(a1_in_maps_[page_access.tenant_id - 1].find(page_access.page_id) !=
           a1_in_maps_[page_access.tenant_id - 1].end());
  }
}

void _2Q::AddPage(PageAccess page_access, int buffer_location) {
  bool was_in_a1_out_queue_ = false;

  // Check if page is in A1 out queue
  auto found_in_a1_out =
      a1_out_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found_in_a1_out != a1_out_maps_[page_access.tenant_id - 1].end()) {
    was_in_a1_out_queue_ = true;
    // Remove page from A1 out queue
    a1_out_queue_[page_access.tenant_id - 1].erase(found_in_a1_out->second);
    a1_out_maps_[page_access.tenant_id - 1].erase(page_access.page_id);
  }

  if (was_in_a1_out_queue_) {
    // Add page to Am queue
    am_queue_[page_access.tenant_id - 1].push_front(
        {page_access.page_id, buffer_location});
    am_maps_[page_access.tenant_id - 1][page_access.page_id] =
        am_queue_[page_access.tenant_id - 1].begin();
  } else {
    // Add page to A1 in queue
    a1_in_queue_[page_access.tenant_id - 1].push_front(
        {page_access.page_id, buffer_location});
    a1_in_maps_[page_access.tenant_id - 1][page_access.page_id] =
        a1_in_queue_[page_access.tenant_id - 1].begin();
  }
}

void _2Q::AddPageToA1OutQueue(int tenant_id, int page_id) {
  // Add page to A1 out queue
  a1_out_queue_[tenant_id - 1].push_front(page_id);
  a1_out_maps_[tenant_id - 1][page_id] = a1_out_queue_[tenant_id - 1].begin();

  // If A1 out queue is full, evict the last page
  if (a1_out_queue_[tenant_id - 1].size() >
      (double)tenants_[tenant_id - 1].base_buffer_size *
          a1_in_buffer_ratio_size_) {
    int page_to_evict = a1_out_queue_[tenant_id - 1].back();
    a1_out_queue_[tenant_id - 1].pop_back();
    a1_out_maps_[tenant_id - 1].erase(page_to_evict);
  }
}

int _2Q::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }
  int location = available_locations_.front();
  available_locations_.pop();
  return location;
}