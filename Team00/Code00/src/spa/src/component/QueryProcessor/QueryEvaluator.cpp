#include "QueryEvaluator.h"
#include "types/QueryEvaluatorTable.h"
//#include "../PKB/PKB.h"

std::vector<std::string> QueryEvaluator::EvaluateQuery(
        std::list<Synonym> syn_list, Synonym target, std::list<Group> groups) {
  QueryEvaluatorTable table(target.GetName());

  std::unordered_map<DesignEntity, std::list<std::string>> map_of_synonym_values;
  // Query all the design entities and add them to an unordered_map.
  for (auto iter = syn_list.begin(); iter != syn_list.end(); iter++) {
    DesignEntity synonym_design_entity = iter->GetType();
    //std::list<std::string> list_of_synonym_values = PKB.getDesignEntity(synonym_design_entity);
    //map_of_synonym_values[synonym_design_entity] = list_of_synonym_values;
    if (iter->GetName() == target.GetName()) {
      //table.add_target_synonym(iter->GetName(), list_of_synonym_values);
    }
  }

  // return the final results
  return table.GetResults();
};
