#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <component/QueryProcessor/types/Types.h>



// representation of a clause with weights
struct WeightedClause {
  Clause* clause;
  int weight = 0;
  int subgroup_penalty = 0;
  WeightedClause(Clause* cl) { clause = cl; };
  int GetWeight() { return weight + subgroup_penalty; };
};

// a container containing clauses with weights
struct WeightedGroup {
  Group* group;
  std::vector<WeightedClause*> weighted_clauses;
  WeightedGroup(Group* grp) { group = grp; };
};

class QueryOptimizer {
  private:
    std::vector<Clause*>& clauses;
    std::vector<Group*>& groups;
    std::vector<WeightedClause*> weighted_clauses;
    std::vector<WeightedGroup*> weighted_groups;
    std::vector<std::pair<Synonym*, Attribute>>& target_syn_attrs_list;
    std::unordered_map<std::string, Synonym*>& target_synonyms_map;
    std::unordered_map<std::string, std::vector<int>> map_of_syn_to_clause_indices;
    void PopulateSynAdjacencyList();
    void PopulateWeightedClausesList();
    void ReorderGroups();
    void ReorderClausesWithinWeightedGroups();
    void UpdateClauseWeights();
    void PopulateGroupsList();
    static const int subgroup_penalty = 50;
  public:
    QueryOptimizer(std::vector<Clause*>& clauses, std::vector<Group*>& groups,
                   std::vector<std::pair<Synonym*, Attribute>>& target_syn_attrs_list,
                   std::unordered_map<std::string, Synonym*>& target_synonyms_map) :
                   clauses(clauses), groups(groups), target_syn_attrs_list(target_syn_attrs_list),
                   target_synonyms_map(target_synonyms_map) {};
    void Optimize();
    static int GetSubgroupPenalty() { return subgroup_penalty; };
};


#endif //AUTOTESTER_QUERYOPTIMIZER_H
