from .test_case.validator import Validator
from .test_case.random_generator import RandomGenerator
from .test_case.io_wrapper import IoWrapper
from .test_case.page_accesses_plotter import PageAccessesPlotter
import sys

if __name__ == "__main__":
    test_type_input = sys.argv[1] if len(sys.argv) > 1 else "all"

    if test_type_input == "all":
        test_types = ["random", "brightkite", "citbike"]
    else:
        test_types = [test_type_input]

    if "random" in test_types:
        first_test = sys.argv[2] if len(sys.argv) > 2 else 1
        last_test = sys.argv[3] if len(sys.argv) > 3 else 16

        for test_number in range(int(first_test), int(last_test) + 1):
            test_case = RandomGenerator(test_number).generate_test_case()
            IoWrapper(test_case, test_number, "random").print_test_case()
            Validator(test_number, "random").validate()
            PageAccessesPlotter(
                test_case["page_accesses"],
                len(test_case["tenants"]),
                test_number,
                "random",
            ).plot()
