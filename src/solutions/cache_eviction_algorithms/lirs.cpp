#include "lirs.h"
#include <assert.h>

LIRS::LIRS(std::vector<Tenant> tenants, int total_buffer_size,
           double lirs_buffer_ratio_size)
    : tenants_(tenants), total_buffer_size_(total_buffer_size),
      lirs_buffer_ratio_size_(lirs_buffer_ratio_size) {

  while (!available_locations_.empty())
    available_locations_.pop();
  for (int i = 1; i <= total_buffer_size; i++)
    available_locations_.push(i);

  lirs_list_.clear();
  lirs_locations_.clear();
  hirs_list_.clear();
  hirs_locations_.clear();

  lirs_list_.resize(tenants.size());
  lirs_locations_.resize(tenants.size());
  hirs_list_.resize(tenants.size());
  hirs_locations_.resize(tenants.size());

  lirs_sizes_.clear();
  lirs_sizes_.resize(tenants.size(), 0);
}

std::pair<int, bool> LIRS::GetPage(PageAccess page_access) {
  auto found =
      lirs_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != lirs_locations_[page_access.tenant_id - 1].end()) {
    auto page = *found->second;
    if (page.IsInCache()) {
      return {page.buffer_location, true};
    }
  }

  found = hirs_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != hirs_locations_[page_access.tenant_id - 1].end()) {
    auto page = *found->second;
    assert(page.IsInCache());
    return {page.buffer_location, true};
  }

  return {0, false};
}

LIRS::Page LIRS::EvictPage(int tenant_id) {
  if (!hirs_list_[tenant_id - 1].empty()) {
    auto page = hirs_list_[tenant_id - 1].back();

    hirs_list_[tenant_id - 1].pop_back();
    hirs_locations_[tenant_id - 1].erase(page.page_id);

    // If the page is in the LIRS list, update it saying that is no longer in
    // the cache
    auto page_it_in_lirs = lirs_locations_[tenant_id - 1].find(page.page_id);
    if (page_it_in_lirs != lirs_locations_[tenant_id - 1].end()) {
      page_it_in_lirs->second->buffer_location = -1;
    }

    if (ShouldTurnLastLIRPageToHIR(tenant_id)) {
      TurnLastLIRPageToHIR(tenant_id);
    }

    return page;
  } else {
    auto page = lirs_list_[tenant_id - 1].back();
    assert(page.is_lir && page.IsInCache());

    lirs_list_[tenant_id - 1].pop_back();
    lirs_locations_[tenant_id - 1].erase(page.page_id);

    PruneLIRS(tenant_id);

    if (ShouldTurnLastLIRPageToHIR(tenant_id)) {
      TurnLastLIRPageToHIR(tenant_id);
    }

    return page;
  }
}

void LIRS::UpdateAccessHistory(PageAccess page_access) {
  auto page_in_lirs =
      lirs_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (page_in_lirs != lirs_locations_[page_access.tenant_id - 1].end()) {
    auto page = *page_in_lirs->second;

    if (page.is_lir) {
      // It is a LIR page, move it to the front of the LIRS list
      lirs_list_[page_access.tenant_id - 1].erase(page_in_lirs->second);

      lirs_list_[page_access.tenant_id - 1].push_front(page);
      lirs_locations_[page_access.tenant_id - 1][page.page_id] =
          lirs_list_[page_access.tenant_id - 1].begin();

      PruneLIRS(page_access.tenant_id);
    } else {
      // It is a recent HIR page, move it to the front of the LIRS list
      assert(page.IsInCache());

      lirs_list_[page_access.tenant_id - 1].erase(page_in_lirs->second);
      page.is_lir = true;
      lirs_sizes_[page_access.tenant_id - 1]++;

      // Find page in hirs list and remove it
      auto page_in_hirs =
          hirs_locations_[page_access.tenant_id - 1].find(page.page_id);

      assert(page_in_hirs != hirs_locations_[page_access.tenant_id - 1].end());

      hirs_list_[page_access.tenant_id - 1].erase(page_in_hirs->second);
      hirs_locations_[page_access.tenant_id - 1].erase(page.page_id);

      // Add page to front of lirs list
      lirs_list_[page_access.tenant_id - 1].push_front(page);
      lirs_locations_[page_access.tenant_id - 1][page.page_id] =
          lirs_list_[page_access.tenant_id - 1].begin();

      PruneLIRS(page_access.tenant_id);
    }
  } else {
    // It was a HIR page, that is not frequent enough to be in the LIRS list
    auto page_in_hirs =
        hirs_locations_[page_access.tenant_id - 1].find(page_access.page_id);

    assert(page_in_hirs != hirs_locations_[page_access.tenant_id - 1].end());

    auto page = *page_in_hirs->second;

    hirs_list_[page_access.tenant_id - 1].erase(page_in_hirs->second);

    hirs_list_[page_access.tenant_id - 1].push_front(page);
    hirs_locations_[page_access.tenant_id - 1][page.page_id] =
        hirs_list_[page_access.tenant_id - 1].begin();
  }

  if (ShouldTurnLastLIRPageToHIR(page_access.tenant_id)) {
    TurnLastLIRPageToHIR(page_access.tenant_id);
  }
}

void LIRS::AddPage(PageAccess page_access, int buffer_location) {
  auto page_in_lirs =
      lirs_locations_[page_access.tenant_id - 1].find(page_access.page_id);

  if (page_in_lirs != lirs_locations_[page_access.tenant_id - 1].end()) {
    auto page = *page_in_lirs->second;
    assert(!page.IsInCache());

    page.buffer_location = buffer_location;
    page.is_lir = true;
    lirs_sizes_[page_access.tenant_id - 1]++;

    lirs_list_[page_access.tenant_id - 1].erase(page_in_lirs->second);
    lirs_list_[page_access.tenant_id - 1].push_front(page);
    lirs_locations_[page_access.tenant_id - 1][page.page_id] =
        lirs_list_[page_access.tenant_id - 1].begin();

    PruneLIRS(page_access.tenant_id);
  } else {
    assert(
        hirs_locations_[page_access.tenant_id - 1].find(page_access.page_id) ==
        hirs_locations_[page_access.tenant_id - 1].end());

    auto page = Page(page_access.page_id, buffer_location, false);

    hirs_list_[page_access.tenant_id - 1].push_front(page);
    hirs_locations_[page_access.tenant_id - 1][page.page_id] =
        hirs_list_[page_access.tenant_id - 1].begin();

    lirs_list_[page_access.tenant_id - 1].push_front(page);
    lirs_locations_[page_access.tenant_id - 1][page.page_id] =
        lirs_list_[page_access.tenant_id - 1].begin();
  }

  if (ShouldTurnLastLIRPageToHIR(page_access.tenant_id)) {
    TurnLastLIRPageToHIR(page_access.tenant_id);
  }
}

int LIRS::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }

  int buffer_location = available_locations_.front();
  available_locations_.pop();
  return buffer_location;
}

void LIRS::PruneLIRS(int tenant_id) {
  while (!lirs_list_[tenant_id - 1].empty() &&
         !lirs_list_[tenant_id - 1].back().is_lir) {
    auto page = lirs_list_[tenant_id - 1].back();

    lirs_list_[tenant_id - 1].pop_back();
    lirs_locations_[tenant_id - 1].erase(page.page_id);
  }
}

void LIRS::TurnLastLIRPageToHIR(int tenant_id) {
  if (lirs_list_[tenant_id - 1].empty() || lirs_sizes_[tenant_id - 1] != 0) {
    return;
  }

  auto page = lirs_list_[tenant_id - 1].back();
  assert(page.is_lir);
  assert(page.IsInCache());

  lirs_list_[tenant_id - 1].pop_back();
  lirs_locations_[tenant_id - 1].erase(page.page_id);

  page.is_lir = false;
  lirs_sizes_[tenant_id - 1]--;

  hirs_list_[tenant_id - 1].push_front(page);
  hirs_locations_[tenant_id - 1][page.page_id] =
      hirs_list_[tenant_id - 1].begin();

  PruneLIRS(tenant_id);

  assert(!ShouldTurnLastLIRPageToHIR(tenant_id));
}

bool LIRS::ShouldTurnLastLIRPageToHIR(int tenant_id) {
  return lirs_sizes_[tenant_id - 1] >=
         lirs_buffer_ratio_size_ * double(lirs_sizes_[tenant_id - 1] +
                                          hirs_list_[tenant_id - 1].size());
}
