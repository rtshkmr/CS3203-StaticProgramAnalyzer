//
// Created by Max Ng on 13/9/21.
//

#ifndef AUTOTESTER_EVALUATEPATTERNCLAUSE_H
#define AUTOTESTER_EVALUATEPATTERNCLAUSE_H

#include <string>
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include "QueryPKB.h"
#include "model/Statement.h"

QueryEvaluatorTable ProcessPatternClause(Pattern pattern, QueryEvaluatorTable table, PKB pkb);
QueryEvaluatorTable EvaluatePatternDoubleSynonym(Pattern p, QueryEvaluatorTable table, PKB pkb);
QueryEvaluatorTable EvaluatePatternDoubleSynonymFirstPresent(Pattern p, QueryEvaluatorTable table, PKB pkb);
QueryEvaluatorTable EvaluatePatternDoubleSynonymSecondPresent(Pattern p, QueryEvaluatorTable table, PKB pkb);
QueryEvaluatorTable EvaluatePatternSingleSynonym(Pattern p, QueryEvaluatorTable table, PKB pkb);
bool HasExpressionMatch(Pattern p, AssignEntity assign_entity);

#endif //AUTOTESTER_EVALUATEPATTERNCLAUSE_H
