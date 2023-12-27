from importlib import import_module
from . import constants


class Validator:
    def __init__(self, test_number: int):
        self.test_number = test_number
        self.test_case = import_module(f"test_cases.test_case_{test_number}.description")
        if self.test_case is None:
            raise ModuleNotFoundError(f"Test case {test_number} description does not exist.")
        self.total_buffer_size = self.test_case.total_buffer_size
        self.tenants = self.test_case.tenants
        self.input_file = f"test_cases/test_case_{test_number}/input.txt"

    def validate_total_buffer_size(self):
        assert self.total_buffer_size >= constants.MIN_TOTAL_BUFFER_SIZE, f"Total buffer size must be at least {constants.MIN_TOTAL_BUFFER_SIZE}."
        assert self.total_buffer_size <= constants.MAX_TOTAL_BUFFER_SIZE, f"Total buffer size must be at most {constants.MAX_TOTAL_BUFFER_SIZE}."
        
    def validate_tenants(self):
        assert len(self.tenants) > 0, "There must be at least one tenant."
        assert len(self.tenants) <= constants.MAX_TENANTS, f"There can be at most {constants.MAX_TENANTS} tenants."
        for tenant in self.tenants:
            assert tenant['priority_level'] >= constants.MIN_PRIORITY_LEVEL, f"Priority level must be at least {constants.MIN_PRIORITY_LEVEL}."
            assert tenant['priority_level'] <= constants.MAX_PRIORITY_LEVEL, f"Priority level must be at most {constants.MAX_PRIORITY_LEVEL}."
            assert tenant['minimum_buffer_size'] >= constants.MIN_BUFFER_SIZE, f"Minimum buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert tenant['minimum_buffer_size'] <= constants.MAX_BUFFER_SIZE, f"Minimum buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert tenant['base_buffer_size'] >= constants.MIN_BUFFER_SIZE, f"Base buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert tenant['base_buffer_size'] <= constants.MAX_BUFFER_SIZE, f"Base buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert tenant['maximum_buffer_size'] >= constants.MIN_BUFFER_SIZE, f"Maximum buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert tenant['maximum_buffer_size'] <= constants.MAX_BUFFER_SIZE, f"Maximum buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert tenant['database_size'] >= constants.MIN_DATABASE_SIZE, f"Database size must be at least {constants.MIN_DATABASE_SIZE}."
            assert tenant['database_size'] <= constants.MAX_DATABASE_SIZE, f"Database size must be at most {constants.MAX_DATABASE_SIZE}."
            assert tenant['minimum_buffer_size'] <= tenant['base_buffer_size'], "Minimum buffer size must be at most base buffer size."
            assert tenant['base_buffer_size'] <= tenant['maximum_buffer_size'], "Base buffer size must be at most maximum buffer size."
            assert tenant['minimum_buffer_size'] <= tenant['maximum_buffer_size'], "Minimum buffer size must be at most maximum buffer size."
            
        assert sum([tenant['minimum_buffer_size'] for tenant in self.tenants]) <= self.total_buffer_size, "Total buffer size must be at least the sum of minimum buffer sizes."
        assert sum([tenant['page_accesses_dist']['length'] for tenant in self.tenants]) <= constants.MAX_PAGE_ACCESSES_LENGTH, f"Total page accesses length must be at most {constants.MAX_PAGE_ACCESSES_LENGTH}."

    def validate_input_file(self):
        try:
            with open(self.input_file, 'r') as f:
                lines = f.readlines()

                assert len(lines[0].split()) == 3, "First line must contain three integers."
                tenant_number, total_buffer_size, page_accesses_number = map(int, lines[0].split())
                assert tenant_number == len(self.tenants), "Tenant number mismatch."
                assert total_buffer_size == self.total_buffer_size, "Total buffer size mismatch."
                assert page_accesses_number == sum([tenant['page_accesses_dist']['length'] for tenant in self.tenants]), "Page accesses number mismatch."

                assert len(lines[1].split()) == tenant_number, "Second line must contain tenant number integers."
                priority_levels = list(map(int, lines[1].split()))
                for i in range(tenant_number):
                    assert priority_levels[i] == self.tenants[i]['priority_level'], "Priority level mismatch."

                assert len(lines[2].split()) == tenant_number, "Third line must contain tenant number integers."
                database_sizes = list(map(int, lines[2].split()))
                for tenant in range(tenant_number):
                    assert database_sizes[tenant] == self.tenants[tenant]['database_size'], "Database size mismatch."

                for tenant in range(tenant_number):
                    assert len(lines[3 + tenant].split()) == 3, "Tenant line must contain three integers."
                    minimum_buffer_size, base_buffer_size, maximum_buffer_size = map(int, lines[3 + tenant].split())
                    assert minimum_buffer_size == self.tenants[tenant]['minimum_buffer_size'], "Minimum buffer size mismatch."
                    assert base_buffer_size == self.tenants[tenant]['base_buffer_size'], "Base buffer size mismatch."
                    assert maximum_buffer_size == self.tenants[tenant]['maximum_buffer_size'], "Maximum buffer size mismatch."

                page_accesses_per_tenant = [0] * (tenant_number + 1)

                for page_access in range(page_accesses_number):
                    assert len(lines[3 + tenant_number + page_access].split()) == 2, "Page access line must contain two integers."
                    tenant_id, page_id = map(int, lines[3 + tenant_number + page_access].split())
                    assert tenant_id >= 1 and tenant_id <= tenant_number, "Tenant ID out of range."
                    assert page_id >= 1 and page_id <= self.tenants[tenant_id - 1]['database_size'], f"Page ID {page_id} out of range."
                    page_accesses_per_tenant[tenant_id] += 1

                for tenant in range(1, tenant_number + 1):
                    assert page_accesses_per_tenant[tenant] == self.tenants[tenant - 1]['page_accesses_dist']['length'], "Page access length per tenant mismatch."

        except FileNotFoundError:
            raise FileNotFoundError(f"Input file {self.input_file} not found.")

    def validate(self):
        '''Validate the test case, including the description file and the input file.'''
        self.validate_total_buffer_size()
        self.validate_tenants()
        self.validate_input_file()
