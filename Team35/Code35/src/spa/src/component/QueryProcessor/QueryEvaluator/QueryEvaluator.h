#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <list>
#include <string>
#include <vector>
#include "component/QueryProcessor/types/Types.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/PKB/PKB.h"
#include "PKBDispatcher.h"
#include "EvaluateSuchThatClause.h"
#include "EvaluatePatternClause.h"

class QueryEvaluator {
 public:
  QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group*> groups, PKB pkb);
  std::vector<std::string> EvaluateQuery();
 private:
  std::list<Synonym> synonym_list;
  Synonym target_synonym;
  std::list<Group*> group_list;
  PKB pkb;
  bool boolean_result;
  std::unordered_map<std::string, std::list<std::string>> map_of_synonym_values;
  std::unordered_map<std::string, DesignEntity> synonym_design_entity_map;

  void PopulateSynonymValues(QueryEvaluatorTable* table);
  void EvaluateAllGroups(QueryEvaluatorTable* table);
  std::vector<std::string> GetResult(QueryEvaluatorTable* table) const;
  void PreprocessBooleanGroup(std::vector<Clause*> clauseList);
  void ProcessGroup(const std::vector<Clause*>& clauseList, QueryEvaluatorTable* table);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
