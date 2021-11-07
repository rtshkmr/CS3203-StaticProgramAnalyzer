#ifndef AUTOTESTER_CLAUSECOMMANDEXECUTOR_H
#define AUTOTESTER_CLAUSECOMMANDEXECUTOR_H

#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
#include <component/QueryProcessor/types/IntermediateTable.h>
#include <model/Statement.h>

class ClauseCommandExecutor {
  private:
    QueryEvaluatorTable *group_table;
    IntermediateTable *table;

    bool DoubleSynonymIntersectionCheck(Clause *clause, int index);
    bool SuchThatDoubleSynonymIntersection(SuchThat *clause, int index);
    bool PatternDoubleSynonymIntersection(Pattern *clause, int index);
    bool WithDoubleSynonymIntersection(With *with_clause, int index);
    bool HasAssignPatternRelationship(Entity * assign_entity, Entity *variable_entity, Pattern *pattern);
    bool HasWhileOrIfPatternRelationship(Entity * assign_entity, Entity *variable_entity);
    static std::string RetrieveEntityAttribute(Entity *entity, Attribute attribute);
    std::tuple<bool, int> DoubleSynonymExpansionCheck(Clause *clause, bool first_syn_in, int index);
    std::tuple<bool, int> SuchThatRowAddition(SuchThat *clause, bool first_syn_in, int index);
    std::tuple<bool, int> PatternRowAddition(Pattern *clause, bool first_syn_in, int index);
    std::tuple<bool, int> WithRowAddition(With *with_clause, bool first_syn_in, int index);
    std::tuple<bool, int>PatternRowAdditionForStmt(int index, Synonym *synonym_column_to_add, Entity* entity_in_table, Pattern *pattern);
    std::tuple<bool, int>PatternRowAdditionForVariable(int index, Synonym *synonym_column_to_add, Entity* entity_in_table, Pattern *pattern);
    static bool HasPatternValueMatch(Entity *stmt_entity_in_table, const std::string& value, Pattern *pattern);
    static std::vector<Variable*> RetrieveVariablesFromStmt(Entity *stmt_entity);
    bool SingleSynonymIntersectionCheck(Clause *clause, bool first_syn_in, int index);
    bool SuchThatSingleSynonymIntersection(SuchThat *clause, bool first_syn_in, int index);
    bool PatternSingleSynonymIntersection(Pattern *clause, int index);
    bool WithSingleSynonymIntersection(With *clause, bool first_syn_in, int index);
    static bool HasExpressionMatch(Pattern *pattern , AssignEntity *assign_entity);
    std::vector<Entity*> FilterAssignSingleClause(const std::vector<Entity *>& unfiltered_entities, Clause *clause, bool is_first);
  public:
    ClauseCommandExecutor(QueryEvaluatorTable *table, IntermediateTable *intermediate_table);

    void DoubleSynonymIntersection(Clause *clause);
    void DoubleSynonymExpansion(Clause *clause, bool first_syn_in);
    void SingleSynonymIntersection(Clause *clause, bool first_syn_in);
    void DoubleSynonymCross(Clause *clause);
    void SingleSynonymCross(Clause *clause, bool syn_is_first_param);
};

#endif //AUTOTESTER_CLAUSECOMMANDEXECUTOR_H
