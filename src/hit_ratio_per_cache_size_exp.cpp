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

int main() {
  std::vector<SolutionMetadata> solutions = {
      {"2Q", 1},   {"Belady", 2}, {"LFU", 3}, {"LIRS", 4},
      {"LRFU", 5}, {"LRU-2", 6},  {"LRU", 7}, {"MQ", 8},
  };

  std::vector<ExperimentMetadata> experiments = {
      {"brightkite", 1, {50, 75, 100, 125, 150, 175, 200, 225, 250, 275}},
      {"citibike_exp_1",
       1,
       {4000, 8000, 12000, 16000, 20000, 24000, 28000, 32000, 36000, 40000}},
      {"citibike_exp_2",
       2,
       {3000, 6000, 9000, 12000, 15000, 18000, 21000, 24000, 27000, 30000}},
      {"citibike_exp_3",
       3,
       {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 200}}};

  for (auto experiment : experiments) {
    for (int total_buffer_size : experiment.total_buffer_sizes) {
    }
  }
}