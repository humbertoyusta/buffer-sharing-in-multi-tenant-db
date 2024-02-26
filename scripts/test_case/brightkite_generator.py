import pandas as pd
from ..helpers.belady import BeladyCacheAlgorithm


class BrightkiteGenerator:
    INITIAL_BUFFER_SIZE = 40
    EXTRA_CACHE_FAULTS_THRESHOLD = 40
    CSV_FILE_NAMES = [
        "brightkite_4_tenants_1.csv",
        "brightkite_4_tenants_2.csv",
        "brightkite_5_tenants_1.csv",
        "brightkite_5_tenants_2.csv",
        "brightkite_8_tenants_1.csv",
        "brightkite_10_tenants_1.csv",
    ]

    def __init__(self, test_number: int):
        self.test_number = test_number
        self.csv_file = f"data/{self.CSV_FILE_NAMES[test_number - 1]}"

    def generate_test_case(self):
        df = pd.read_csv(self.csv_file)
        page_accesses = [tuple(map(int, row)) for row in df.values]

        tenant_number = int(self.csv_file.split("_")[1])

        self.tenants = [
            {
                "tenant_id": tenant_id,
                "page_accesses_sequence": [
                    page_access[1]
                    for page_access in page_accesses
                    if page_access[0] == tenant_id
                ],
                "database_size": max(
                    page_access[1]
                    for page_access in page_accesses
                    if page_access[0] == tenant_id
                ),
            }
            for tenant_id in range(1, tenant_number + 1)
        ]

        max_database_size = max([tenant["database_size"] for tenant in self.tenants])
        max_sequence_length = max(
            [len(tenant["page_accesses_sequence"]) for tenant in self.tenants]
        )

        for tenant in self.tenants:
            base_buffer_size = self.INITIAL_BUFFER_SIZE

            while True:
                belady = BeladyCacheAlgorithm()
                cache_faults = belady.get_cache_faults_in_sequence(
                    tenant["page_accesses_sequence"], base_buffer_size
                )
                if (
                    cache_faults - tenant["database_size"]
                    <= self.EXTRA_CACHE_FAULTS_THRESHOLD
                ):
                    break
                base_buffer_size += 10

            tenant["base_buffer_size"] = base_buffer_size
            tenant["minimum_buffer_size"] = int(float(base_buffer_size) * 0.4)
            tenant["maximum_buffer_size"] = int(float(base_buffer_size) * 2.5)

            tenant["priority_level"] = round(
                (
                    tenant["database_size"] / max_database_size
                    + len(tenant["page_accesses_sequence"]) / max_sequence_length
                )
                * 5
            )

        return {
            "tenants": self.tenants,
            "total_buffer_size": int(
                sum([tenant["base_buffer_size"] for tenant in self.tenants]) * 0.8
            ),
            "page_accesses": page_accesses,
        }
