#ifndef AUTOTESTER_CLAUSECOMMANDINVOKER_H
#define AUTOTESTER_CLAUSECOMMANDINVOKER_H


#include <tuple>
#include "PKBQueryCommand.h"
#include "ClauseCommand.h"

class ClauseStrategy {
  public:
    virtual std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) = 0;

  protected:
    static std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineDoubleSynonymCommands(QueryEvaluatorTable *table, Synonym *first_synonym, Synonym *second_synonym, Clause *clause);
    static std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineSingleSynonymCommand(Clause *clause, bool synonym_is_first_param);
};

class ClauseContext {
  private:
    QueryEvaluatorTable *group_table;
  public:
    ClauseContext(QueryEvaluatorTable *table);
    std::tuple<PKBQueryCommand *, ClauseCommand *> ProcessClause(Clause *clause);
};

class SuchThatStrategy : public ClauseStrategy {
  public:
    SuchThatStrategy() {};
    std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) override;
};

class PatternStrategy : public ClauseStrategy {
  public:
    PatternStrategy() {};
    std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) override;
};

class WithStrategy : public ClauseStrategy {
  public:
    WithStrategy() {};
    std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) override;
};

#endif //AUTOTESTER_CLAUSECOMMANDINVOKER_H
