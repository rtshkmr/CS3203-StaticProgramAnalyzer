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
  void UpdateClauseWeight(int w) { weight = w; };
};

// a container containing clauses with weights
struct WeightedGroup {
  Group* group;
  std::vector<WeightedClause*> weighted_clauses;
  WeightedGroup(Group* grp) { group = grp; };
};

/**
 * QueryOptimizer is an optional component whose functionality does not affect correctness of the program, but
 * improves the speed of query evaluation. Its key features include grouping clauses together,
 * reordering across groups of clauses, reordering of clauses within a group, as well as query caching.
 */
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
    void UpdateClauseWeight(WeightedClause* cl);
    int GetTypePenalty(Clause* cl);
    int GetNumberOfSynonymsPenalty(Clause* cl);
    void PopulateGroupsList();
    void FreeWeightedLists();
  public:
    QueryOptimizer(std::vector<Clause*>& clauses, std::vector<Group*>& groups,
                   std::vector<std::pair<Synonym*, Attribute>>& target_syn_attrs_list,
                   std::unordered_map<std::string, Synonym*>& target_synonyms_map) :
                   clauses(clauses), groups(groups), target_syn_attrs_list(target_syn_attrs_list),
                   target_synonyms_map(target_synonyms_map) {};
    void Optimize();
    static int GetSubgroupPenalty();
};

#endif //AUTOTESTER_QUERYOPTIMIZER_H
