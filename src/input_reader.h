#ifndef INPUT_READER_H
#define INPUT_READER_H

#include "types/page_access.h"
#include "types/tenant.h"
#include <string>
#include <vector>

class InputReader {
public:
  /**
   * @brief Reads test case from input file and returns its contents
   * @param test_number Number of test case to read
   * @return std::tuple<std::vector<Tenant>, std::vector<PageAccess>, int> -
   * Vector of tenants, vector of page accesses, total buffer size
   */
  static std::tuple<std::vector<Tenant>, std::vector<PageAccess>, int>
  ReadInput(std::string test_type, int test_number);
};

#endif // INPUT_READER_H