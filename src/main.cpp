#include "checker.h"
#include "input_reader.h"
#include "solutions/lru_policy_1_solution.h"
#include <iostream>

int main() {
  auto input_reader = InputReader();
  auto [tenants, page_accesses, total_buffer_size] = input_reader.ReadInput(2);

  auto checker = Checker();
  Solution *solution = new LruPolicy1Solution();
  checker.CheckSolution(solution, tenants, page_accesses, total_buffer_size);
}