import numpy as np

np.random.seed(0)

def generate_zipfian_array (parameter: float, length: int, max_value: int) -> np.array:
    return (np.random.zipf(parameter, length) - 1) % max_value + 1

def generate_uniform_array (length: int, max_value: int) -> np.array:
    return np.random.randint(1, max_value + 1, length)

def generate_normal_array (variance: float, length: int, max_value: int) -> np.array:
    return (abs(np.random.normal(0, variance, length)).astype(int)) % max_value + 1

def generate_pareto_array (parameter: float, length: int, max_value: int) -> np.array:
    return (np.random.pareto(parameter, length) % max_value).astype(int) + 1