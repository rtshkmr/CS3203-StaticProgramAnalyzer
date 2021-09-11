#include "Types.h"

bool Group::AddClauseToVector(Clause* clause) {
  auto initialSize = clauses.size();
  clauses.push_back(clause);
  return (clauses.size() == initialSize + 1);
}

std::vector<Clause*> Group::GetClauses() {
  return clauses;
}

bool Group::ContainsTargetSynonym() {
  return has_target_synonym;
}


