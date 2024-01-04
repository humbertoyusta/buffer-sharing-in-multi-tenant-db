#include "checker.h"
#include "input_reader.h"
#include "scorer.h"
#include "solutions/lfu_policy_1_solution.h"
#include "solutions/lfu_policy_2_solution.h"
#include "solutions/lru_2_solution.h"
#include "solutions/lru_policy_1_solution.h"
#include "solutions/lru_policy_2_solution.h"
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

int main(int argc, char **argv) {
  std::string solution_name = argv[1];
  int first_test_number = std::stoi(argv[2]);
  int last_test_number = std::stoi(argv[3]);

  auto scorer = Scorer(solution_name);
  std::vector<TestScore> test_scores;

  double best_tune_parameter = -1;
  double best_mean_fault_score = INT_MAX;

  std::vector<double> tune_parameters;

  if (solution_name == "lru_2_solution")
    tune_parameters = {0.005, 0.01, 0.015, 0.02, 0.025, 0.03, 0.04,
                       0.05,  0.06, 0.07,  0.08, 0.09,  0.1};

  std::string filepath = "results/tuner_" + solution_name + ".yaml";
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "solution_name" << YAML::Value << solution_name;
  out << YAML::Key << "tune_parameters" << YAML::Value << YAML::BeginSeq;

  for (auto tune_parameter : tune_parameters) {
    std::cout << "Tuning parameter: " << tune_parameter << std::endl;

    out << YAML::BeginMap;
    out << YAML::Key << "tune_parameter" << YAML::Value << tune_parameter;

    for (int test_number = first_test_number; test_number <= last_test_number;
         test_number++) {
      auto input_reader = InputReader();
      auto [tenants, page_accesses, total_buffer_size] =
          input_reader.ReadInput(test_number);

      auto checker = Checker();
      Solution *solution = nullptr;

      if (solution_name == "lru_2_solution") {
        solution = new Lru2Solution(tune_parameter);
      } else {
        std::cout << "Solution not tunable, with name: " << solution_name
                  << std::endl;
        return 1;
      }

      std::cout << "Running test " << test_number << " with solution "
                << solution_name << " and tune parameter " << tune_parameter
                << std::endl;

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

    auto [mean_fault_score, mean_hit_score] = scorer.GetMeanScores(test_scores);

    std::cout << "Parameter: " << tune_parameter << std::endl;
    std::cout << "Mean fault score: " << mean_fault_score << std::endl;
    std::cout << "Mean hit score: " << mean_hit_score << std::endl;

    out << YAML::Key << "mean_fault_score" << YAML::Value << mean_fault_score;
    out << YAML::Key << "mean_hit_score" << YAML::Value << mean_hit_score;
    out << YAML::EndMap;

    if (mean_fault_score < best_mean_fault_score) {
      best_mean_fault_score = mean_fault_score;
      best_tune_parameter = tune_parameter;
    }
  }

  std::cout << "Best parameter: " << best_tune_parameter << std::endl;

  out << YAML::EndSeq;
  out << YAML::Key << "best_tune_parameter" << YAML::Value
      << best_tune_parameter;
  out << YAML::EndMap;

  std::ofstream file(filepath);
  file << out.c_str();
  file.close();
}