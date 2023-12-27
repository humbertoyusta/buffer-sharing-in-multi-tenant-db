from test_case.validator import Validator
from test_case.generator import Generator
from test_case.io_wrapper import IoWrapper
from test_case.page_accesses_plotter import PageAccessesPlotter

if __name__ == "__main__":
    for test_number in range(1, 15):
        test_case = Generator(test_number).generate_test_case()
        IoWrapper(test_case, test_number).print_test_case()
        Validator(test_number).validate()
        PageAccessesPlotter(test_case['page_accesses'], len(test_case['tenants']), test_number).plot()
