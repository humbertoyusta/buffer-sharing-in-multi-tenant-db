from .test_case.validator import Validator
from .test_case.random_generator import RandomGenerator
from .test_case.io_wrapper import IoWrapper
from .test_case.page_accesses_plotter import PageAccessesPlotter
import sys
from .test_case.brightkite_generator import BrightkiteGenerator
from .test_case.citibike_generator import CitibikeGenerator

TEST_CASES_PER_CITIBIKE_EXP = 4

if __name__ == "__main__":
    test_type_input = sys.argv[1] if len(sys.argv) > 1 else "all"

    if test_type_input == "all":
        test_types = ["random", "brightkite", "citibike"]
    else:
        test_types = [test_type_input]

    if "random" in test_types:
        first_test = sys.argv[2] if len(sys.argv) > 2 else 1
        last_test = sys.argv[3] if len(sys.argv) > 3 else 16

        for test_number in range(int(first_test), int(last_test) + 1):
            test_case = RandomGenerator(test_number).generate_test_case()
            IoWrapper(test_case, test_number, "random").print_test_case()
            Validator(test_number, "random", has_description=True).validate()
            PageAccessesPlotter(
                test_case["page_accesses"],
                len(test_case["tenants"]),
                test_number,
                "random",
            ).plot()

    if "brightkite" in test_types:
        first_test = sys.argv[2] if len(sys.argv) > 2 else 1
        last_test = (
            sys.argv[3]
            if len(sys.argv) > 3
            else len(BrightkiteGenerator.CSV_FILE_NAMES)
        )

        for test_number in range(int(first_test), int(last_test) + 1):
            test_case = BrightkiteGenerator(test_number).generate_test_case()
            IoWrapper(test_case, test_number, "brightkite").print_test_case()
            Validator(test_number, "brightkite", has_description=False).validate()
            PageAccessesPlotter(
                test_case["page_accesses"],
                len(test_case["tenants"]),
                test_number,
                "brightkite",
            ).plot()

    if "citibike" in test_types:
        first_exp = sys.argv[2] if len(sys.argv) > 2 else 1
        last_exp = sys.argv[3] if len(sys.argv) > 3 else 3

        for exp_number in range(int(first_exp), int(last_exp) + 1):
            for test_number in range(1, TEST_CASES_PER_CITIBIKE_EXP + 1):
                test_case = CitibikeGenerator(
                    exp_number=exp_number, test_number=test_number
                ).generate_test_case()
                IoWrapper(
                    test_case=test_case,
                    test_number=test_number,
                    test_type="citibike",
                    exp_number=exp_number,
                ).print_test_case()
                Validator(
                    test_number=test_number,
                    test_type="citibike",
                    has_description=False,
                    exp_number=exp_number,
                ).validate()
                PageAccessesPlotter(
                    page_accesses=test_case["page_accesses"],
                    tenant_number=len(test_case["tenants"]),
                    test_number=test_number,
                    test_type="citibike",
                    exp_number=exp_number,
                ).plot()
