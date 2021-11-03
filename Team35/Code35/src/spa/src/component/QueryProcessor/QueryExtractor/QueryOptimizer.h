#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <component/QueryProcessor/types/Types.h>

class QueryOptimizer {
  private:
    std::vector<Clause*>& clauses;
    std::vector<Group*>& groups;
    std::vector<std::pair<Synonym*, Attribute>>& target_syn_attrs_list;
    std::unordered_map<std::string, Synonym*>& target_synonyms_map;
    std::unordered_map<std::string, std::vector<int>> map_of_syn_to_clause_indices;
    void PopulateSynAdjacencyList();
    void ReorderGroups();
  public:
    QueryOptimizer(std::vector<Clause*>& clauses, std::vector<Group*>& groups,
                   std::vector<std::pair<Synonym*, Attribute>>& target_syn_attrs_list,
                   std::unordered_map<std::string, Synonym*>& target_synonyms_map) :
                   clauses(clauses), groups(groups), target_syn_attrs_list(target_syn_attrs_list),
                   target_synonyms_map(target_synonyms_map) {};
    void Optimize();
};


#endif //AUTOTESTER_QUERYOPTIMIZER_H
