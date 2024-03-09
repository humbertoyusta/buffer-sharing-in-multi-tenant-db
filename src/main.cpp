#include "checker.h"
#include "input_reader.h"
#include "scorer.h"
#include "solutions/_2q_solution.h"
#include "solutions/lfu_policy_1_solution.h"
#include "solutions/lfu_policy_2_solution.h"
#include "solutions/lirs_solution.h"
#include "solutions/lrfu_solution.h"
#include "solutions/lru_2_solution.h"
#include "solutions/lru_policy_1_solution.h"
#include "solutions/lru_policy_2_solution.h"
#include "solutions/lru_policy_3_solution.h"
#include "solutions/mq_solution.h"
#include <iostream>
#include <map>
#include <string>

std::map<std::string, int> TEST_TYPE_TO_TEST_COUNT = {{"random", 16},
                                                      {"brightkite", 6},
                                                      {"citibike_exp_1", 4},
                                                      {"citibike_exp_2", 4},
                                                      {"citibike_exp_3", 4}};

int main(int argc, char **argv) {
  std::string solution_name = argv[1];

  std::string test_type = argv[2];
  std::vector<std::string> test_types;
  if (test_type == "all")
    test_types = {"random", "brightkite", "citibike_exp_1", "citibike_exp_2",
                  "citibike_exp_3"};
  else
    test_types = {test_type};

  int first_test_number = (argc >= 4) ? std::stoi(argv[3]) : 1;
  int last_test_number = (argc >= 5) ? std::stoi(argv[4]) : 0;

  for (std::string test_type : test_types) {

    auto scorer = Scorer(solution_name, test_type);
    std::vector<TestScore> test_scores;

    int actual_last_test_number = last_test_number;
    if (last_test_number == 0) {
      actual_last_test_number = TEST_TYPE_TO_TEST_COUNT[test_type];
    }

    for (int test_number = first_test_number;
         test_number <= actual_last_test_number; test_number++) {
      auto input_reader = InputReader();
      auto [tenants, page_accesses, total_buffer_size] =
          input_reader.ReadInput(test_type, test_number);

      auto checker = Checker();
      Solution *solution = nullptr;

      if (solution_name == "lru_policy_1_solution") {
        solution = new LruPolicy1Solution();
      } else if (solution_name == "lru_policy_2_solution") {
        solution = new LruPolicy2Solution();
      } else if (solution_name == "lru_policy_3_solution") {
        solution = new LruPolicy3Solution();
      } else if (solution_name == "lfu_policy_1_solution") {
        solution = new LfuPolicy1Solution();
      } else if (solution_name == "lfu_policy_2_solution") {
        solution = new LfuPolicy2Solution();
      } else if (solution_name == "lru_2_solution") {
        solution = new Lru2Solution(0.1, 1.5);
      } else if (solution_name == "_2q_solution") {
        solution = new _2QSolution(0.1, 0.5);
      } else if (solution_name == "lrfu_solution") {
        solution = new LRFUSolution(0.0001);
      } else if (solution_name == "mq_solution") {
        solution = new MQSolution(7, 0.2, 1.0);
      } else if (solution_name == "lirs_solution") {
        solution = new LIRSSolution(0.95);
      } else {
        std::cout << "Unknown solution name: " << solution_name << std::endl;
        return 1;
      }

      std::cout << "Running test " << test_number << " with solution "
                << solution_name << std::endl;

      auto [judge_page_hits_per_tenant, judge_page_faults_per_tenant,
            solution_page_hits_per_tenant, solution_page_faults_per_tenant] =
          checker.CheckSolution(solution, tenants, page_accesses,
                                total_buffer_size);

      std::cout << "Test " << test_number << " with solution " << solution_name
                << " finished" << std::endl;

      test_scores.push_back(scorer.GetTestScore(
          tenants, judge_page_hits_per_tenant, judge_page_faults_per_tenant,
          solution_page_hits_per_tenant, solution_page_faults_per_tenant));

      delete solution;
    }

    scorer.ReportScores(test_scores);
  }
}