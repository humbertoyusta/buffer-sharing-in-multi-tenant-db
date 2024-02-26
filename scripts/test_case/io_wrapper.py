import os


class IoWrapper:
    def __init__(self, test_case, test_number: int, test_type: str):
        self.test_case = test_case
        self.test_number = test_number
        self.input_file = f"test_cases/{test_type}/test_case_{test_number}/input.txt"

    def print_test_case(self):
        os.makedirs(os.path.dirname(self.input_file), exist_ok=True)

        with open(self.input_file, "w") as f:
            f.write(
                f"{len(self.test_case['tenants'])} {self.test_case['total_buffer_size']} {len(self.test_case['page_accesses'])}\n"
            )
            f.write(
                " ".join(
                    [
                        str(tenant["priority_level"])
                        for tenant in self.test_case["tenants"]
                    ]
                )
                + "\n"
            )
            f.write(
                " ".join(
                    [
                        str(tenant["database_size"])
                        for tenant in self.test_case["tenants"]
                    ]
                )
                + "\n"
            )
            for tenant in self.test_case["tenants"]:
                f.write(
                    f"{tenant['minimum_buffer_size']} {tenant['base_buffer_size']} {tenant['maximum_buffer_size']}\n"
                )
            for page_access in self.test_case["page_accesses"]:
                f.write(f"{page_access[0]} {page_access[1]}\n")
