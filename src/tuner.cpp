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
#include "solutions/mq_solution.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

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

  double best_tune_parameter = -1;
  double best_second_tune_parameter = -1;
  double best_third_tune_parameter = -1;
  double best_mean_fault_score = INT_MAX;

  std::vector<double> tune_parameters;
  std::vector<double> second_tune_parameters;
  std::vector<double> third_tune_parameters;

  if (solution_name == "lru_2_solution") {
    tune_parameters = {0.05, 0.05, 0.05, 0.1,  0.1,  0.1,  0.15, 0.15, 0.15,
                       0.2,  0.2,  0.2,  0.25, 0.25, 0.25, 0.3,  0.3,  0.3};
    second_tune_parameters = {
        0.5, 1.0, 1.5, 0.5, 1.0, 1.5, 0.5, 1.0, 1.5,
        0.5, 1.0, 1.5, 0.5, 1.0, 1.5, 0.5, 1.0, 1.5,
    };
  }

  if (solution_name == "_2q_solution") {
    tune_parameters = {0.1, 0.1, 0.1, 0.2, 0.2, 0.2,
                       0.3, 0.3, 0.3, 0.4, 0.4, 0.4};
    second_tune_parameters = {0.5, 1.0, 1.5, 0.5, 1.0, 1.5,
                              0.5, 1.0, 1.5, 0.5, 1.0, 1.5};
  }

  if (solution_name == "lrfu_solution") {
    tune_parameters = {0.001, 0.0001, 0.00001, 0.000001};
  }

  if (solution_name == "mq_solution") {
    tune_parameters = {3, 5, 7, 3, 5, 7, 3, 5, 7, 3, 5, 7, 3, 5, 7, 3, 5, 7};
    second_tune_parameters = {0.1, 0.1, 0.1, 0.2, 0.2, 0.2, 0.3, 0.3, 0.3,
                              0.1, 0.1, 0.1, 0.2, 0.2, 0.2, 0.3, 0.3, 0.3};
    third_tune_parameters = {
        0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    };
  }

  if (solution_name == "lirs_solution") {
    tune_parameters = {0.8, 0.85, 0.9, 0.95, 0.98};
  }

  auto scorer = Scorer(solution_name, test_type);
  std::vector<TestScore> test_scores;

  std::string filepath =
      "results/tuner_" + solution_name + "." + test_type + ".yaml";
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "solution_name" << YAML::Value << solution_name;
  out << YAML::Key << "tune_parameters" << YAML::Value << YAML::BeginSeq;

  int i = 0;
  for (auto tune_parameter : tune_parameters) {
    std::cout << "Tuning parameter: " << tune_parameter;
    if (second_tune_parameters.size() > 0)
      std::cout << ", " << second_tune_parameters[i];
    if (third_tune_parameters.size() > 0)
      std::cout << ", " << third_tune_parameters[i];
    std::cout << std::endl;

    out << YAML::BeginMap;
    out << YAML::Key << "tune_parameter" << YAML::Value << tune_parameter;
    if (second_tune_parameters.size() > 0)
      out << YAML::Key << "second_tune_parameter" << YAML::Value
          << second_tune_parameters[i];
    if (third_tune_parameters.size() > 0)
      out << YAML::Key << "third_tune_parameter" << YAML::Value
          << third_tune_parameters[i];

    for (std::string current_test_type : test_types) {

      int actual_last_test_number = last_test_number;
      if (last_test_number == 0) {
        actual_last_test_number = TEST_TYPE_TO_TEST_COUNT[current_test_type];
      }

      for (int test_number = first_test_number;
           test_number <= actual_last_test_number; test_number++) {
        auto input_reader = InputReader();
        auto [tenants, page_accesses, total_buffer_size] =
            input_reader.ReadInput(current_test_type, test_number);

        auto checker = Checker();
        Solution *solution = nullptr;

        if (solution_name == "lru_2_solution") {
          solution =
              new Lru2Solution(tune_parameter, second_tune_parameters[i]);
        } else if (solution_name == "_2q_solution") {
          solution = new _2QSolution(tune_parameter, second_tune_parameters[i]);
        } else if (solution_name == "lrfu_solution") {
          solution = new LRFUSolution(tune_parameter);
        } else if (solution_name == "mq_solution") {
          solution = new MQSolution(int(std::round(tune_parameter)),
                                    second_tune_parameters[i],
                                    third_tune_parameters[i]);
        } else if (solution_name == "lirs_solution") {
          solution = new LIRSSolution(tune_parameter);
        } else {
          std::cout << "Solution not tunable, with name: " << solution_name
                    << std::endl;
          return 1;
        }

        std::cout << "Running test " << current_test_type << " - "
                  << test_number << " with solution " << solution_name
                  << " and tune parameter " << tune_parameter;
        if (second_tune_parameters.size() > 0)
          std::cout << ", " << second_tune_parameters[i];
        if (third_tune_parameters.size() > 0)
          std::cout << ", " << third_tune_parameters[i];
        std::cout << std::endl;

        auto [judge_page_hits_per_tenant, judge_page_faults_per_tenant,
              solution_page_hits_per_tenant, solution_page_faults_per_tenant] =
            checker.CheckSolution(solution, tenants, page_accesses,
                                  total_buffer_size);

        std::cout << "Test " << current_test_type << " - " << test_number
                  << " with solution " << solution_name << " finished"
                  << std::endl;

        test_scores.push_back(scorer.GetTestScore(
            tenants, judge_page_hits_per_tenant, judge_page_faults_per_tenant,
            solution_page_hits_per_tenant, solution_page_faults_per_tenant));

        delete solution;
      }
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
      if (!second_tune_parameters.empty())
        best_second_tune_parameter = second_tune_parameters[i];
      if (!third_tune_parameters.empty())
        best_third_tune_parameter = third_tune_parameters[i];
    }

    i++;
  }

  std::cout << "Best parameter: " << best_tune_parameter << std::endl;
  if (!second_tune_parameters.empty())
    std::cout << "Best second parameter: " << best_second_tune_parameter
              << std::endl;
  if (!third_tune_parameters.empty())
    std::cout << "Best third parameter: " << best_third_tune_parameter
              << std::endl;
  std::cout << "Best mean fault score: " << best_mean_fault_score << std::endl;

  out << YAML::EndSeq;
  out << YAML::Key << "best_tune_parameter" << YAML::Value
      << best_tune_parameter;
  out << YAML::Key << "best_second_tune_parameter" << YAML::Value
      << best_second_tune_parameter;
  out << YAML::Key << "best_third_tune_parameter" << YAML::Value
      << best_third_tune_parameter;
  out << YAML::EndMap;

  std::ofstream file(filepath);
  file << out.c_str();
  file.close();
}