## Description of the test case 3
## Three tenants following distributions similar to 
## Taras Basiuk, Le Gruenwald, Laurent d’Orazio, Eleazar Leal. A Persistent Memory-Aware Buffer
## Pool Manager Simulator for Multi-Tenant Cloud Databases. International Workshop on Big Data
## Management on Emerging Hardware, Apr 2020, Dallas, United States. ffhal-02514273f 

tenants = [
    {
        'tenant_id': 1,
        'priority_level': 1,
        'database_size': 10000,
        'minimum_buffer_size': 100,
        'base_buffer_size': 1000,
        'maximum_buffer_size': 3000,
        'page_accesses_dist': {
            'type': 'uniform',
            'length': 40000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 40000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 2,
        'priority_level': 4,
        'database_size': 20000,
        'minimum_buffer_size': 1000,
        'base_buffer_size': 5000,
        'maximum_buffer_size': 18000,
        'page_accesses_dist': {
            'type': 'normal',
            'variance': 2000,
            'length': 200000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 200000,
            'max_value': 1,
        },
    },
    {
        'tenant_id': 3,
        'priority_level': 10,
        'database_size': 100000,
        'minimum_buffer_size': 10000,
        'base_buffer_size': 20000,
        'maximum_buffer_size': 60000,
        'page_accesses_dist': {
            'type': 'pareto',
            'parameter': 0.04,
            'length': 760000,
        },
        'pages_should_be_renumbered': False,
        'page_accesses_interval_dist': {
            'type': 'uniform',
            'length': 760000,
            'max_value': 1,
        },
    },
]

merge_policy = "full_merge_and_shuffle"
min_batch_length = 1
max_batch_length = 1

total_buffer_size = int(sum([tenant['base_buffer_size'] for tenant in tenants]) * 0.8)