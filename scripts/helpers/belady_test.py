from scripts.helpers.belady import BeladyCacheAlgorithm


def test_get_cache_faults_in_sequence():
    belady = BeladyCacheAlgorithm()

    first_seq = [0, 1, 1, 2, 2, 0, 2, 2, 1, 1, 0]
    assert belady.get_cache_faults_in_sequence(first_seq, 2) == 4

    second_seq = [1, 2, 3, 4, 5, 1, 2, 3, 4, 5]
    assert belady.get_cache_faults_in_sequence(second_seq, 3) == 7

    third_seq = [1, 2, 1, 3, 1, 3, 1, 2]
    assert belady.get_cache_faults_in_sequence(third_seq, 2) == 4


def test__init_next_accesses_by_index():
    belady = BeladyCacheAlgorithm()

    first_seq = [0, 1, 1, 2, 2, 0, 2, 2, 1, 1, 0]
    belady._init_next_accesses_by_index(first_seq)
    assert belady.next_accesses_by_index == [
        5,
        2,
        8,
        4,
        6,
        10,
        7,
        BeladyCacheAlgorithm.INF,
        9,
        BeladyCacheAlgorithm.INF,
        BeladyCacheAlgorithm.INF,
    ]


def test__init_next_accesses_by_value():
    belady = BeladyCacheAlgorithm()

    first_seq = [0, 1, 1, 3, 3, 0, 3, 3, 1, 1, 0]
    belady._init_next_accesses_by_value(first_seq)
    assert belady.next_accesses_by_value == [0, 1, BeladyCacheAlgorithm.INF, 3]


def test__get_element_to_remove():
    belady = BeladyCacheAlgorithm()

    belady.cache = [1, 3, 4]
    belady.next_accesses_by_value = [0, 2, 1, 4, 3]

    assert belady._get_element_to_remove(belady.cache, 0) == 3
