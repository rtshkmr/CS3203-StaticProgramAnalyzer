#include "ClauseCommand.h"

DoubleSynonymBothPresentCommand::DoubleSynonymBothPresentCommand() : executor(nullptr) {}

void DoubleSynonymBothPresentCommand::SetExecutor(ClauseCommandExecutor *executor_to_set) {
  delete this->executor;
  this->executor = executor_to_set;
}

void DoubleSynonymBothPresentCommand::Execute(Clause *clause) {
  this->executor->DoubleSynonymIntersection(clause);
}

DoubleSynonymSinglePresentCommand::DoubleSynonymSinglePresentCommand(bool first_synonym_present) :
first_synonym_given(first_synonym_present), executor(nullptr) {}

void DoubleSynonymSinglePresentCommand::SetExecutor(ClauseCommandExecutor *executor_to_set) {
  delete this->executor;
  this->executor = executor_to_set;
}

void DoubleSynonymSinglePresentCommand::Execute(Clause *clause) {
  this->executor->DoubleSynonymExpansion(clause, first_synonym_given);
}

SingleSynonymPresentCommand::SingleSynonymPresentCommand(bool synonym_is_first_param) : synonym_is_first_param(synonym_is_first_param),
executor(nullptr) {}

void SingleSynonymPresentCommand::SetExecutor(ClauseCommandExecutor *executor_to_set) {
  delete this->executor;
  this->executor = executor_to_set;
}

void SingleSynonymPresentCommand::Execute(Clause *clause) {
  this->executor->SingleSynonymIntersection(clause, synonym_is_first_param);
}
