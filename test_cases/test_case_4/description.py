## Description of the test case 4
## Ten tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 7,
        'database_size': 100000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 20000,
        'maximum_buffer_size': 50000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 10000,
            'length': 30000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 5,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 2,
        'priority_level': 6,
        'database_size': 100000,
        'minimum_buffer_size': 4000,
        'base_buffer_size': 15000,
        'maximum_buffer_size': 40000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 20000,
            'length': 20000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 6,
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 120000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 3,
        'priority_level': 8,
        'database_size': 100000,
        'minimum_buffer_size': 8000,
        'base_buffer_size': 25000,
        'maximum_buffer_size': 60000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.05,
            'length': 50000,
            'interval_type': 'pareto',
            'interval_parameter': 0.5,
            'interval_max_value': 5000,
          },
        ] * 3,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 4,
        'priority_level': 7,
        'database_size': 100000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 20000,
        'maximum_buffer_size': 50000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 40000,
            'interval_type': 'zipfian',
            'interval_parameter': 2,
            'interval_max_value': 100000,
          },
        ] * 3,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 5,
        'priority_level': 10,
        'database_size': 180000,
        'minimum_buffer_size': 20000,
        'base_buffer_size': 40000,
        'maximum_buffer_size': 100000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 5000,
            'length': 50000,
            'interval_type': 'normal',
            'interval_variance': 10,
            'interval_max_value': 100000,
          },
        ] * 4,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 6,
        'priority_level': 5,
        'database_size': 80000,
        'minimum_buffer_size': 4000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 30000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 5000,
            'length': 30000,
            'interval_type': 'normal',
            'interval_variance': 3,
            'interval_max_value': 80000,
          },
        ] * 4,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 7,
        'priority_level': 9,
        'database_size': 200000,
        'minimum_buffer_size': 14000,
        'base_buffer_size': 30000,
        'maximum_buffer_size': 80000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 4000,
            'length': 10000,
            'interval_type': 'zipfian',
            'interval_parameter': 2,
            'interval_max_value': 30000,
          },
        ] * 4,
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 8,
        'priority_level': 5,
        'database_size': 100000,
        'minimum_buffer_size': 4000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 30000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ] * 2,
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 9,
        'priority_level': 3,
        'database_size': 100000,
        'minimum_buffer_size': 2000,
        'base_buffer_size': 6000,
        'maximum_buffer_size': 15000,
        'page_accesses_dist': [
          {
            'type': 'uniform',
            'length': 25000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 10,
        'priority_level': 4,
        'database_size': 60000,
        'minimum_buffer_size': 3000,
        'base_buffer_size': 7500,
        'maximum_buffer_size': 20000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 5000,
            'length': 25000,
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