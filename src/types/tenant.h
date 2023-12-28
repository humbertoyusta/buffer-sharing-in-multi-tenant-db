#ifndef TENANT_H
#define TENANT_H

struct Tenant {
  int tenant_id;
  int priority;
  int min_buffer_size;
  int base_buffer_size;
  int max_buffer_size;
  int database_size;

  Tenant(int t_tenant_id, int t_priority, int t_min_buffer_size,
         int t_base_buffer_size, int t_max_buffer_size, int t_database_size);

  Tenant();
};

#endif // TENANT_H
