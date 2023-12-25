import matplotlib.pyplot as plt
import numpy as np


class PageAccessesPlotter:
    def __init__(self, page_accesses, tenant_number: int, test_number: int):
        self.page_accesses = page_accesses
        self.tenant_number = tenant_number
        self.test_number = test_number

        self.page_accesses_per_tenant = [[] for _ in range(tenant_number + 1)]

        for page_access in page_accesses:
            self.page_accesses_per_tenant[page_access[0]].append(page_access[1])

        for page_accesses_per_tenant in self.page_accesses_per_tenant:
            page_accesses_per_tenant.sort()
            self._renumber(page_accesses_per_tenant)

    def plot(self):
        num_columns = 3
        num_rows = (self.tenant_number + num_columns - 1) // num_columns
        fig, axes = plt.subplots(num_rows, num_columns, figsize=(20, num_rows * 5))

        if num_rows == 1 or num_columns == 1:
            axes = np.array(axes).reshape(num_rows, num_columns)

        for i in range(self.tenant_number):
            ax = axes[i // num_columns, i % num_columns]
            ax.hist(self.page_accesses_per_tenant[i + 1], bins=100)
            ax.set_title(f"Tenant {i + 1}")

        for i in range(self.tenant_number, num_columns * num_rows):
            axes[i // num_columns, i % num_columns].axis('off')

        plt.savefig(f"test_cases/test_case_{self.test_number}/page_accesses_distributions.png")

    def _renumber(self, page_accesses_per_tenant):
        current = 1
        for i in range(len(page_accesses_per_tenant)):
            if i > 0 and page_accesses_per_tenant[i] != page_accesses_per_tenant[i - 1]:
                current += 1
            page_accesses_per_tenant[i] = current
        return

