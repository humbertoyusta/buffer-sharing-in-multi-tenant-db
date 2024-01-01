## Description of the test case 11
## Five tenants following different distributions of page accesses

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
            'length': 200000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'uniform',
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 8000,
            'length': 80000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 8,
        'database_size': 200000,
        'minimum_buffer_size': 6750,
        'base_buffer_size': 15000,
        'maximum_buffer_size': 37500,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 8000,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'uniform',
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 120000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
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
            'type': 'uniform',
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 80000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
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
            'type': 'normal',
            'variance': 4500,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          }
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 5,
        'priority_level': 6,
        'database_size': 120000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 8000,
        'maximum_buffer_size': 20000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 70000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 4500,
            'length': 10000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 70000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    }
]

merge_policy = "batch_merge"
min_batch_length = 100
max_batch_length = 1000

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)