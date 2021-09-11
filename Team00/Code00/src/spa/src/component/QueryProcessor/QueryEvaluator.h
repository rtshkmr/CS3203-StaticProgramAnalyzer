#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <list>
#include <string>
#include <vector>
#include "types/Types.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/PKB/PKB.h"
#include "EvaluateSuchThatSingleSynonym.h"
#include "types/QueryEvaluatorTable.h"
#include "../PKB/PKB.h"
#include "QueryPKB.h"
#include "EvaluateSuchThatDoubleSynonyms.h"

class QueryEvaluator {
  public:
    QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups, PKB pkb);
    std::vector<std::string> EvaluateQuery();
  private:
    std::list<Synonym> synonymlist;
    Synonym targetSynonym;
    std::list<Group> groupList;
    PKB pkb;
    QueryEvaluatorTable evaluateSuchThat(Clause* clause, QueryEvaluatorTable table);
    QueryEvaluatorTable processDoubleStmtRef(SuchThat st, QueryEvaluatorTable table, RelRef query_relation);
    QueryEvaluatorTable ProcessNewColumn(std::string targetSynonymName, Synonym newSynonym, QueryEvaluatorTable table, RelRef rs, bool givenFirstParam);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
