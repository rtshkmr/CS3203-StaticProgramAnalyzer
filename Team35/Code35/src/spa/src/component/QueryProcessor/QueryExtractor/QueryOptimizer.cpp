#include "QueryOptimizer.h"
#include <algorithm>

/**
 * Updates the ordering of Groups in the given list of Groups based on the following heuristic:
 * In increasing order of size, boolean groups (don't contain a target synonym) are placed before non-boolean groups.
 * Since we need to sort boolean groups by order of size, this optimization has loglinear time complexity in the
 * number of boolean groups.
 */
void QueryOptimizer::ReorderGroups(std::vector<Group*>* groups) {
  if (!this->are_optimizations_enabled) {
    return;
  }

  std::vector<Group*> boolean_groups;
  boolean_groups.reserve(groups->size());
  std::vector<Group*> non_boolean_groups;
  non_boolean_groups.reserve(groups->size());

  for (Group* g : *groups) {
    if (g->ContainsTargetSynonym()) {
      non_boolean_groups.push_back(g);
    } else {
      boolean_groups.push_back(g);
    }
  }

  auto comparator = [](Group* const& g1, Group* const& g2) -> bool {
    return g1->GetGroupSize() < g2->GetGroupSize();
  };
  std::sort(boolean_groups.begin(), boolean_groups.end(), comparator);

  boolean_groups.insert(boolean_groups.end(), non_boolean_groups.begin(), non_boolean_groups.end());
  groups->swap(boolean_groups);
}

