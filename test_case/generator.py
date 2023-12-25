from test_case.helpers import *
import numpy as np
from importlib import import_module
from test_case import constants

class Generator:
    def __init__(self, test_number: int):
        self.test_number = test_number
        self.test_case = import_module(f"test_cases.test_case_{test_number}.description")
        if self.test_case is None:
            raise ModuleNotFoundError(f"Test case {test_number} description does not exist.")
        self.total_buffer_size = self.test_case.total_buffer_size
        self.tenants = self.test_case.tenants
        self.input_file = f"test_cases/test_case_{test_number}/input.txt"
        np.random.seed(0)

    def generate_test_case(self):
        for tenant in self.test_case.tenants:
            current_tenant_page_accesses = []
            current_tenant_page_accesses_sum = 0

            while current_tenant_page_accesses_sum < tenant['page_accesses_dist']['length']:
                # Generate a batch of page accesses

                batch_pages = np.array([])
                if tenant['page_accesses_dist']['type'] == 'zipfian':
                    batch_pages = generate_zipfian_array(
                        tenant['page_accesses_dist']['parameter'], 
                        constants.BATCH_LENGTH, 
                        tenant['database_size'],
                    )
                elif tenant['page_accesses_dist']['type'] == 'uniform':
                    batch_pages = generate_uniform_array(
                        constants.BATCH_LENGTH, 
                        tenant['database_size'],
                    )
                elif tenant['page_accesses_dist']['type'] == 'normal':
                    batch_pages = generate_normal_array(
                        tenant['page_accesses_dist']['variance'], 
                        constants.BATCH_LENGTH, 
                        tenant['database_size'],
                    )
                elif tenant['page_accesses_dist']['type'] == 'pareto':
                    batch_pages = generate_pareto_array(
                        tenant['page_accesses_dist']['parameter'], 
                        constants.BATCH_LENGTH, 
                        tenant['database_size'],
                    )
                else:
                    raise Exception(f"Unknown distribution type: {tenant['page_accesses_dist']['type']}")
                
                # Generate a batch of page access intervals

                batch_interval_lengths = np.array([])
                if tenant['page_accesses_interval_dist']['type'] == 'uniform':
                    batch_interval_lengths = generate_uniform_array(
                        constants.BATCH_LENGTH, 
                        tenant['page_accesses_interval_dist']['max_value'],
                    )
                elif tenant['page_accesses_interval_dist']['type'] == 'normal':
                    batch_interval_lengths = generate_normal_array(
                        tenant['page_accesses_interval_dist']['variance'], 
                        constants.BATCH_LENGTH, 
                        tenant['page_accesses_interval_dist']['max_value'],
                    )
                elif tenant['page_accesses_interval_dist']['type'] == 'pareto':
                    batch_interval_lengths = generate_pareto_array(
                        tenant['page_accesses_interval_dist']['parameter'], 
                        constants.BATCH_LENGTH, 
                        tenant['page_accesses_interval_dist']['max_value'],
                    )
                elif tenant['page_accesses_interval_dist']['type'] == 'zipfian':
                    batch_interval_lengths = generate_zipfian_array(
                        tenant['page_accesses_interval_dist']['parameter'], 
                        constants.BATCH_LENGTH, 
                        tenant['page_accesses_interval_dist']['max_value'],
                    )
                else:
                    raise Exception(f"Unknown distribution type: {tenant['page_accesses_interval_dist']['type']}")

                # Combine the two batches
                batch = np.column_stack((batch_pages, batch_interval_lengths))
                
                # Add the batch to the current tenant page accesses, stopping if the tenant's page access length is reached
                for batch_row in batch:
                    if current_tenant_page_accesses_sum + batch_row[1] <= tenant['page_accesses_dist']['length']:
                        current_tenant_page_accesses.append([batch_row[0], batch_row[1]])
                        current_tenant_page_accesses_sum += batch_row[1]
                    else:
                        current_tenant_page_accesses.append([batch_row[0], tenant['page_accesses_dist']['length'] - current_tenant_page_accesses_sum])
                        current_tenant_page_accesses_sum = tenant['page_accesses_dist']['length']
                        break

            current_tenant_page_accesses = np.array(current_tenant_page_accesses)

            # Renumber pages if necessary, applying the permutation to the page access intervals
            if tenant['pages_should_be_renumbered']:
                permutation = np.random.permutation(tenant['database_size'] + 1)
                current_tenant_page_accesses[:, 0] = permutation[current_tenant_page_accesses[:, 0].astype(int)]

            # Add tenant ID to the page accesses
            current_tenant_page_accesses = np.column_stack((
                np.full(current_tenant_page_accesses.shape[0], tenant['tenant_id']), 
                current_tenant_page_accesses[:, 0], 
                current_tenant_page_accesses[:, 1],
            ))

            tenant['page_accesses'] = current_tenant_page_accesses

        # Merge the page accesses of all tenants
        compressed_page_accesses = np.concatenate([tenant['page_accesses'] for tenant in self.test_case.tenants])
        np.random.shuffle(compressed_page_accesses)
        
        full_page_accesses = []

        # Expand the page accesses to the full length, first they were compressed in intervals
        for page_access in compressed_page_accesses:
            current_page = page_access[1]
            for _ in range(page_access[2]):
                full_page_accesses.append([page_access[0], current_page])
                current_page += 1
                if current_page > self.test_case.tenants[page_access[0] - 1]['database_size']:
                    current_page = 1

        return {
            'total_buffer_size': self.test_case.total_buffer_size,
            'tenants': self.test_case.tenants,
            'page_accesses': np.array(full_page_accesses),
        }
            
