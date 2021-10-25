#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <component/PKB/PKB.h>
#include <component/QueryProcessor/types/UnformattedQueryResult.h>

class QueryEvaluator {
  public:
    QueryEvaluator(PKB *pkb);
    UnformattedQueryResult EvaluateQuery(const std::vector<Group *>&);
  private:
    PKB *pkb;
    bool boolean_result;

    void ProcessGroup(QueryEvaluatorTable *table, Group *group);
    bool ProcessBooleanGroupWithoutSynonym(Group *group);
    void ProcessBooleanGroupWithSynonym(Group *group, Synonym *main_synonym);
    void PreprocessBooleanGroup(Group *group);
    void PreprocessNonBooleanGroup(Group *group, QueryEvaluatorTable *table);
    Synonym* ProcessMainSynonymFromSuchThat(Clause *first_clause, Group *group);
    static Synonym* ProcessMainSynonymFromPattern(Clause *first_clause);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H