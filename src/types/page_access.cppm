export module page_access;

export struct PageAccess{
    int tenant_id;
    int page_id;

    PageAccess(
        int tenant_id, 
        int page_id
    ){
        this->tenant_id = tenant_id;
        this->page_id = page_id;
    }

    PageAccess() : PageAccess(0, 0) {}
};