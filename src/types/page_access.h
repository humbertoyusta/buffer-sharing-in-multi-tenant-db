#ifndef PAGE_ACCESS_H
#define PAGE_ACCESS_H

#include <functional>

struct PageAccess {
  int tenant_id;
  int page_id;

  PageAccess(int t_tenant_id, int t_page_id);

  PageAccess();

  bool operator==(const PageAccess &other) const {
    return tenant_id == other.tenant_id && page_id == other.page_id;
  }
};

namespace std {
template <> struct hash<PageAccess> {
  size_t operator()(const PageAccess &page_access) const {
    size_t h1 = std::hash<int>()(page_access.tenant_id);
    size_t h2 = std::hash<int>()(page_access.page_id);

    return h1 ^ (h2 << 1);
  }
};
} // namespace std

#endif // PAGE_ACCESS_H