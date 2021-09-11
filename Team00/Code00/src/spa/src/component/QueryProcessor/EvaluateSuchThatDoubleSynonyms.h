//
// Created by Max Ng on 11/9/21.
//

#ifndef AUTOTESTER_EVALUATESUCHTHATDOUBLESYNONYMS_H
#define AUTOTESTER_EVALUATESUCHTHATDOUBLESYNONYMS_H

#include <string>
#include <vector>
#include "component/QueryProcessor/types/Types.h"
#include "QueryPKB.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"

QueryEvaluatorTable BothSynonymInTable(PKB pkb, SuchThat such_that_clause, RelRef relation, QueryEvaluatorTable table);

#endif //AUTOTESTER_EVALUATESUCHTHATDOUBLESYNONYMS_H
