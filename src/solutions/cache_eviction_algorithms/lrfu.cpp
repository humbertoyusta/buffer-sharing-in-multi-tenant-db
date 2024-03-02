#include "lrfu.h"
#include <assert.h>

int LRFU::current_time_ = 0;
double LRFU::lambda_ = 0.0;
double LRFU::two_to_the_power_minus_lambda_ = 0.0;

double LRFU::Page::getCRFValue(int current_time) const {
  return last_CRF_value * std::pow(two_to_the_power_minus_lambda_,
                                   current_time - last_access_time);
}

bool LRFU::Page::operator<(const Page &other) const {
  double crf_value = getCRFValue(current_time_);
  double other_crf_value = other.getCRFValue(current_time_);
  if (crf_value != other_crf_value) {
    return crf_value < other_crf_value;
  } else {
    return last_access_time < other.last_access_time;
  }
}

LRFU::LRFU(std::vector<Tenant> tenants, int total_buffer_size, double lambda)
    : total_buffer_size_(total_buffer_size), tenants_(tenants) {
  lambda_ = lambda;
  two_to_the_power_minus_lambda_ = std::pow(2, -lambda_);

  while (!available_locations_.empty())
    available_locations_.pop();
  for (int i = 1; i <= total_buffer_size; ++i) {
    available_locations_.push(i);
  }

  pages_maps_.clear();
  pages_sets_.clear();
  pages_sets_.resize(tenants.size());
  pages_maps_.resize(tenants.size());
}

std::pair<int, bool> LRFU::GetPage(PageAccess page_access) {
  current_time_++;

  auto found = pages_maps_[page_access.tenant_id - 1].find(page_access.page_id);

  if (found != pages_maps_[page_access.tenant_id - 1].end()) {
    auto page = found->second;
    return {page->buffer_location, true};
  }

  return {0, false};
}

LRFU::Page LRFU::EvictPage(int tenant_id) {
  auto page_it = pages_sets_[tenant_id - 1].begin();
  auto page = *page_it;
  pages_sets_[tenant_id - 1].erase(page_it);
  pages_maps_[tenant_id - 1].erase(page.page_id);
  return page;
}

void LRFU::SetPageAsMostRecentlyUsed(PageAccess page_access) {
  auto page_pointer =
      pages_maps_[page_access.tenant_id - 1][page_access.page_id];

  auto page = *page_pointer;
  pages_sets_[page_access.tenant_id - 1].erase(page_pointer);

  page.last_CRF_value = page.getCRFValue(current_time_) + 1.0;
  page.last_access_time = current_time_;

  auto inserted_page = pages_sets_[page_access.tenant_id - 1].insert(page);
  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      inserted_page.first;
  assert(inserted_page.second);
}

void LRFU::AddPage(PageAccess page_access, int buffer_location) {

  auto inserted_page = pages_sets_[page_access.tenant_id - 1].insert(
      Page(page_access.page_id, buffer_location, current_time_, 1.0));

  assert(inserted_page.second);

  pages_maps_[page_access.tenant_id - 1][page_access.page_id] =
      inserted_page.first;
}

int LRFU::GetAvailableLocation() {
  if (available_locations_.empty()) {
    return -1;
  }

  int buffer_location = available_locations_.front();
  available_locations_.pop();
  return buffer_location;
}