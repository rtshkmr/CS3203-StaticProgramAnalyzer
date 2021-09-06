#include "Types.h"

bool Group::addClauseToVector(Clause clause) {
  auto initialSize = clauses.size();
  clauses.push_back(clause);
  return (clauses.size() == initialSize + 1);
}

std::vector<Clause> Group::getClauses() {
  return clauses;
}

bool Group::containsTargetSynonym() {
  return hasTargetSynonym;
}
