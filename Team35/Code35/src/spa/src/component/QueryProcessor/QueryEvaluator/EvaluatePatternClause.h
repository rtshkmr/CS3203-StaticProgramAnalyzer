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

void ProcessPatternClause(const Pattern& pattern, QueryEvaluatorTable* table, const PKB& pkb,
                          std::unordered_map<std::string, DesignEntity> synonym_design_entity_map);
void EvaluatePatternDoubleSynonym(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb);
void EvaluatePatternDoubleSynonymFirstPresent(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb);
void EvaluatePatternDoubleSynonymSecondPresent(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb,
                                               std::unordered_map<std::string, DesignEntity> synonym_design_entity_map);
void EvaluatePatternSingleSynonym(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb);
bool HasExpressionMatch(const Pattern& p, AssignEntity assign_entity);

#endif //AUTOTESTER_EVALUATEPATTERNCLAUSE_H
