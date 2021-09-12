#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <list>
#include <string>
#include <vector>
#include "component/QueryProcessor/types/Types.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/PKB/PKB.h"
#include "EvaluateSuchThatSingleSynonym.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/PKB/PKB.h"
#include "QueryPKB.h"
#include "EvaluateSuchThatDoubleSynonyms.h"
#include "EvaluateSuchThatNoSynonym.h"

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
    QueryEvaluatorTable evaluateSuchThat(Clause* clause, QueryEvaluatorTable table);
    QueryEvaluatorTable processNonBooleanSuchThat(SuchThat st, QueryEvaluatorTable table, RelRef query_relation);
    QueryEvaluatorTable processBooleanSuchThat(SuchThat such_that_clause);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
