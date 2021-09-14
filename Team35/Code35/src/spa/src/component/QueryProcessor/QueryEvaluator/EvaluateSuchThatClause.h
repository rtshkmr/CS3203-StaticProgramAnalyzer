//
// Created by Max Ng on 14/9/21.
//

#ifndef AUTOTESTER_EVALUATESUCHTHATCLAUSE_H
#define AUTOTESTER_EVALUATESUCHTHATCLAUSE_H

#include <vector>
#include <list>
#include <string>
#include <component/QueryProcessor/types/Types.h>
#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
#include "component/PKB/PKB.h"
#include "QueryPKB.h"

void ProcessQueryGivenFirstSynonym(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table);
void ProcessQueryGivenSecondSynonym(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table);
void BothSynonymInTable(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table);
void ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable* table,
                      RelRef relationship, bool givenFirstParam, PKB pkb);
bool EvaluateNoSynonym(SuchThat st, PKB pkb);

#endif //AUTOTESTER_EVALUATESUCHTHATCLAUSE_H
