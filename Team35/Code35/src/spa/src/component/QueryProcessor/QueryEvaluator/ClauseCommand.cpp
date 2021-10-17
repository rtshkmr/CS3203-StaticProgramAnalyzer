#include "ClauseCommand.h"

DoubleSynonymBothPresentCommand::DoubleSynonymBothPresentCommand() : executor(nullptr) {}

DoubleSynonymBothPresentCommand::DoubleSynonymBothPresentCommand(ClauseCommandExecutor *executor): executor(executor) {}

void DoubleSynonymBothPresentCommand::SetExecutor(ClauseCommandExecutor *executor) {
  delete this->executor;
  this->executor = executor;
}

void DoubleSynonymBothPresentCommand::Execute(Clause *clause) {
  if (typeid(*clause) == typeid(Pattern)) {
    this->executor->PatternTwoSynonym(clause);
  } else {
    this->executor->SuchThatTwoSynonym(clause);
  }
}

DoubleSynonymSinglePresentCommand::DoubleSynonymSinglePresentCommand(bool first_synonym_present) :
first_synonym_given(first_synonym_present), executor(nullptr) {}

void DoubleSynonymSinglePresentCommand::SetExecutor(ClauseCommandExecutor *executor) {
  delete this->executor;
  this->executor = executor;
}

void DoubleSynonymSinglePresentCommand::Execute(Clause *clause) {
  if (typeid(*clause) == typeid(Pattern)) {
    this->executor->PatternTwoSynonymOneInTable(clause, first_synonym_given);
  } else {
    this->executor->SuchThatTwoSynonymOneInTable(clause, first_synonym_given);
  }
}

SingleSynonymPresentCommand::SingleSynonymPresentCommand(bool synonym_is_first_param) : synonym_is_first_param(synonym_is_first_param),
executor(nullptr) {}

void SingleSynonymPresentCommand::SetExecutor(ClauseCommandExecutor *executor) {
  delete this->executor;
  this->executor = executor;
}

void SingleSynonymPresentCommand::Execute(Clause *clause) {
  if (typeid(*clause) == typeid(Pattern)) {
    this->executor->PatternOneSynonym(clause);
  } else {
    this->executor->SuchThatOneSynonym(clause, synonym_is_first_param);
  }
}
