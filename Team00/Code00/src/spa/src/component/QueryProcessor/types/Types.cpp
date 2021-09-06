#include "Types.h"

bool Group::addClauseToVector(Clause clause) {
  clauses.push_back(clause);
}

std::vector<Clause> Group::getClauses() {
  return clauses;
}

bool Group::containsTargetSynonym() {
  return hasTargetSynonym;
}
