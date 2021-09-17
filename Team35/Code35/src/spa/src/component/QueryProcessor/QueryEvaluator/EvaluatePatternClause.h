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

void ProcessPatternClause(Pattern pattern, QueryEvaluatorTable* table, PKB pkb,
                          std::unordered_map<std::string, DesignEntity> synonym_design_entity_map);
void EvaluatePatternDoubleSynonym(Pattern p, QueryEvaluatorTable* table, PKB pkb);
void EvaluatePatternDoubleSynonymFirstPresent(Pattern p, QueryEvaluatorTable* table, PKB pkb);
void EvaluatePatternDoubleSynonymSecondPresent(Pattern p, QueryEvaluatorTable* table, PKB pkb,
                                               std::unordered_map<std::string, DesignEntity> synonym_design_entity_map);
void EvaluatePatternSingleSynonym(Pattern p, QueryEvaluatorTable* table, PKB pkb);
bool HasExpressionMatch(Pattern p, AssignEntity assign_entity);

#endif //AUTOTESTER_EVALUATEPATTERNCLAUSE_H
