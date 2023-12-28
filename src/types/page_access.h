#ifndef PAGE_ACCESS_H
#define PAGE_ACCESS_H

struct PageAccess {
  int tenant_id;
  int page_id;

  PageAccess(int t_tenant_id, int t_page_id);

  PageAccess();
};

#endif // PAGE_ACCESS_H