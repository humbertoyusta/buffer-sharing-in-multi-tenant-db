import pandas as pd
from ..helpers.belady import BeladyCacheAlgorithm
import math


class CitibikeGenerator:
    INITIAL_BUFFER_SIZES_PER_EXP = [
        5000,
        2000,
        50,
    ]
    BUFFER_SIZE_INCREMENT_PER_EXP = [
        1000,
        400,
        10,
    ]
    EXTRA_CACHE_FAULTS_RATIO_THRESHOLD_PER_EXP = [
        0.2,
        0.1,
        0.05,
    ]

    def __init__(self, exp_number: int, test_number: int):
        self.exp_number = exp_number
        self.test_number = test_number
        self.csv_file = f"data/citibike_exp_{exp_number}_case_{test_number}.csv"

    def generate_test_case(self):
        df = pd.read_csv(self.csv_file)
        page_accesses = [tuple(map(int, row)) for row in df.values]

        num_tenants = df["tenant_id"].nunique()

        self.tenants = [
            {
                "tenant_id": tenant_id,
                "database_size": max(
                    page_access[1]
                    for page_access in page_accesses
                    if page_access[0] == tenant_id
                ),
                "page_accesses_sequence": [
                    page_access[1]
                    for page_access in page_accesses
                    if page_access[0] == tenant_id
                ],
            }
            for tenant_id in range(1, num_tenants + 1)
        ]

        for tenant in self.tenants:
            assert tenant["database_size"] == len(set(tenant["page_accesses_sequence"]))

        max_database_size = max([tenant["database_size"] for tenant in self.tenants])
        max_sequence_length = max(
            [len(tenant["page_accesses_sequence"]) for tenant in self.tenants]
        )

        for tenant in self.tenants:
            base_buffer_size = self.INITIAL_BUFFER_SIZES_PER_EXP[self.exp_number - 1]

            count = 0
            while True:
                belady = BeladyCacheAlgorithm()
                count += 1
                cache_faults = belady.get_cache_faults_in_sequence(
                    tenant["page_accesses_sequence"], base_buffer_size
                )
                if (cache_faults - tenant["database_size"]) / (
                    len(tenant["page_accesses_sequence"]) - tenant["database_size"]
                ) <= self.EXTRA_CACHE_FAULTS_RATIO_THRESHOLD_PER_EXP[
                    self.exp_number - 1
                ]:
                    break
                else:
                    base_buffer_size += self.BUFFER_SIZE_INCREMENT_PER_EXP[
                        self.exp_number - 1
                    ]

            tenant["base_buffer_size"] = base_buffer_size
            tenant["minimum_buffer_size"] = int(float(base_buffer_size) * 0.4)
            tenant["maximum_buffer_size"] = int(float(base_buffer_size) * 2.2)

            tenant["priority_level"] = round(
                (
                    math.sqrt(tenant["database_size"]) / math.sqrt(max_database_size)
                    + math.sqrt(len(tenant["page_accesses_sequence"]))
                    / math.sqrt(max_sequence_length)
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
