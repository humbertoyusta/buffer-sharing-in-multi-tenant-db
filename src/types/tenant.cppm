export module tenant;

export struct Tenant{
    int tenant_id;
    int priority;
    int min_buffer_size;
    int base_buffer_size;
    int max_buffer_size;
    int database_size;

    Tenant(
        int tenant_id, 
        int priority, 
        int min_buffer_size, 
        int base_buffer_size, 
        int max_buffer_size, 
        int database_size
    ) {
        this->tenant_id = tenant_id;
        this->priority = priority;
        this->min_buffer_size = min_buffer_size;
        this->base_buffer_size = base_buffer_size;
        this->max_buffer_size = max_buffer_size;
        this->database_size = database_size;
    }

    Tenant() : Tenant(0, 0, 0, 0, 0, 0) {}
};