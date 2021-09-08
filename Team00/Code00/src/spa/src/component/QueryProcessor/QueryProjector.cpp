#include "QueryProjector.h"

std::string formatQuery(std::vector<std::string> resultList) {
  std::string output;
  for (std::string result : resultList) {
    output += result;
    output += ", ";
  }
  output.erase(output.size() - 1);
  output.erase(output.size() - 1);
  return output;
};
