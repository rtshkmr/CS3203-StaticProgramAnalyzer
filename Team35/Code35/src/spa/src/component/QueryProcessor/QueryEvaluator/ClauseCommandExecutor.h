#ifndef AUTOTESTER_CLAUSECOMMANDEXECUTOR_H
#define AUTOTESTER_CLAUSECOMMANDEXECUTOR_H

//#include <string>
//#include <vector>
//#include "component/QueryProcessor/types/Types.h"
//#include "component/PKB/PKB.h"
//#include "ClauseCommandExecutor.h"
//#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
//#include "PKBQueryCommand.h"
//#include "ClauseStrategy.h"

#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
#include <component/QueryProcessor/types/IntermediateTable.h>
#include <model/Statement.h>

class ClauseCommandExecutor {
  private:
    QueryEvaluatorTable *group_table;
    IntermediateTable *table;
    bool HasExpressionMatch(Pattern *pattern , AssignEntity *assign_entity);
    bool HasAssignPatternRelationship(Entity * assign_entity, Entity *variable_entity, Pattern *pattern);
    bool HasWhileOrIfPatternRelationship(Entity * assign_entity, Entity *variable_entity);
    std::tuple<bool, int>PatternRowAdditionForStmt(int index, Synonym *synonym_column_to_add, Entity* entity_in_table, Pattern *pattern);
    std::tuple<bool, int>PatternRowAdditionForVariable(int index, Synonym *synonym_column_to_add, Entity* entity_in_table);
    bool HasPatternValueMatch(Entity *stmt_entity_in_table, std::string value, Pattern *pattern);
    std::vector<Variable*> RetrieveVariablesFromStmt(Entity *stmt_entity);
  public:
    ClauseCommandExecutor(QueryEvaluatorTable *table, IntermediateTable *intermediate_table);
    void SuchThatTwoSynonym(Clause *clause);
    void PatternTwoSynonym(Clause *clause);
    void SuchThatTwoSynonymOneInTable(Clause *clause, bool first_syn_in);
    void PatternTwoSynonymOneInTable(Clause *clause, bool first_syn_in);
    void SuchThatOneSynonym(Clause *clause, bool first_syn_in);
    void PatternOneSynonym(Clause *clause);
};

#endif //AUTOTESTER_CLAUSECOMMANDEXECUTOR_H
