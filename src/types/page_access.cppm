export module page_access;

export struct PageAccess
{
    int tenant_id;
    int page_id;

    PageAccess(
        int t_tenant_id,
        int t_page_id)
    {
        this->tenant_id = t_tenant_id;
        this->page_id = t_page_id;
    }

    PageAccess() : PageAccess(0, 0) {}
};