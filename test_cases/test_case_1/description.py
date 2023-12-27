## Description of the test case 1
## Five tenants following different distributions of page accesses

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 5,
        'database_size': 20000,
        'minimum_buffer_size': 500,
        'base_buffer_size': 2000,
        'maximum_buffer_size': 5000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.05,
            'length': 100000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 100000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 2,
        'priority_level': 10,
        'database_size': 100000,
        'minimum_buffer_size': 3500,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 12500,
        'page_accesses_dist': {
            'type': 'zipfian',
            'parameter': 1.01,
            'length': 400000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 100000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 3,
        'priority_level': 3,
        'database_size': 5000,
        'minimum_buffer_size': 125,
        'base_buffer_size': 500,
        'maximum_buffer_size': 1250,
        'page_accesses_dist': {
            'type': 'uniform',
            'length': 10000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 100000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 4,
        'priority_level': 8,
        'database_size': 50000,
        'minimum_buffer_size': 1500,
        'base_buffer_size': 3000,
        'maximum_buffer_size': 8000,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 10000,
            'length': 300000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 100000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 5,
        'priority_level': 2,
        'database_size': 2500,
        'minimum_buffer_size': 40,
        'base_buffer_size': 200,
        'maximum_buffer_size': 500,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 500,
            'length': 5000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 100000,
            'max_value': 1,
        },
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)