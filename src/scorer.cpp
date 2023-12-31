#include "scorer.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <yaml-cpp/yaml.h>

Scorer::Scorer(std::string solution_name) : solution_name_(solution_name) {}

double Scorer::ratio(const int a, const int b) {
  return static_cast<double>(a) / static_cast<double>(b);
}

double Scorer::square(const double a) { return a * a; }

TestScore
Scorer::GetTestScore(std::vector<Tenant> tenants,
                     std::vector<int> judge_page_hits_per_tenant,
                     std::vector<int> judge_page_faults_per_tenant,
                     std::vector<int> solution_page_hits_per_tenant,
                     std::vector<int> solution_page_faults_per_tenant) {
  TestScore test_score;

  for (int i = 0; i < tenants.size(); i++) {
    double fault_service_level_agreement_rate =
        ratio(std::max(solution_page_faults_per_tenant[i] -
                           judge_page_faults_per_tenant[i],
                       0),
              judge_page_faults_per_tenant[i]);
    test_score.tenant_fault_scores.push_back(
        (fault_service_level_agreement_rate));
  }

  double tenant_priority_sum_of_sqrt = std::accumulate(
      tenants.begin(), tenants.end(), 0,
      [](double sum, const Tenant &t) { return sum + std::sqrt(t.priority); });

  for (int i = 0; i < tenants.size(); i++) {
    test_score.total_fault_score += test_score.tenant_fault_scores[i] *
                                    std::sqrt((double)tenants[i].priority);
  }

  test_score.total_fault_score /= tenant_priority_sum_of_sqrt;

  for (int i = 0; i < tenants.size(); i++) {
    double hit_service_level_agreement_rate =
        ratio(std::max(judge_page_hits_per_tenant[i] -
                           solution_page_hits_per_tenant[i],
                       0),
              judge_page_hits_per_tenant[i]);
    test_score.tenant_hit_scores.push_back((hit_service_level_agreement_rate));

    test_score.judge_hit_ratio_per_tenant.push_back(
        ratio(judge_page_hits_per_tenant[i],
              judge_page_hits_per_tenant[i] + judge_page_faults_per_tenant[i]));

    test_score.solution_hit_ratio_per_tenant.push_back(ratio(
        solution_page_hits_per_tenant[i],
        solution_page_hits_per_tenant[i] + solution_page_faults_per_tenant[i]));
  }

  for (int i = 0; i < tenants.size(); i++) {
    test_score.total_hit_score += test_score.tenant_hit_scores[i] *
                                  std::sqrt((double)tenants[i].priority);
  }

  test_score.total_hit_score /= tenant_priority_sum_of_sqrt;

  return test_score;
}

void Scorer::ReportScores(std::vector<TestScore> test_scores) {
  double mean_fault_score = 0;
  double mean_hit_score = 0;

  for (auto test_score : test_scores) {
    mean_fault_score += test_score.total_fault_score;
    mean_hit_score += test_score.total_hit_score;
  }

  mean_fault_score /= test_scores.size();
  mean_hit_score /= test_scores.size();

  int test_number = 1;
  std::string filepath = "results/" + solution_name_ + ".yaml";
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "solution_name" << YAML::Value << solution_name_;
  out << YAML::Key << "mean_fault_score" << YAML::Value << mean_fault_score;
  out << YAML::Key << "mean_hit_score" << YAML::Value << mean_hit_score;
  out << YAML::Key << "test_cases" << YAML::Value << YAML::BeginSeq;
  for (auto test_score : test_scores) {
    out << YAML::BeginMap;
    out << YAML::Key << "test_number" << YAML::Value
        << std::to_string(test_number++);
    out << YAML::Key << "fault_score" << YAML::Value
        << test_score.total_fault_score;
    out << YAML::Key << "hit_score" << YAML::Value
        << test_score.total_hit_score;
    out << YAML::Key << "tenants" << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < test_score.tenant_fault_scores.size(); i++) {
      out << YAML::BeginMap;
      out << YAML::Key << "tenant_id" << YAML::Value << i + 1;
      out << YAML::Key << "fault_score" << YAML::Value
          << test_score.tenant_fault_scores[i];
      out << YAML::Key << "hit_score" << YAML::Value
          << test_score.tenant_hit_scores[i];
      out << YAML::Key << "judge_hit_ratio" << YAML::Value
          << test_score.judge_hit_ratio_per_tenant[i];
      out << YAML::Key << "solution_hit_ratio" << YAML::Value
          << test_score.solution_hit_ratio_per_tenant[i];
      out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream file(filepath);
  file << out.c_str();
  file.close();
}