#include "QueryOptimizer.h"
#include "QueryGrouper.h"
#include <algorithm>

void QueryOptimizer::Optimize() {
  PopulateSynAdjacencyList();
  PopulateWeightedClausesList();
  QueryGrouper::AdvancedGroupClauses(& weighted_clauses, & weighted_groups, & target_syn_attrs_list, &target_synonyms_map,
                                     & map_of_syn_to_clause_indices);
  UpdateClauseWeights();
  ReorderClausesWithinWeightedGroups();
  PopulateGroupsList();
  ReorderGroups();
  // free memory for weighted
  // FreeWeightedGroupsList();
}

// populate intermediate data structure used for advanced grouping algorithm.
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

void QueryOptimizer::PopulateWeightedClausesList() {
  for (auto cl : clauses) {
    weighted_clauses.push_back(new WeightedClause(cl));
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

// TODO: make grouping algorithm work with a vector of weightedgroups. The only thing special about weightedgroup
// is that it works with weightedclauses. weightedclause contains the clause obj itself as an attribute, and
// has a penalty/scoring system, lower the better
// getter method that will calculate the organic clause penalty based on type/nSyns (default method is called: updateWeight)
//  // ReorderClauses method will take the vector of weighted groups, call updateWeight()),
// but has another field for subgroup penalty. -> ie based on subgrouping.
// ReorderClauses method will take the vector of weighted groups, and sort each group of clauses based on
// getScore() method call, which will take default score + penalty on the fly.
// finally, we loop over each group, and for each weightedclause, we just need to clear vector<grooups*>, and rebuild it based on new order.
void QueryOptimizer::ReorderClausesWithinWeightedGroups() {
  auto comparator = [](WeightedClause* const& c1, WeightedClause* const& c2) -> bool {
    return c1->GetWeight() < c2->GetWeight();
  };
  for (int i = 0; i < weighted_groups.size(); i++) {
    std::sort(weighted_groups[i]->weighted_clauses.begin(),weighted_groups[i]->weighted_clauses.end(), comparator);
  }
}

void QueryOptimizer::UpdateClauseWeights() {

}

void QueryOptimizer::PopulateGroupsList() {
  for (WeightedGroup* w_g : weighted_groups) {
    std::vector<WeightedClause*> w_c = w_g->weighted_clauses;
    Group* g = w_g->group;
    for (auto w_cl : w_c) g->AddClauseToVector(w_cl->clause);
    groups.push_back(g);
  }
}