#include "checker.h"
#include "input_reader.h"
#include "solutions/lru_policy_1_solution.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  std::string solution_name = argv[1];
  int first_test_number = std::stoi(argv[2]);
  int last_test_number = std::stoi(argv[3]);

  for (int test_number = first_test_number; test_number <= last_test_number;
       test_number++) {
    auto input_reader = InputReader();
    auto [tenants, page_accesses, total_buffer_size] =
        input_reader.ReadInput(test_number);

    auto checker = Checker();
    Solution *solution = nullptr;

    if (solution_name == "lru_policy_1_solution") {
      solution = new LruPolicy1Solution();
    } else {
      std::cout << "Unknown solution name: " << solution_name << std::endl;
      return 1;
    }

    checker.CheckSolution(solution, tenants, page_accesses, total_buffer_size);

    delete solution;
  }
}