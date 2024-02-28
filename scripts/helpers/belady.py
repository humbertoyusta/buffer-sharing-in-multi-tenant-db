from sortedcontainers import SortedDict


class BeladyCacheAlgorithm:
    INF = 10**9 + 7

    def __init__(self):
        # The next access of each element in the sequence, by index in the sequence
        self.next_accesses_by_index: list[int] = []
        # The future access of each element in the sequence, by value of the element
        self.next_accesses_by_value: list[int] = []

        self.next_access_to_value: SortedDict[int, int] = SortedDict()

        self.value_to_next_access: SortedDict[int, int] = SortedDict()

    def get_cache_faults_in_sequence(self, sequence: list[int], cache_size: int) -> int:
        """
        @param sequence: The sequence of cache keys accessed.
        @param cache_size: The size of the cache.
        @return: The number of cache faults in the sequence, using Belady's algorithm.
        """
        self.next_access_to_value.clear()
        self.value_to_next_access.clear()

        cache_faults: int = 0

        self._init_next_accesses_by_value(sequence)
        self._init_next_accesses_by_index(sequence)

        for index, value in enumerate(sequence):

            if value not in self.value_to_next_access:
                cache_faults += 1

                if len(self.next_access_to_value) == cache_size:
                    next_access_to_remove, value_to_remove = (
                        self.next_access_to_value.popitem(index=-1)
                    )
                    self.value_to_next_access.pop(value_to_remove)

                # Insert the current element with its next access time
                self.next_access_to_value[index] = value
                self.value_to_next_access[value] = index

            self._update_next_accesses(index, value)
            self.next_accesses_by_value[value] = self.next_accesses_by_index[index]

        return cache_faults

    def _update_next_accesses(self, index: int, value: int):
        """
        Update the next access of the element that was accessed.
        """
        next_access = self.next_accesses_by_index[index]

        if value in self.value_to_next_access:
            self.next_access_to_value.pop(self.value_to_next_access[value])

        self.next_access_to_value[next_access] = value
        self.value_to_next_access[value] = next_access

    def _init_next_accesses_by_index(self, sequence: list[int]):
        """
        Initialize the next_accesses_by_index list, which contains the next access of each element
        in the sequence, by index in the sequence.
        """
        last_seen = [self.INF] * (max(sequence) + 1)

        self.next_accesses_by_index = [self.INF] * len(sequence)

        for i in range(len(sequence) - 1, -1, -1):
            self.next_accesses_by_index[i] = last_seen[sequence[i]]

            last_seen[sequence[i]] = i

    def _init_next_accesses_by_value(self, sequence):
        """
        Initialize the next_accesses_by_value list, which contains the next access of each element
        in the sequence, by value of the element.
        """
        self.next_accesses_by_value = [self.INF] * (max(sequence) + 1)

        for index, value in enumerate(sequence):
            if self.next_accesses_by_value[value] == self.INF:
                self.next_accesses_by_value[value] = index
