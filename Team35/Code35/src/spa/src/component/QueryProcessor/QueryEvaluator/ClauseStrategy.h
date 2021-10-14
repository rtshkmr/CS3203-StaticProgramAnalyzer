#ifndef AUTOTESTER_CLAUSECOMMANDINVOKER_H
#define AUTOTESTER_CLAUSECOMMANDINVOKER_H


#include <tuple>
#include "PKBQueryCommand.h"
#include "ClauseCommand.h"

class ClauseStrategy {
  public:
    virtual ~ClauseStrategy() {};
    virtual std::tuple<PKBQueryCommand *, ClauseCommand *>
    DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) = 0;
};

class ClauseContext {
  private:
    QueryEvaluatorTable *group_table;
  public:
    ~ ClauseContext() {
      delete this->group_table;
    }
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

#endif //AUTOTESTER_CLAUSECOMMANDINVOKER_H
