#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

//#include <list>
//#include <string>
//#include <vector>
//#include "component/QueryProcessor/types/Types.h"
//#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
//#include "component/QueryProcessor/types/UnformattedQueryResult.h"
//#include "component/PKB/PKB.h"
//#include "ClauseStrategy.h"
//#include "component/QueryProcessor/types/ClauseInformation.h"

#include <component/PKB/PKB.h>
#include <component/QueryProcessor/types/UnformattedQueryResult.h>
//#include "ClauseCommand.h"

class QueryEvaluator {
  public:
    QueryEvaluator(PKB *pkb);
    UnformattedQueryResult EvaluateQuery(std::vector<Group *>);
  private:
    PKB *pkb;
    bool boolean_result;

    void ProcessGroup(QueryEvaluatorTable *table, Group group);
    void PreprocessBooleanGroup(Group group);
    bool ProcessSingleClauseBooleanGroup(Group group);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H