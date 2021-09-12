//
// Created by Max Ng on 11/9/21.
//

#ifndef AUTOTESTER_EVALUATESUCHTHATSINGLESYNONYM_H
#define AUTOTESTER_EVALUATESUCHTHATSINGLESYNONYM_H

#include <vector>
#include <list>
#include <string>
#include <component/QueryProcessor/types/Types.h>
#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
#include "component/PKB/PKB.h"

QueryEvaluatorTable ProcessQueryGivenFirstSynonym(PKB pkb, SuchThat such_that_clause, RelRef relation, QueryEvaluatorTable table);
QueryEvaluatorTable ProcessQueryGivenSecondSynonym(PKB pkb, SuchThat such_that_clause, RelRef query_relation, QueryEvaluatorTable table);

#endif //AUTOTESTER_EVALUATESUCHTHATSINGLESYNONYM_H
