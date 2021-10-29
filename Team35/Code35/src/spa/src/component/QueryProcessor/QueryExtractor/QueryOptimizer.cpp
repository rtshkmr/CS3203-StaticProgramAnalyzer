#include "QueryOptimizer.h"

/**
 * Updates the ordering of Groups in the given list of Groups based on the following heuristic:
 * boolean groups (don't contain a target synonym) are placed before non-boolean groups.
 * As we simply create 2 buckets & merge them (instead of sorting), this runs in O(n) instead of O(nlogn).
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
  boolean_groups.insert(boolean_groups.end(), non_boolean_groups.begin(), non_boolean_groups.end());
  groups->swap(boolean_groups);
}

