#include "QueryOptimizer.h"
#include "QueryGrouper.h"
#include <algorithm>

void QueryOptimizer::Optimize() {
  PopulateSynAdjacencyList();
  QueryGrouper::AdvancedGroupClauses(& clauses, & groups, & target_syn_attrs_list, &target_synonyms_map,
                                     & map_of_syn_to_clause_indices);
  ReorderGroups();
}

void QueryOptimizer::PopulateSynAdjacencyList() {
  for (int i = 0; i < clauses.size(); i++) {
    std::vector<std::string> syns = clauses[i]->GetAllSynonymNamesOfClause();
    for (std::string s : syns) {
      if (this->map_of_syn_to_clause_indices.find(s) != this->map_of_syn_to_clause_indices.end()) {
        this->map_of_syn_to_clause_indices.at(s).push_back(i);
      } else {
        std::vector<int> v { i };
        this->map_of_syn_to_clause_indices.insert(std::make_pair(s, v));
      }
    }
  }
}

/**
 * Updates the ordering of Groups in the given list of Groups based on the following heuristic:
 * In increasing order of size, boolean groups (don't contain a target synonym) are placed before non-boolean groups.
 * Since we need to sort boolean groups by order of size, this optimization has loglinear time complexity in the
 * number of boolean groups.
 */
void QueryOptimizer::ReorderGroups() {
  std::vector<Group*> boolean_groups;
  boolean_groups.reserve(this->groups.size());
  std::vector<Group*> non_boolean_groups;
  non_boolean_groups.reserve(this->groups.size());

  for (Group* g : this->groups) {
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
  this->groups.swap(boolean_groups);
}

