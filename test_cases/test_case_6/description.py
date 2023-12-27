## Description of the test case 6
## Ten tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 10,
        'database_size': 200000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 30000,
        'maximum_buffer_size': 100000,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 8000,
            'length': 400000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 400000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 2,
        'priority_level': 10,
        'database_size': 200000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 30000,
        'maximum_buffer_size': 100000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 400000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 400000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 3,
        'priority_level': 5,
        'database_size': 20000,
        'minimum_buffer_size': 1500,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 12500,
        'page_accesses_dist': {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 40000,
        },
        'pages_should_be_renumbered': True,
        'page_accesses_interval_dist': {
            'type': 'zipfian',
            'length': 40000,
            'parameter': 3,
            'max_value': 4000,
        },
    },
    {
        'tenant_id': 4,
        'priority_level': 4,
        'database_size': 20000,
        'minimum_buffer_size': 1200,
        'base_buffer_size': 4000,
        'maximum_buffer_size': 10000,
        'page_accesses_dist': {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 30000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 30000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 5,
        'priority_level': 4,
        'database_size': 15000,
        'minimum_buffer_size': 1200,
        'base_buffer_size': 4000,
        'maximum_buffer_size': 10000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 30000,
            'max_value': 3000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 30000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 6,
        'priority_level': 3,
        'database_size': 10000,
        'minimum_buffer_size': 1000,
        'base_buffer_size': 3000,
        'maximum_buffer_size': 7000,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 1000,
            'length': 20000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 20000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 7,
        'priority_level': 3,
        'database_size': 12000,
        'minimum_buffer_size': 1000,
        'base_buffer_size': 3000,
        'maximum_buffer_size': 7000,
        'page_accesses_dist': {
            'type': 'uniform',
            'length': 20000,
        },
        'pages_should_be_renumbered': True,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 20000,
            'max_value': 10,
        },
    },
    {
        'tenant_id': 8,
        'priority_level': 2,
        'database_size': 7500,
        'minimum_buffer_size': 500,
        'base_buffer_size': 1500,
        'maximum_buffer_size': 4000,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 500,
            'length': 18000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 18000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 9,
        'priority_level': 2,
        'database_size': 7500,
        'minimum_buffer_size': 500,
        'base_buffer_size': 1500,
        'maximum_buffer_size': 4000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 16000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 16000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 10,
        'priority_level': 2,
        'database_size': 10000,
        'minimum_buffer_size': 500,
        'base_buffer_size': 1500,
        'maximum_buffer_size': 4000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 16000,
        },
        'pages_should_be_renumbered': True,
        'page_accesses_interval_dist': {
            'type': 'pareto',
            'length': 16000,
            'parameter': 1,
            'max_value': 1000,
        },
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)