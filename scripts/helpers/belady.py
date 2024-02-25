class BeladyCacheAlgorithm:
    INF = 10**9 + 7

    def __init__(self):
        # The next access of each element in the sequence, by index in the sequence
        self.next_accesses_by_index: list[int] = []
        # The future access of each element in the sequence, by value of the element
        self.next_accesses_by_value: list[int] = []

    def get_cache_faults_in_sequence(self, sequence: list[int], cache_size: int) -> int:
        """
        @param sequence: The sequence of cache keys accessed.
        @param cache_size: The size of the cache.
        @return: The number of cache faults in the sequence, using Belady's algorithm.
        """
        # The elements currently in the cache
        cache: list[int] = []

        cache_faults: int = 0

        self._init_next_accesses_by_value(sequence)
        self._init_next_accesses_by_index(sequence)

        for index, value in enumerate(sequence):

            if value not in cache:
                cache_faults += 1

                if len(cache) == cache_size:
                    cache.remove(self._get_element_to_remove(cache, index))

                cache.append(value)

            # Update the next access of the element that was accessed
            self.next_accesses_by_value[value] = self.next_accesses_by_index[index]

        return cache_faults

    def _get_element_to_remove(self, cache: list[int], current_index: int) -> int:
        """
        Get the element that should be removed from the cache. Since we are using Belady's
        algorithm, we want to remove the element that will be accessed the farthest in the future.
        """
        element_to_remove = cache[0]
        max_next_access = self.next_accesses_by_value[element_to_remove]

        for element in cache:
            next_access = self.next_accesses_by_value[element]

            if next_access > max_next_access:
                max_next_access = next_access
                element_to_remove = element

        return element_to_remove

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
