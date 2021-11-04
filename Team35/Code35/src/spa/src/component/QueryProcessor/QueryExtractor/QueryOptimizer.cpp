#include "QueryOptimizer.h"
#include "QueryGrouper.h"
#include <algorithm>

static const int subgroup_penalty = 50;
static const int per_synonym_penalty = 15;
static const int type_with_penalty = 0;
static const int type_default_penalty = 10;
static const std::unordered_map<RelRef, int> type_relref_penalty = {
        {RelRef::kParent, 10}, {RelRef::kFollows, 10}, {RelRef::kModifiesS, 10}, {RelRef::kUsesS, 10},
        {RelRef::kModifiesP, 20}, {RelRef::kUsesP, 20}, {RelRef::kCalls, 20}, {RelRef::kNext, 30},
        {RelRef::kFollowsT, 900}, {RelRef::kParentT, 900}, {RelRef::kCallsT, 900}, {RelRef::kNextT, 1000},
        {RelRef::kAffects, 1100}, {RelRef::kAffectsT, 1200}
};

int QueryOptimizer::GetSubgroupPenalty() {
  return subgroup_penalty;
}

int QueryOptimizer::GetTypePenalty(Clause* cl) {
  if (typeid(*cl) == typeid(SuchThat)) {
    auto* such_that = dynamic_cast<SuchThat *>(cl);
    bool is_known_relref = type_relref_penalty.find(such_that->rel_ref) != type_relref_penalty.end();
    return is_known_relref ? type_relref_penalty.at(such_that->rel_ref) : type_default_penalty;
  } else if (typeid(*cl) == typeid(Pattern)) {
    return type_default_penalty;
  } else {
    return type_with_penalty;
  }
};

int QueryOptimizer::GetNumberOfSynonymsPenalty(Clause* cl) {
  int num_syns = cl->GetAllSynonymNamesOfClause().size();
  return num_syns * per_synonym_penalty;
}

void QueryOptimizer::Optimize() {
  PopulateSynAdjacencyList();
  PopulateUniqueWeightedClausesList();
  QueryGrouper::AdvancedGroupClauses(& weighted_clauses, & weighted_groups, & target_syn_attrs_list, &target_synonyms_map,
                                     & map_of_syn_to_clause_indices);
  UpdateClauseWeights();
  ReorderClausesWithinWeightedGroups();
  PopulateGroupsList();
  ReorderGroups();
  FreeWeightedLists();
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

/**
 * creates a list of weighted clauses from the original list of clauses; in the process dropping duplicate clauses.
 */
void QueryOptimizer::PopulateUniqueWeightedClausesList() {
  for (auto cl : clauses) {
    if (!HasClauseBeenSeen(cl)) {
      weighted_clauses.push_back(new WeightedClause(cl));
      SeeClause(cl);
    }
  }
}

bool QueryOptimizer::HasClauseBeenSeen(Clause* cl) {
  return false;
}

void QueryOptimizer::SeeClause(Clause* cl) {

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

void QueryOptimizer::ReorderClausesWithinWeightedGroups() {
  auto comparator = [](WeightedClause* const& c1, WeightedClause* const& c2) -> bool {
    return c1->GetWeight() < c2->GetWeight();
  };
  for (int i = 0; i < weighted_groups.size(); i++) {
    std::sort(weighted_groups[i]->weighted_clauses.begin(),weighted_groups[i]->weighted_clauses.end(), comparator);
  }
}

void QueryOptimizer::UpdateClauseWeights() {
  for (auto cl : weighted_clauses) {
    UpdateClauseWeight(cl);
  }
}

void QueryOptimizer::UpdateClauseWeight(WeightedClause* cl) {
  int total_weight = 0;
  total_weight += GetTypePenalty(cl->clause);
  total_weight += GetNumberOfSynonymsPenalty(cl->clause);
  cl->UpdateClauseWeight(total_weight);
}

void QueryOptimizer::PopulateGroupsList() {
  for (WeightedGroup* w_g : weighted_groups) {
    std::vector<WeightedClause*> w_c = w_g->weighted_clauses;
    Group* g = w_g->group;
    for (auto w_cl : w_c) g->AddClauseToVector(w_cl->clause);
    groups.push_back(g);
  }
}

void QueryOptimizer::FreeWeightedLists() {
  weighted_clauses.clear();
  weighted_clauses.shrink_to_fit();
  weighted_groups.clear();
  weighted_groups.shrink_to_fit();
}
