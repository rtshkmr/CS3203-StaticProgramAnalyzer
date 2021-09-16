#include "QueryProjector.h"

std::string QueryProjector::FormatQuery(std::vector<std::string> result_list) {
  std::string output;

  std::sort( result_list.begin(), result_list.end() );
  result_list.erase( unique( result_list.begin(), result_list.end() ), result_list.end() );

  for (std::string result : result_list) {
    output += result;
    output += " ";
  }
  output.erase(output.size() - 1);
  return output;
};
