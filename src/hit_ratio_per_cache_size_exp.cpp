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
  std::vector<int> total_buffer_sizes;
  ExperimentMetadata(std::string test_type, int test_number,
                     std::vector<int> total_buffer_sizes)
      : test_type(test_type), test_number(test_number),
        total_buffer_sizes(total_buffer_sizes) {}
};

int main(int argc, char **argv) {
  bool all_priorities_equal = false;
  if (argc >= 2) {
    all_priorities_equal = std::string(argv[1]) == "all_priorities_equal";
  }

  std::vector<SolutionMetadata> solutions = {
      {"2Q", 1},    {"Belady", 2}, {"LFU", 3}, {"LIRS", 4},     {"LRFU", 5},
      {"LRU-2", 6}, {"LRU", 7},    {"MQ", 8},  {"NaiveLRU", 9},
  };

  std::vector<ExperimentMetadata> experiments = {
      {"brightkite", 3, {50, 75, 100, 125, 150, 175, 200, 225, 250, 275}},
      {"citibike_exp_1",
       2,
       {4000, 8000, 12000, 16000, 20000, 24000, 28000, 32000, 36000, 40000}},
      {"citibike_exp_2",
       3,
       {3000, 6000, 9000, 12000, 15000, 18000, 21000, 24000, 27000, 30000}},
      {"citibike_exp_3",
       4,
       {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 200}},
  };

  std::string filepath = "experiments/hit_ratio_per_cache_size_exp.yaml";
  if (all_priorities_equal) {
    filepath =
        "experiments/hit_ratio_per_cache_size_exp_all_priorities_equal.yaml";
  }
  YAML::Emitter out;

  out << YAML::BeginMap;

  int experiment_number = 0;
  for (auto experiment : experiments) {

    ++experiment_number;
    std::string exp_name = "experiment_" + std::to_string(experiment_number);
    out << exp_name << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "test_type" << YAML::Value << experiment.test_type;
    out << YAML::Key << "test_number" << YAML::Value << experiment.test_number;
    out << YAML::Key << "total_buffer_sizes" << YAML::Value << YAML::BeginSeq;

    int buffer_size_index = 0;
    for (int total_buffer_size : experiment.total_buffer_sizes) {

      out << YAML::BeginMap;
      ++buffer_size_index;
      std::string buffer_size_name =
          "buffer_size_" + std::to_string(buffer_size_index);
      out << YAML::Key << buffer_size_name << YAML::Value << total_buffer_size;

      std::cout << "Running experiment " << experiment.test_type << " "
                << experiment.test_number << " with total buffer size "
                << total_buffer_size << std::endl;

      auto input_reader = InputReader();

      auto [tenants, page_accesses, original_total_buffer_size] =
          input_reader.ReadInput(experiment.test_type, experiment.test_number);

      int sum_of_base_buffer_sizes = 0;
      for (auto tenant : tenants) {
        sum_of_base_buffer_sizes += tenant.base_buffer_size;
      }

      double buffer_size_conversion_ratio =
          double(total_buffer_size) / double(original_total_buffer_size);

      int actual_total_buffer_size = 0;
      for (auto &tenant : tenants) {
        tenant.base_buffer_size = int(round(double(tenant.base_buffer_size) *
                                            buffer_size_conversion_ratio));
        tenant.min_buffer_size = int(round(double(tenant.min_buffer_size) *
                                           buffer_size_conversion_ratio));
        tenant.max_buffer_size = int(round(double(tenant.max_buffer_size) *
                                           buffer_size_conversion_ratio));
        actual_total_buffer_size += tenant.base_buffer_size;
      }

      if (all_priorities_equal) {
        for (auto &tenant : tenants) {
          tenant.priority = 1;
        }
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
  }

  out << YAML::EndMap;

  std::ofstream file(filepath);
  file << out.c_str();
  file.close();
}