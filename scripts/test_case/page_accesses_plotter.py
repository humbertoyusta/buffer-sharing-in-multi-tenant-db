import matplotlib.pyplot as plt
import numpy as np
from . import constants
import seaborn as sns


class PageAccessesPlotter:
    def __init__(self, page_accesses, tenant_number: int, test_number: int):
        self.page_accesses = page_accesses
        self.tenant_number = tenant_number
        self.test_number = test_number
        self.page_accesses_per_tenant = self._counting_renumber(page_accesses, tenant_number)

    def plot(self):
        self.plot_histograms()
        self.plot_distributions()

    def plot_histograms(self):
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

        plt.savefig(f"test_cases/test_case_{self.test_number}/page_accesses_histograms.png")

    def plot_distributions(self):
        num_columns = 3
        num_rows = (self.tenant_number + num_columns - 1) // num_columns
        fig, axes = plt.subplots(num_rows, num_columns, figsize=(20, num_rows * 5))

        if num_rows == 1 or num_columns == 1:
            axes = np.array(axes).reshape(num_rows, num_columns)

        for i in range(self.tenant_number):
            ax = axes[i // num_columns, i % num_columns]
            sns.kdeplot(self.page_accesses_per_tenant[i + 1], ax=ax)
            ax.set_title(f"Tenant {i + 1}")

        for i in range(self.tenant_number, num_columns * num_rows):
            axes[i // num_columns, i % num_columns].axis('off')

        plt.savefig(f"test_cases/test_case_{self.test_number}/page_accesses_distributions.png")
        plt.close()

    def _renumber(self, page_accesses_per_tenant):
        current = 1
        for i in range(len(page_accesses_per_tenant)):
            if i > 0 and page_accesses_per_tenant[i] != page_accesses_per_tenant[i - 1]:
                current += 1
            page_accesses_per_tenant[i] = current
        return
    
    def _counting_renumber(self, page_accesses, tenant_number):
        '''Takes a list of page accesses and renumbers them so that the most frequent 
        page accessed is 1, the second most frequent page accessed is 2, etc, keeping 
        the frequencies the same. Used for plotting page accesses distributions, without
        considering the number (ID) of the page accessed, instead considering the frequency
        '''
        page_access_freq_per_tenant = [[0] * (constants.MAX_DATABASE_SIZE + 1) for _ in range(tenant_number + 1)]

        for page_access in page_accesses:
            page_access_freq_per_tenant[page_access[0]][page_access[1]] += 1

        for i in range(tenant_number + 1):
            page_access_freq_per_tenant[i].sort(reverse=True)

        result = [[] for _ in range(tenant_number + 1)]

        for i in range(tenant_number + 1):
            for j in range(constants.MAX_DATABASE_SIZE + 1):
                if page_access_freq_per_tenant[i][j] > 0:
                    result[i].extend([j] * page_access_freq_per_tenant[i][j])

        return result


