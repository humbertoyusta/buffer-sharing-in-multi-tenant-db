from .helpers import *
import numpy as np
from importlib import import_module
from . import constants
import random

class Generator:
    def __init__(self, test_number: int):
        self.test_number = test_number
        self.test_case = import_module(f"test_cases.test_case_{test_number}.description")
        if self.test_case is None:
            raise ModuleNotFoundError(f"Test case {test_number} description does not exist.")
        self.total_buffer_size = self.test_case.total_buffer_size
        self.merge_policy = self.test_case.merge_policy
        self.min_batch_length = self.test_case.min_batch_length
        self.max_batch_length = self.test_case.max_batch_length
        self.tenants = self.test_case.tenants
        self.input_file = f"test_cases/test_case_{test_number}/input.txt"
        np.random.seed(0)

    def generate_test_case(self):
        for tenant in self.test_case.tenants:
            current_tenant_page_accesses = []
            current_tenant_page_accesses_sum = 0

            for page_access_dist_batch in tenant['page_accesses_dist']:

                prv_length = current_tenant_page_accesses_sum

                permutation = np.arange(tenant['database_size']) + 1
                if tenant['pages_should_be_renumbered']:
                    permutation = np.random.permutation(tenant['database_size']) + 1

                while current_tenant_page_accesses_sum < prv_length + page_access_dist_batch['length']:
                    # Generate a batch of page accesses

                    batch_pages = np.array([])
                    if page_access_dist_batch['type'] == 'zipfian':
                        batch_pages = generate_zipfian_array(
                            page_access_dist_batch['parameter'], 
                            constants.BATCH_LENGTH, 
                            tenant['database_size'],
                        )
                    elif page_access_dist_batch['type'] == 'uniform':
                        batch_pages = generate_uniform_array(
                            constants.BATCH_LENGTH, 
                            tenant['database_size'],
                        )
                    elif page_access_dist_batch['type'] == 'normal':
                        batch_pages = generate_normal_array(
                            page_access_dist_batch['variance'], 
                            constants.BATCH_LENGTH, 
                            tenant['database_size'],
                        )
                    elif page_access_dist_batch['type'] == 'pareto':
                        batch_pages = generate_pareto_array(
                            page_access_dist_batch['parameter'], 
                            constants.BATCH_LENGTH, 
                            tenant['database_size'],
                        )
                    else:
                        raise Exception(f"Unknown distribution type: {page_access_dist_batch['type']}")
                    
                    # Generate a batch of page access intervals

                    batch_interval_lengths = np.array([])
                    if page_access_dist_batch['interval_type'] == 'uniform':
                        batch_interval_lengths = generate_uniform_array(
                            constants.BATCH_LENGTH, 
                            page_access_dist_batch['interval_max_value'],
                        )
                    elif page_access_dist_batch['interval_type'] == 'normal':
                        batch_interval_lengths = generate_normal_array(
                            page_access_dist_batch['interval_variance'], 
                            constants.BATCH_LENGTH, 
                            page_access_dist_batch['interval_max_value'],
                        )
                    elif page_access_dist_batch['interval_type'] == 'pareto':
                        batch_interval_lengths = generate_pareto_array(
                            page_access_dist_batch['interval_parameter'], 
                            constants.BATCH_LENGTH, 
                            page_access_dist_batch['interval_max_value'],
                        )
                    elif page_access_dist_batch['interval_type'] == 'zipfian':
                        batch_interval_lengths = generate_zipfian_array(
                            page_access_dist_batch['interval_parameter'], 
                            constants.BATCH_LENGTH, 
                            page_access_dist_batch['interval_max_value'],
                        )
                    else:
                        raise Exception(f"Unknown distribution type: {page_access_dist_batch['interval_type']}")

                    # Combine the two batches
                    batch = np.column_stack((batch_pages, batch_interval_lengths))
                    
                    # Add the batch to the current tenant page accesses, stopping if the tenant's page access length is reached
                    for batch_row in batch:
                        if current_tenant_page_accesses_sum + batch_row[1] <= prv_length + page_access_dist_batch['length']:
                            current_tenant_page_accesses.append([permutation[batch_row[0] - 1], batch_row[1]])
                            current_tenant_page_accesses_sum += batch_row[1]
                        else:
                            current_tenant_page_accesses.append([permutation[batch_row[0] - 1], prv_length + page_access_dist_batch['length'] - current_tenant_page_accesses_sum])
                            current_tenant_page_accesses_sum = prv_length + page_access_dist_batch['length']
                            break

            current_tenant_page_accesses = np.array(current_tenant_page_accesses)

            # Add tenant ID to the page accesses
            current_tenant_page_accesses = np.column_stack((
                np.full(current_tenant_page_accesses.shape[0], tenant['tenant_id']), 
                current_tenant_page_accesses[:, 0], 
                current_tenant_page_accesses[:, 1],
            ))

            tenant['page_accesses'] = current_tenant_page_accesses

        compressed_page_accesses = self._merge_page_accesses()
        
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
    
    def _merge_page_accesses(self):
        if self.merge_policy == "full_merge_and_shuffle":
            # Initialize an index list for each tenant
            tenant_indices = [i for i in range(len(self.test_case.tenants))]

            # Track the current access position for each tenant
            current_positions = [0] * len(self.test_case.tenants)

            # Prepare the container for the merged page accesses
            compressed_page_accesses = []

            # Calculate initial weights based on the number of page accesses for each tenant
            weights = [len(tenant['page_accesses']) for tenant in self.test_case.tenants]

            # Continue until all tenant accesses are merged
            while tenant_indices:
                # Randomly select a tenant index based on weights
                tenant_index = random.choices(tenant_indices, weights=weights, k=1)[0]
                tenant = self.test_case.tenants[tenant_index]

                # Add the next page access from this tenant
                compressed_page_accesses.append(tenant['page_accesses'][current_positions[tenant_index]])
                current_positions[tenant_index] += 1

                # Check if all accesses for this tenant are merged and update weights
                if current_positions[tenant_index] >= len(tenant['page_accesses']):
                    weights.pop(tenant_indices.index(tenant_index))
                    tenant_indices.remove(tenant_index)
        elif self.merge_policy == "batch_merge":
            compressed_page_accesses = []
            offsets = np.zeros(len(self.test_case.tenants) + 1, dtype=int)
            tenants_with_remaining_pages = [tenant['tenant_id'] for tenant in self.test_case.tenants]
            while len(tenants_with_remaining_pages) > 0:
                selected_tenant_id = np.random.choice(tenants_with_remaining_pages)
                batch_length = min(np.random.randint(self.min_batch_length, self.max_batch_length + 1), self.test_case.tenants[selected_tenant_id - 1]['page_accesses'].shape[0] - offsets[selected_tenant_id])
                batch = self.test_case.tenants[selected_tenant_id - 1]['page_accesses'][offsets[selected_tenant_id]:offsets[selected_tenant_id] + batch_length]
                compressed_page_accesses.extend(batch)
                
                offsets[selected_tenant_id] += batch_length
                if offsets[selected_tenant_id] == self.test_case.tenants[selected_tenant_id - 1]['page_accesses'].shape[0]:
                    tenants_with_remaining_pages.remove(selected_tenant_id)
        else:
            raise Exception(f"Unknown merge policy: {self.merge_policy}")
        
        return np.array(compressed_page_accesses)


            
