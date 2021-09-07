#include "QueryEvaluator.h"
#include "types/QueryEvaluatorTable.h"

std::vector<std::string> QueryEvaluator::evaluateQuery(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups) {
  QueryEvaluatorTable table(target.getName());

  // Query all the design entities and add them to a unordered_map.
  for (auto iter = syn_list.begin(); iter != syn_list.end(); iter++) {

  }
};