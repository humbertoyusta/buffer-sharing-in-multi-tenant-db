import numpy as np

np.random.seed(0)


def generate_zipfian_array(parameter: float, length: int, max_value: int) -> np.array:
    return (np.random.zipf(parameter, length) - 1) % max_value + 1


def generate_uniform_array(length: int, max_value: int) -> np.array:
    return np.random.randint(1, max_value + 1, length)


def generate_normal_array(variance: float, length: int, max_value: int) -> np.array:
    return (abs(np.random.normal(0, variance, length)).astype(int)) % max_value + 1


def generate_pareto_array(parameter: float, length: int, max_value: int) -> np.array:
    return (np.random.pareto(parameter, length) % max_value).astype(int) + 1


def generate_normal_with_correlated_references(
    variance: float,
    length: int,
    max_value: int,
    max_correlated_references: int,
    first_correlated_reference_min_distance: float,
    first_correlated_reference_max_distance: float,
    correlated_reference_distance_growth_factor: float,
) -> np.array:
    arr = np.zeros(length).astype(int)
    for i in range(length):
        if arr[i] == 0:
            arr[i] = int(abs(np.random.normal(0, variance, 1)[0])) % max_value + 1
            correlated_reference_min_distance = first_correlated_reference_min_distance
            correlated_reference_max_distance = first_correlated_reference_max_distance
            dist = np.random.uniform(
                correlated_reference_min_distance,
                correlated_reference_max_distance,
            )

            for _ in range(max_correlated_references):
                if (i + int(dist)) >= length:
                    break
                arr[i + int(dist)] = arr[i]

                correlated_reference_min_distance *= (
                    correlated_reference_distance_growth_factor
                )
                correlated_reference_max_distance *= (
                    correlated_reference_distance_growth_factor
                )
                dist *= np.random.uniform(
                    correlated_reference_min_distance,
                    correlated_reference_max_distance,
                )
    return arr


def generate_uniform_with_correlated_references(
    length: int,
    max_value: int,
    max_correlated_references: int,
    first_correlated_reference_min_distance: float,
    first_correlated_reference_max_distance: float,
    correlated_reference_distance_growth_factor: float,
) -> np.array:
    arr = np.zeros(length).astype(int)
    for i in range(length):
        if arr[i] == 0:
            arr[i] = np.random.randint(1, max_value + 1, 1)[0]
            correlated_reference_min_distance = first_correlated_reference_min_distance
            correlated_reference_max_distance = first_correlated_reference_max_distance
            dist = np.random.uniform(
                correlated_reference_min_distance,
                correlated_reference_max_distance,
            )

            for _ in range(max_correlated_references):
                if (i + int(dist)) >= length:
                    break
                arr[i + int(dist)] = arr[i]

                correlated_reference_min_distance *= (
                    correlated_reference_distance_growth_factor
                )
                correlated_reference_max_distance *= (
                    correlated_reference_distance_growth_factor
                )
                dist *= np.random.uniform(
                    correlated_reference_min_distance,
                    correlated_reference_max_distance,
                )
    return arr


def generate_pareto_with_correlated_references(
    parameter: float,
    length: int,
    max_value: int,
    max_correlated_references: int,
    first_correlated_reference_min_distance: float,
    first_correlated_reference_max_distance: float,
    correlated_reference_distance_growth_factor: float,
) -> np.array:
    arr = np.zeros(length).astype(int)
    for i in range(length):
        if arr[i] == 0:
            arr[i] = np.random.pareto(parameter, 1)[0] % max_value + 1
            correlated_reference_min_distance = first_correlated_reference_min_distance
            correlated_reference_max_distance = first_correlated_reference_max_distance
            dist = np.random.uniform(
                correlated_reference_min_distance,
                correlated_reference_max_distance,
            )

            for _ in range(max_correlated_references):
                if (i + int(dist)) >= length:
                    break
                arr[i + int(dist)] = arr[i]

                correlated_reference_min_distance *= (
                    correlated_reference_distance_growth_factor
                )
                correlated_reference_max_distance *= (
                    correlated_reference_distance_growth_factor
                )
                dist *= np.random.uniform(
                    correlated_reference_min_distance,
                    correlated_reference_max_distance,
                )
    return arr


def generate_zipfian_with_correlated_references(
    parameter: float,
    length: int,
    max_value: int,
    max_correlated_references: int,
    first_correlated_reference_min_distance: float,
    first_correlated_reference_max_distance: float,
    correlated_reference_distance_growth_factor: float,
) -> np.array:
    arr = np.zeros(length).astype(int)
    for i in range(length):
        if arr[i] == 0:
            arr[i] = np.random.zipf(parameter, 1)[0] % max_value + 1
            correlated_reference_min_distance = first_correlated_reference_min_distance
            correlated_reference_max_distance = first_correlated_reference_max_distance
            dist = np.random.uniform(
                correlated_reference_min_distance,
                correlated_reference_max_distance,
            )

            for _ in range(max_correlated_references):
                if (i + int(dist)) >= length:
                    break
                arr[i + int(dist)] = arr[i]

                correlated_reference_min_distance *= (
                    correlated_reference_distance_growth_factor
                )
                correlated_reference_max_distance *= (
                    correlated_reference_distance_growth_factor
                )
                dist *= np.random.uniform(
                    correlated_reference_min_distance,
                    correlated_reference_max_distance,
                )
    return arr
