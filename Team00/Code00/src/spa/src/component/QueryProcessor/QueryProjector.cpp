#include "QueryProjector.h"

std::string QueryProjector::FormatQuery(std::vector<std::string> result_list) {
  std::string output;
  for (std::string result : result_list) {
    output += result;
    output += ", ";
  }
  output.erase(output.size() - 1);
  output.erase(output.size() - 1);
  return output;
};
