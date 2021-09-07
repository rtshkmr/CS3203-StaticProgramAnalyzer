#include "QueryEvaluator.h"
#include "types/QueryEvaluatorTable.h"
//#include "../PKB/PKB.h"

std::vector<std::string> QueryEvaluator::evaluateQuery(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups) {
  QueryEvaluatorTable table(target.getName());

  std::unordered_map<DesignEntity, std::list<std::string>> mapOfSynonymValues;
  // Query all the design entities and add them to an unordered_map.
  for (auto iter = syn_list.begin(); iter != syn_list.end(); iter++) {
    DesignEntity synonymDesignEntity = iter->getType();
    //std::list<std::string> listOfSynonymValues = PKB.getDesignEntity(synonymDesignEntity);
    //mapOfSynonymValues[synonymDesignEntity] = listOfSynonymValues;
    if (iter->getName() == target.getName()) {
      //table.addTargetSynonym(iter->getName(), listOfSynonymValues);
    }
  }

  // return the final results
  return table.getResults();
};
