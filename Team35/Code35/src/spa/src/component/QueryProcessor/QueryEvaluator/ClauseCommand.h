#ifndef AUTOTESTER_CLAUSECOMMAND_H
#define AUTOTESTER_CLAUSECOMMAND_H

#include "ClauseCommandExecutor.h"

class ClauseCommand {
  protected:
    ClauseCommandExecutor *executor;
  public:
    virtual void SetExecutor(ClauseCommandExecutor *executor) = 0;
    virtual void Execute(Clause *clause) = 0;
};

class DoubleSynonymBothPresentCommand : public ClauseCommand {
  private:
    ClauseCommandExecutor *executor;
  public:
    DoubleSynonymBothPresentCommand();
    DoubleSynonymBothPresentCommand(ClauseCommandExecutor *executor);
    void SetExecutor(ClauseCommandExecutor *executor);
    void Execute(Clause *clause);
};

class DoubleSynonymSinglePresentCommand : public ClauseCommand {
  private:
    ClauseCommandExecutor *executor;
    bool first_synonym_given;
  public:
    DoubleSynonymSinglePresentCommand(bool first_synonym_given);
    void SetExecutor(ClauseCommandExecutor *executor) override;
    void Execute(Clause *clause) override;
};

class SingleSynonymPresentCommand : public ClauseCommand {
  private:
    ClauseCommandExecutor *executor;
    bool synonym_is_first_param;
  public:
    SingleSynonymPresentCommand(bool synonym_is_first_param);
    void SetExecutor(ClauseCommandExecutor *executor) override;
    void Execute(Clause *clause) override;
};


#endif //AUTOTESTER_CLAUSECOMMAND_H
