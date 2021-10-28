#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <component/PKB/DBManager.h>
#include <component/QueryProcessor/types/UnformattedQueryResult.h>

class QueryEvaluator {
  public:
    QueryEvaluator(DBManager *db_manager);
    UnformattedQueryResult EvaluateQuery(const std::vector<Group *>&);
  private:
    DBManager *db_manager;
    bool boolean_result;

    void ProcessGroup(QueryEvaluatorTable *table, Group *group);
    void ProcessBooleanGroupWithoutSynonym(Group *group);
    void ProcessBooleanGroupWithSynonym(Group *group, Synonym *main_synonym);
    void PreprocessBooleanGroup(Group *group);
    void PreprocessNonBooleanGroup(Group *group, QueryEvaluatorTable *table);
    static Synonym *ProcessMainSynonymFromSuchThat(Clause *first_clause);
    static Synonym* ProcessMainSynonymFromPattern(Clause *first_clause);
    static Synonym* ProcessMainSynonymFromWith(Clause *first_clause);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H