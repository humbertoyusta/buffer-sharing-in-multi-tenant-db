## Description of the test case 10
## Four tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 9,
        'database_size': 200000,
        'minimum_buffer_size': 8000,
        'base_buffer_size': 17500,
        'maximum_buffer_size': 45000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 300000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 2,
        'priority_level': 10,
        'database_size': 200000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 20000,
        'maximum_buffer_size': 50000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 8000,
            'length': 175000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 2,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 3,
        'priority_level': 4,
        'database_size': 100000,
        'minimum_buffer_size': 2000,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 12500,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 4,
        'priority_level': 5,
        'database_size': 80000,
        'minimum_buffer_size': 2500,
        'base_buffer_size': 6500,
        'maximum_buffer_size': 16000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 120000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)