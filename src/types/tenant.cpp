#include "tenant.h"

Tenant::Tenant(int t_tenant_id, int t_priority, int t_min_buffer_size,
               int t_base_buffer_size, int t_max_buffer_size,
               int t_database_size) {
  this->tenant_id = t_tenant_id;
  this->priority = t_priority;
  this->min_buffer_size = t_min_buffer_size;
  this->base_buffer_size = t_base_buffer_size;
  this->max_buffer_size = t_max_buffer_size;
  this->database_size = t_database_size;
};

Tenant::Tenant() : Tenant(0, 0, 0, 0, 0, 0){};