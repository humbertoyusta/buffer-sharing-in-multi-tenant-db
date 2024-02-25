## Description of the test case 8
## Ten tenants following different distributions of page accesses,
## Like test 7, but with bigger buffer sizes

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 5000,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 2,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 3,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
          {
            'type': 'normal',
            'variance': 5000,
            'length': 50000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 4,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 50000,
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
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'uniform',
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 1,
          },
        ],
        'pages_should_be_renumbered': False,
    },
    {
        'tenant_id': 6,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'normal',
            'variance': 3000,
            'length': 100000,
            'interval_type': 'normal',
            'interval_variance': 4,
            'interval_max_value': 100,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 7,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 100000,
            'interval_type': 'zipfian',
            'interval_parameter': 2,
            'interval_max_value': 200000,
          },
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 8,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 50000,
            'interval_type': 'pareto',
            'interval_parameter': 1,
            'interval_max_value': 10000,
          },
          {
            'type': 'normal',
            'variance': 5000,
            'length': 50000,
            'interval_type': 'normal',
            'interval_variance': 4,
            'interval_max_value': 100,
          }
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 9,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'zipfian',
            'parameter': 1.005,
            'length': 50000,
            'interval_type': 'zipfian',
            'interval_parameter': 2,
            'interval_max_value': 200000,
          },
          {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 50000,
            'interval_type': 'pareto',
            'interval_parameter': 1,
            'interval_max_value': 10000,
          }
        ],
        'pages_should_be_renumbered': True,
    },
    {
        'tenant_id': 10,
        'priority_level': 5,
        'database_size': 200000,
        'minimum_buffer_size': 5000,
        'base_buffer_size': 10000,
        'maximum_buffer_size': 25000,
        'page_accesses_dist': [
          {
            'type': 'uniform',
            'length': 100000,
            'interval_type': 'uniform',
            'interval_max_value': 20,
          },
        ],
        'pages_should_be_renumbered': True,
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)