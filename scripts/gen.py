from .test_case.validator import Validator
from .test_case.generator import Generator
from .test_case.io_wrapper import IoWrapper
from .test_case.page_accesses_plotter import PageAccessesPlotter
import sys

if __name__ == "__main__":
    first_test = sys.argv[1] if len(sys.argv) > 1 else 1
    last_test = sys.argv[2] if len(sys.argv) > 2 else 14

    for test_number in range(int(first_test), int(last_test) + 1):
        test_case = Generator(test_number).generate_test_case()
        IoWrapper(test_case, test_number).print_test_case()
        Validator(test_number).validate()
        PageAccessesPlotter(test_case['page_accesses'], len(test_case['tenants']), test_number).plot()
