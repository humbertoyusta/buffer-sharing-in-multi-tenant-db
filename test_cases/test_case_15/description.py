## Description of the test case 15
## x tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 5,
        'database_size': 10000,
        'minimum_buffer_size': 400,
        'base_buffer_size': 1000,
        'maximum_buffer_size': 2500,
        'page_accesses_dist': [
          {
            'type': 'pareto_with_correlated_references',
            'first_correlated_reference_min_distance': 2.0,
            'first_correlated_reference_max_distance': 5.0,
            'correlated_reference_distance_growth_factor': 1.8,
            'max_correlated_references': 10,
            'parameter': 0.05,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 2,
        'priority_level': 7,
        'database_size': 10000,
        'minimum_buffer_size': 1000,
        'base_buffer_size': 1800,
        'maximum_buffer_size': 5000,
        'page_accesses_dist': [
          {
            'type': 'pareto_with_correlated_references',
            'first_correlated_reference_min_distance': 1.5,
            'first_correlated_reference_max_distance': 4.0,
            'correlated_reference_distance_growth_factor': 1.7,
            'max_correlated_references': 8,
            'parameter': 0.05,
            'length': 200000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 3,
        'priority_level': 6,
        'database_size': 10000,
        'minimum_buffer_size': 500,
        'base_buffer_size': 1400,
        'maximum_buffer_size': 4000,
        'page_accesses_dist': [
          {
            'type': 'zipfian_with_correlated_references',
            'first_correlated_reference_min_distance': 1.5,
            'first_correlated_reference_max_distance': 6.0,
            'correlated_reference_distance_growth_factor': 2.0,
            'max_correlated_references': 14,
            'parameter': 1.01,
            'length': 160000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 4,
        'priority_level': 4,
        'database_size': 6000,
        'minimum_buffer_size': 500,
        'base_buffer_size': 800,
        'maximum_buffer_size': 4000,
        'page_accesses_dist': [
          {
            'type': 'normal_with_correlated_references',
            'first_correlated_reference_min_distance': 1.5,
            'first_correlated_reference_max_distance': 6.0,
            'correlated_reference_distance_growth_factor': 2.0,
            'max_correlated_references': 14,
            'variance': 400,
            'length': 80000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)