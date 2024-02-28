from importlib import import_module
from . import constants


class Validator:
    def __init__(
        self,
        test_number: int,
        test_type: str,
        has_description: bool = True,
        exp_number: int = None,
    ):
        self.test_number = test_number
        self.has_description = has_description
        if has_description:
            self.test_case = import_module(
                f"test_cases.{test_type}.test_case_{test_number}.description"
            )
            if self.test_case is None:
                raise ModuleNotFoundError(
                    f"Test case {test_number} description does not exist."
                )
        self.description = {
            "total_buffer_size": (
                self.test_case.total_buffer_size if has_description else 0
            ),
            "tenants": self.test_case.tenants if has_description else [],
        }
        if exp_number:
            self.input_file = f"test_cases/{test_type}/exp_{exp_number}/test_case_{test_number}/input.txt"
        else:
            self.input_file = (
                f"test_cases/{test_type}/test_case_{test_number}/input.txt"
            )

    def validate_total_buffer_size(self, total_buffer_size: int):
        assert (
            total_buffer_size >= constants.MIN_TOTAL_BUFFER_SIZE
        ), f"Total buffer size must be at least {constants.MIN_TOTAL_BUFFER_SIZE}."
        assert (
            total_buffer_size <= constants.MAX_TOTAL_BUFFER_SIZE
        ), f"Total buffer size must be at most {constants.MAX_TOTAL_BUFFER_SIZE}."

    def validate_page_accesses_length(self, page_accesses_length: int):
        assert (
            page_accesses_length <= constants.MAX_PAGE_ACCESSES_LENGTH
        ), f"Page accesses number must be at most {constants.MAX_PAGE_ACCESSES_LENGTH}."

    def validate_tenant_number(self, tenant_number: int):
        assert tenant_number > 0, f"Tenant number must be at least 1."
        assert (
            tenant_number <= constants.MAX_TENANTS
        ), f"Tenant number must be at most {constants.MAX_TENANTS}."

    def validate_priority_levels(self, priority_levels: list[int], tenant_number: int):
        assert len(priority_levels) == tenant_number, "Priority levels length mismatch."
        for priority_level in priority_levels:
            assert (
                priority_level >= constants.MIN_PRIORITY_LEVEL
            ), f"Priority level must be at least {constants.MIN_PRIORITY_LEVEL}."
            assert (
                priority_level <= constants.MAX_PRIORITY_LEVEL
            ), f"Priority level must be at most {constants.MAX_PRIORITY_LEVEL}."

    def validate_database_sizes(self, database_sizes: list[int], tenant_number: int):
        assert len(database_sizes) == tenant_number, "Database sizes length mismatch."
        for database_size in database_sizes:
            assert (
                database_size >= constants.MIN_DATABASE_SIZE
            ), f"Database size must be at least {constants.MIN_DATABASE_SIZE}."
            assert (
                database_size <= constants.MAX_DATABASE_SIZE
            ), f"Database size must be at most {constants.MAX_DATABASE_SIZE}."

    def validate_buffer_sizes(
        self,
        minimum_buffer_sizes: list[int],
        base_buffer_sizes: list[int],
        maximum_buffer_sizes: list[int],
        tenant_number: int,
        total_buffer_size: int,
    ):
        assert (
            len(minimum_buffer_sizes) == tenant_number
        ), "Minimum buffer sizes length mismatch."
        assert (
            len(base_buffer_sizes) == tenant_number
        ), "Base buffer sizes length mismatch."
        assert (
            len(maximum_buffer_sizes) == tenant_number
        ), "Maximum buffer sizes length mismatch."

        for tenant in range(tenant_number):
            assert (
                minimum_buffer_sizes[tenant] >= constants.MIN_BUFFER_SIZE
            ), f"Minimum buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert (
                minimum_buffer_sizes[tenant] <= constants.MAX_BUFFER_SIZE
            ), f"Minimum buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert (
                base_buffer_sizes[tenant] >= constants.MIN_BUFFER_SIZE
            ), f"Base buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert (
                base_buffer_sizes[tenant] <= constants.MAX_BUFFER_SIZE
            ), f"Base buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert (
                maximum_buffer_sizes[tenant] >= constants.MIN_BUFFER_SIZE
            ), f"Maximum buffer size must be at least {constants.MIN_BUFFER_SIZE}."
            assert (
                maximum_buffer_sizes[tenant] <= constants.MAX_BUFFER_SIZE
            ), f"Maximum buffer size must be at most {constants.MAX_BUFFER_SIZE}."
            assert (
                minimum_buffer_sizes[tenant] <= base_buffer_sizes[tenant]
            ), "Minimum buffer size must be at most base buffer size."
            assert (
                base_buffer_sizes[tenant] <= maximum_buffer_sizes[tenant]
            ), "Base buffer size must be at most maximum buffer size."
            assert (
                minimum_buffer_sizes[tenant] <= maximum_buffer_sizes[tenant]
            ), "Minimum buffer size must be at most maximum buffer size."

        assert (
            sum(minimum_buffer_sizes) <= total_buffer_size
        ), "Total buffer size must be at least the sum of minimum buffer sizes."

    def validate_page_access(
        self,
        tenant_id: int,
        page_id: int,
        tenant_number: int,
        database_sizes: list[int],
    ):
        assert tenant_id >= 1 and tenant_id <= tenant_number, "Tenant ID out of range."
        assert (
            page_id >= 1 and page_id <= database_sizes[tenant_id - 1]
        ), "Page ID out of range."

    def validate(self):
        """Validate the test case, including the description file (if it has) and the input file."""
        try:
            with open(self.input_file, "r") as f:
                lines = f.readlines()

                assert (
                    len(lines[0].split()) == 3
                ), "First line must contain three integers."
                tenant_number, total_buffer_size, page_accesses_length = map(
                    int, lines[0].split()
                )
                self.validate_total_buffer_size(total_buffer_size)
                self.validate_tenant_number(tenant_number)
                self.validate_page_accesses_length(page_accesses_length)

                if self.has_description:
                    assert tenant_number == len(
                        self.description["tenants"]
                    ), "Tenant number mismatch."
                    assert (
                        total_buffer_size == self.description["total_buffer_size"]
                    ), "Total buffer size mismatch."
                    assert page_accesses_length == sum(
                        page_access_batch["length"]
                        for tenant in self.description["tenants"]
                        for page_access_batch in tenant["page_accesses_dist"]
                    ), "Page accesses length mismatch."

                assert (
                    len(lines[1].split()) == tenant_number
                ), "Second line must contain tenant number integers."
                priority_levels = list(map(int, lines[1].split()))
                self.validate_priority_levels(priority_levels, tenant_number)

                if self.has_description:
                    for tenant in range(tenant_number):
                        assert (
                            priority_levels[tenant]
                            == self.description["tenants"][tenant]["priority_level"]
                        ), "Priority level mismatch."

                assert (
                    len(lines[2].split()) == tenant_number
                ), "Third line must contain tenant number integers."
                database_sizes = list(map(int, lines[2].split()))
                self.validate_database_sizes(database_sizes, tenant_number)

                if self.has_description:
                    for tenant in range(tenant_number):
                        assert (
                            database_sizes[tenant]
                            == self.description["tenants"][tenant]["database_size"]
                        ), "Database size mismatch."

                minimum_buffer_sizes = []
                base_buffer_sizes = []
                maximum_buffer_sizes = []

                for tenant in range(tenant_number):
                    assert (
                        len(lines[3 + tenant].split()) == 3
                    ), "Tenant line must contain three integers."
                    minimum_buffer_size, base_buffer_size, maximum_buffer_size = map(
                        int, lines[3 + tenant].split()
                    )

                    minimum_buffer_sizes.append(minimum_buffer_size)
                    base_buffer_sizes.append(base_buffer_size)
                    maximum_buffer_sizes.append(maximum_buffer_size)

                    if self.has_description:
                        assert (
                            minimum_buffer_size
                            == self.description["tenants"][tenant][
                                "minimum_buffer_size"
                            ]
                        ), "Minimum buffer size mismatch."
                        assert (
                            base_buffer_size
                            == self.description["tenants"][tenant]["base_buffer_size"]
                        ), "Base buffer size mismatch."
                        assert (
                            maximum_buffer_size
                            == self.description["tenants"][tenant][
                                "maximum_buffer_size"
                            ]
                        ), "Maximum buffer size mismatch."

                self.validate_buffer_sizes(
                    minimum_buffer_sizes,
                    base_buffer_sizes,
                    maximum_buffer_sizes,
                    tenant_number,
                    total_buffer_size,
                )

                page_accesses_per_tenant = [0] * (tenant_number + 1)

                for page_access in range(page_accesses_length):
                    assert (
                        len(lines[3 + tenant_number + page_access].split()) == 2
                    ), "Page access line must contain two integers."
                    tenant_id, page_id = map(
                        int, lines[3 + tenant_number + page_access].split()
                    )
                    self.validate_page_access(
                        tenant_id, page_id, tenant_number, database_sizes
                    )
                    page_accesses_per_tenant[tenant_id] += 1

                if self.has_description:
                    for tenant in range(1, tenant_number + 1):
                        assert page_accesses_per_tenant[tenant] == sum(
                            page_access_batch["length"]
                            for page_access_batch in self.description["tenants"][
                                tenant - 1
                            ]["page_accesses_dist"]
                        ), "Page accesses length per tenant mismatch."

        except FileNotFoundError:
            raise FileNotFoundError(f"Input file {self.input_file} not found.")
