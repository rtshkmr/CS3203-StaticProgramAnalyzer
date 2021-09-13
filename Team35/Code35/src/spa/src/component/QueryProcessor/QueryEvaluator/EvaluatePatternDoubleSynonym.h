//
// Created by Max Ng on 13/9/21.
//

#ifndef AUTOTESTER_EVALUATEPATTERNDOUBLESYNONYM_H
#define AUTOTESTER_EVALUATEPATTERNDOUBLESYNONYM_H

#include <string>
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include "QueryPKB.h"
#include "model/Statement.h"

QueryEvaluatorTable EvaluatePatternDoubleSynonym(Pattern p, QueryEvaluatorTable table, PKB pkb);

#endif //AUTOTESTER_EVALUATEPATTERNDOUBLESYNONYM_H
