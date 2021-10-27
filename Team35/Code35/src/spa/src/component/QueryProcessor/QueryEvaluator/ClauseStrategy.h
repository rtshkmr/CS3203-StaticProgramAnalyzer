#ifndef AUTOTESTER_CLAUSECOMMANDINVOKER_H
#define AUTOTESTER_CLAUSECOMMANDINVOKER_H


#include <tuple>
#include "DBQueryCommand.h"
#include "ClauseCommand.h"

class ClauseStrategy {
  public:
    virtual std::tuple<DBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) = 0;

  protected:
    static std::tuple<DBQueryCommand *, ClauseCommand *>
    DetermineDoubleSynonymCommands(QueryEvaluatorTable *table, Synonym *first_synonym, Synonym *second_synonym, Clause *clause);
    static std::tuple<DBQueryCommand *, ClauseCommand *>
    DetermineSingleSynonymCommand(Clause *clause, bool synonym_is_first_param);
};

class ClauseContext {
  private:
    QueryEvaluatorTable *group_table;
  public:
    ClauseContext(QueryEvaluatorTable *table);
    std::tuple<DBQueryCommand *, ClauseCommand *> ProcessClause(Clause *clause);
};

class SuchThatStrategy : public ClauseStrategy {
  public:
    SuchThatStrategy() {};
    std::tuple<DBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) override;
};

class PatternStrategy : public ClauseStrategy {
  public:
    PatternStrategy() {};
    std::tuple<DBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) override;
};

#endif //AUTOTESTER_CLAUSECOMMANDINVOKER_H
