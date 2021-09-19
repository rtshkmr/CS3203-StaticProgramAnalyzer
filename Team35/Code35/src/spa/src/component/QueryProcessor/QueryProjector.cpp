#include <list>
#include "QueryProjector.h"

std::list<std::string> QueryProjector::FormatQuery(std::vector<std::string> results) {
  results.erase(std::unique(results.begin(), results.end() ), results.end() );
  std::list<std::string> result_list(results.begin(), results.end());
  return result_list;
};
