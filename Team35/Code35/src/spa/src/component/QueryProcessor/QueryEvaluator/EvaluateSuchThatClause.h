#ifndef AUTOTESTER_EVALUATESUCHTHATCLAUSE_H
#define AUTOTESTER_EVALUATESUCHTHATCLAUSE_H

#include <vector>
#include <list>
#include <string>
#include <component/QueryProcessor/types/Types.h>
#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
#include "component/PKB/PKB.h"
#include "PKBDispatcher.h"

void EvaluateSuchThatClause(const SuchThat& such_that_clause, QueryEvaluatorTable* table, const PKB& pkb,
                            const std::list<Synonym>& synonym_list);
void ProcessQueryGivenFirstSynonym(const PKB& pkb, const SuchThat& such_that_clause, QueryEvaluatorTable* table);
void ProcessQueryGivenSecondSynonym(const PKB& pkb, const SuchThat& such_that_clause, QueryEvaluatorTable* table);
void BothSynonymInTable(const PKB& pkb, const SuchThat& such_that_clause, QueryEvaluatorTable* table);
void ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable* table,
                      RelRef relationship, bool givenFirstParam, const PKB& pkb);
bool SynonymTypeMatches(DesignEntity current_synonym, DesignEntity synonym_type_to_match);
bool EvaluateNoSynonym(const SuchThat& st, const PKB& pkb);
Synonym GetSynonymFromName(const std::string& synonym_name, const std::list<Synonym>& synonym_list);

#endif //AUTOTESTER_EVALUATESUCHTHATCLAUSE_H
