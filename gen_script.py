from test_case.validator import Validator
from test_case.generator import Generator
from test_case.io_wrapper import IoWrapper

if __name__ == "__main__":
    for test_number in range(1, 3):
        test_case = Generator(test_number).generate_test_case()
        IoWrapper(test_case, test_number).print_test_case()
        Validator(test_number).validate()