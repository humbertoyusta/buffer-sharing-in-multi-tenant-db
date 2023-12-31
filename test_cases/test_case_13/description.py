## Description of the test case 13
## Four tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 10,
        'database_size': 200000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 20000,
        'maximum_buffer_size': 50000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 160000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 160000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 160000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 160000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 5,
        'database_size': 50000,
        'minimum_buffer_size': 2500,
        'base_buffer_size': 6500,
        'maximum_buffer_size': 16000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 4500,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 4500,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 3,
        'priority_level': 5,
        'database_size': 50000,
        'minimum_buffer_size': 2500,
        'base_buffer_size': 6500,
        'maximum_buffer_size': 16000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 4,
        'priority_level': 6,
        'database_size': 70000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 7500,
        'maximum_buffer_size': 18500,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
]

merge_policy = "batch_merge"
min_batch_length = 50
max_batch_length = 400

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)