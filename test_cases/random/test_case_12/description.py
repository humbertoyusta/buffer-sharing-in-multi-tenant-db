## Description of the test case 12
## Five tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 5,
        'database_size': 30000,
        'minimum_buffer_size': 2000,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 12500,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'uniform',
            'length': 10000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 2000,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 1000,
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 6,
        'database_size': 50000,
        'minimum_buffer_size': 2500,
        'base_buffer_size': 6500,
        'maximum_buffer_size': 16000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': variance,
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          } for variance in [4500, 1500, 2000, 7000, 3000, 2000, 5000]
        ],
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 140000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 3,
        'priority_level': 7,
        'database_size': 100000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 8000,
        'maximum_buffer_size': 20000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': parameter,
            'length': length,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          } for parameter, length in [(0.04, 40000), (0.05, 20000), (0.04, 50000), (0.05, 80000), (0.07, 10000)]
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 4,
        'priority_level': 8,
        'database_size': 200000,
        'minimum_buffer_size': 4000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 3000,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'uniform',
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 4000,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          }
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 5,
        'priority_level': 4,
        'database_size': 25000,
        'minimum_buffer_size': 1500,
        'base_buffer_size': 4000,
        'maximum_buffer_size': 10000,   
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 2000,
            'length': 10000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'uniform',
            'length': 10000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          }
        ],
        'pages_should_be_renumbered': True,
    }
]

merge_policy = "batch_merge"
min_batch_length = 1000
max_batch_length = 10000

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)