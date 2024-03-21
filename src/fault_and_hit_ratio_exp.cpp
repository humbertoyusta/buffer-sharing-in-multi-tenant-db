#include "checker.h"
#include "input_reader.h"
#include "scorer.h"
#include "solutions/_2q_solution.h"
#include "solutions/belady_solution.h"
#include "solutions/lfu_policy_1_solution.h"
#include "solutions/lirs_solution.h"
#include "solutions/lrfu_solution.h"
#include "solutions/lru_2_solution.h"
#include "solutions/lru_policy_1_solution.h"
#include "solutions/mq_solution.h"
#include "solutions/naive_lru_solution.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

struct SolutionMetadata {
  std::string name;
  int id;
  SolutionMetadata(std::string name, int id) : name(name), id(id) {}
};

struct ExperimentMetadata {
  std::string test_type;
  int test_number;
  ExperimentMetadata(std::string test_type, int test_number)
      : test_type(test_type), test_number(test_number) {}
};

int main() {
  std::vector<SolutionMetadata> solutions = {
      {"2Q", 1},    {"Belady", 2}, {"LFU", 3}, {"LIRS", 4},     {"LRFU", 5},
      {"LRU-2", 6}, {"LRU", 7},    {"MQ", 8},  {"NaiveLRU", 9},
  };

  std::vector<ExperimentMetadata> experiments = {
      {"brightkite", 1},
      {"citibike_exp_1", 1},
      {"citibike_exp_2", 2},
      {"citibike_exp_3", 3},
  };

  std::string filepath = "experiments/fault_and_hit_ratio_exp.yaml";
  YAML::Emitter out;

  out << YAML::BeginMap;

  int experiment_number = 0;
  for (auto experiment : experiments) {

    ++experiment_number;
    std::string exp_name = "experiment_" + std::to_string(experiment_number);
    out << exp_name << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "test_type" << YAML::Value << experiment.test_type;
    out << YAML::Key << "test_number" << YAML::Value << experiment.test_number;

    std::cout << "Running experiment " << experiment.test_type << " "
              << experiment.test_number << std::endl;

    auto input_reader = InputReader();

    auto [tenants, page_accesses, total_buffer_size] =
        input_reader.ReadInput(experiment.test_type, experiment.test_number);

    int sum_of_base_buffer_sizes = 0;
    for (auto tenant : tenants) {
      sum_of_base_buffer_sizes += tenant.base_buffer_size;
    }

    out << YAML::Key << "solutions" << YAML::Value << YAML::BeginSeq;

    TestScore last_test_score;

    for (auto solution_metadata : solutions) {
      auto scorer = Scorer(solution_metadata.name, experiment.test_type);

      auto checker = Checker();

      Solution *solution = nullptr;

      switch (solution_metadata.id) {
      case 1:
        solution = new _2QSolution(0.1, 0.5);
        break;
      case 2:
        solution = new BeladySolution(page_accesses);
        break;
      case 3:
        solution = new LfuPolicy1Solution();
        break;
      case 4:
        solution = new LIRSSolution(0.95);
        break;
      case 5:
        solution = new LRFUSolution(0.0001);
        break;
      case 6:
        solution = new Lru2Solution(0.1, 1.5);
        break;
      case 7:
        solution = new LruPolicy1Solution();
        break;
      case 8:
        solution = new MQSolution(7, 0.2, 1.0);
        break;
      case 9:
        solution = new NaiveLruSolution();
        break;
      }

      auto [judge_page_hits_per_tenant, judge_page_faults_per_tenant,
            solution_page_hits_per_tenant, solution_page_faults_per_tenant] =
          checker.CheckSolution(solution, tenants, page_accesses,
                                total_buffer_size);

      auto test_score = scorer.GetTestScore(
          tenants, judge_page_hits_per_tenant, judge_page_faults_per_tenant,
          solution_page_hits_per_tenant, solution_page_faults_per_tenant);

      out << YAML::BeginMap;
      out << YAML::Key << "solution_name" << YAML::Value
          << solution_metadata.name;
      out << YAML::Key << "fault_score" << YAML::Value
          << test_score.total_fault_score;

      out << YAML::Key << "hit_score" << YAML::Value
          << test_score.total_hit_score;

      out << YAML::Key << "solution_hit_ratio_per_tenant" << YAML::Value
          << YAML::BeginSeq;

      int tenant_index = 0;
      for (auto hit_ratio : test_score.solution_hit_ratio_per_tenant) {
        ++tenant_index;
        std::string tenant_name = "tenant_" + std::to_string(tenant_index);
        out << YAML::BeginMap;
        out << YAML::Key << tenant_name << YAML::Value << hit_ratio;
        out << YAML::EndMap;
      }

      out << YAML::EndSeq;

      out << YAML::EndMap;

      last_test_score = test_score;

      delete solution;
    }

    out << YAML::BeginMap;
    out << YAML::Key << "solution_name" << YAML::Value << "Judge";
    out << YAML::Key << "fault_score" << YAML::Value << 0;
    out << YAML::Key << "hit_score" << YAML::Value << 0;

    out << YAML::Key << "solution_hit_ratio_per_tenant" << YAML::Value
        << YAML::BeginSeq;

    int tenant_index = 0;
    for (auto hit_ratio : last_test_score.judge_hit_ratio_per_tenant) {
      ++tenant_index;
      std::string tenant_name = "tenant_" + std::to_string(tenant_index);
      out << YAML::BeginMap;
      out << YAML::Key << tenant_name << YAML::Value << hit_ratio;
      out << YAML::EndMap;
    }

    out << YAML::EndSeq;

    out << YAML::EndMap;

    out << YAML::EndSeq;
    out << YAML::EndMap;
  }

  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream file(filepath);
  file << out.c_str();
  file.close();
}