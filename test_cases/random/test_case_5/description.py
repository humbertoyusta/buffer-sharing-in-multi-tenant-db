## Description of the test case 5
## Five tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 10,
        'database_size': 200000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 30000,
        'maximum_buffer_size': 100000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 10000,
            'length': 75000,
            'interval_type': 'uniform', 
            'interval_max_value': 1,
          },
        ] * 10,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 3,
        'database_size': 20000,
        'minimum_buffer_size': 1000,
        'base_buffer_size': 3000,
        'maximum_buffer_size': 10000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.05,
            'length': 40000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 3,
        'priority_level': 4,
        'database_size': 10000,
        'minimum_buffer_size': 1500,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 12500,
        'page_accesses_dist': [
          {
            'type': 'uniform',
            'length': 80000,
            'interval_type': 'uniform',
            'interval_max_value': 10,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 4,
        'priority_level': 2,
        'database_size': 20000,
        'minimum_buffer_size': 800,
        'base_buffer_size': 2000,
        'maximum_buffer_size': 5000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],  
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 5,
        'priority_level': 4,
        'database_size': 25000,
        'minimum_buffer_size': 1500,
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
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)