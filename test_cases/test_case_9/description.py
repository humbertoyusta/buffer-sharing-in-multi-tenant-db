## Description of the test case 9
## Five tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 8,
        'database_size': 20000,
        'minimum_buffer_size': 4000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 3000,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 5,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 6,
        'database_size': 200000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 7500,
        'maximum_buffer_size': 18000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 2,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 3,
        'priority_level': 5,
        'database_size': 150000,
        'minimum_buffer_size': 2500,
        'base_buffer_size': 6000,
        'maximum_buffer_size': 15000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 4,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 4,
        'priority_level': 9,
        'database_size': 40000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 12500,
        'maximum_buffer_size': 30000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 300000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 5,
        'priority_level': 6,
        'database_size': 100000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 7500,
        'maximum_buffer_size': 18000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 4000,
            'length': 60000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 2,
        'pages_should_be_renumbered': True,
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)