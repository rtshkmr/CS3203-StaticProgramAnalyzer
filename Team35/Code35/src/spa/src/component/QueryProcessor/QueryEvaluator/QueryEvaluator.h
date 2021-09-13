#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <list>
#include <string>
#include <vector>
#include "component/QueryProcessor/types/Types.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/PKB/PKB.h"
#include "QueryPKB.h"
#include "EvaluateSuchThatClause.h"
#include "EvaluatePatternClause.h"

class QueryEvaluator {
  public:
    QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups, PKB pkb);
    std::vector<std::string> EvaluateQuery();
  private:
    std::list<Synonym> synonymlist;
    Synonym targetSynonym;
    std::list<Group> groupList;
    PKB pkb;
    bool booleanResult;
    std::unordered_map<DesignEntity, std::list<std::string>> map_of_synonym_values;
    void PopulateSynonymValues(QueryEvaluatorTable* table);
    void EvaluateAllGroups(QueryEvaluatorTable* table);
    void EvaluateSuchThatClause(SuchThat st, QueryEvaluatorTable* table);
    void ProcessBooleanGroup(std::vector<Clause*> clauseList);
    void ProcessNonBooleanGroup(std::vector<Clause*> clauseList, QueryEvaluatorTable* table);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
