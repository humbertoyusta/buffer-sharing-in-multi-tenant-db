#include "input_reader.h"

#include <fstream>
#include <stdexcept>

std::tuple<std::vector<Tenant>, std::vector<PageAccess>, int>
InputReader::ReadInput(int test_number) {

  std::string filename =
      "test_cases/test_case_" + std::to_string(test_number) + "/input.txt";
  std::ifstream input_file(filename);

  if (!input_file.is_open()) {
    throw std::runtime_error("Could not open file " + filename);
  }

  int num_tenants, total_buffer_size, page_accesses_length;
  input_file >> num_tenants >> total_buffer_size >> page_accesses_length;

  std::vector<Tenant> tenants(num_tenants);
  std::vector<PageAccess> page_accesses(page_accesses_length);

  for (int i = 0; i < num_tenants; ++i) {
    tenants[i].tenant_id = i + 1;
    input_file >> tenants[i].priority;
  }
  for (int i = 0; i < num_tenants; ++i) {
    input_file >> tenants[i].database_size;
  }
  for (int i = 0; i < num_tenants; ++i) {
    input_file >> tenants[i].min_buffer_size >> tenants[i].base_buffer_size >>
        tenants[i].max_buffer_size;
  }

  for (int i = 0; i < page_accesses_length; ++i) {
    input_file >> page_accesses[i].tenant_id >> page_accesses[i].page_id;
  }

  return std::make_tuple(tenants, page_accesses, total_buffer_size);
};
