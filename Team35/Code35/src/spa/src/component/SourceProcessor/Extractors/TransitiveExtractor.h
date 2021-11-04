#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_

#include <unordered_map>
#include <exception/SpaException.h>
#include <datatype/Deliverable.h>

/**
 * Encapsulates the DFS extraction of transitive relationships from non-transitive relationships.
 * @tparam U Type of Entity that is involved in the relationship.
 */
template<class U>
class TransitiveExtractor {
 public:
  TransitiveExtractor() = default;
  void Extract(std::unordered_map<U*, std::list<U*>*>* t_map,
               std::unordered_map<U*, std::list<U*>*>* reverse_t_map,
               std::unordered_map<U*, std::list<U*>*>* non_t_map);
 private:
  std::unordered_map<U*, std::list<U*>*>* t_map_; // transitive relationship map
  std::unordered_map<U*, std::list<U*>*>* reverse_t_map_; // transitive reverse relationship map
  std::unordered_map<U*, std::list<U*>*>* non_t_map_; // non-transitive relationship map

  std::list<U*>* GetExtractions(U* first_arg, std::vector<U*>* visited_nodes);
  void AddRelationship(U* key, U* value);
};

/**
 * Extracts transitive relationships into t_map and reverse_t_map from non transitive relationships in non_t_map.
 * @tparam U Type of Entity that is involved in the relationship.
 * @param t_map Transitive relationship map.
 * @param reverse_t_map Transitive reverse relationship map.
 * @param non_t_map Non-transitive relationship map.
 * @throws SemanticException when there is cyclic relationship.
 */
template<typename U>
void TransitiveExtractor<U>::Extract(std::unordered_map<U*, std::list<U*>*>* t_map,
                                     std::unordered_map<U*, std::list<U*>*>* reverse_t_map,
                                     std::unordered_map<U*, std::list<U*>*>* non_t_map) {
  t_map_ = t_map;
  reverse_t_map_ = reverse_t_map;
  non_t_map_ = non_t_map;

  std::vector<U*> visited_nodes; // visited list
  for (typename std::unordered_map<U*, std::list<U*>*>::iterator i = non_t_map_->begin(); i != non_t_map_->end(); ++i) {
    bool in_t_map = t_map_->count(i->first) != 0;
    bool was_visited = std::find(visited_nodes.begin(), visited_nodes.end(), i->first)
        != visited_nodes.end();  // procedure found in visited_nodes
    if (in_t_map || was_visited) {
      continue;
    }

    GetExtractions(i->first, &visited_nodes);
  }
}

/**
 * Recursively extracts on the first_arg and records it in visited_nodes.
 * @tparam U Type of Entity that is involved in the relationship.
 * @param first_arg First argument in the relationship.
 * @param visited_nodes List of U that have been visited.
 * @return List of U that in transitive relationship with the first_arg.
 * @throws SemanticException when there is cyclic relationship.
 */
template<typename U>
std::list<U*>* TransitiveExtractor<U>::GetExtractions(U* first_arg, std::vector<U*>* visited_nodes) {
  // node has Relationship*, implies that it was visited.
  if (t_map_->count(first_arg) != 0) {
    return t_map_->find(first_arg)->second;
  }
  bool was_visited = std::find(visited_nodes->begin(), visited_nodes->end(), first_arg) != visited_nodes->end();
  // node does not have Relationship, found in visited_nodes, does not have Relationship*
  if (non_t_map_->count(first_arg) == 0 || was_visited) {
    visited_nodes->push_back(first_arg);
    return new std::list<U*>{first_arg};
  }

  // must record visited before recurring to avoid cycle
  visited_nodes->push_back(first_arg);

  std::list<U*>* non_t_list = non_t_map_->find(first_arg)->second;
  for (U* second_arg: *non_t_list) {
    std::list<U*>* extracted_ts = GetExtractions(second_arg, visited_nodes);
    for (U* extracted_t: *extracted_ts) {
      if (extracted_t == first_arg) {
        throw SemanticException("Cyclic relationship detected.");
      }
      AddRelationship(first_arg, extracted_t);
    }
    AddRelationship(first_arg, second_arg);
  }
  return t_map_->find(first_arg)->second;
}

template<typename U>
void TransitiveExtractor<U>::AddRelationship(U* key, U* value) {
  Deliverable::AddRelationshipToMap(t_map_, key, value);
  Deliverable::AddRelationshipToMap(reverse_t_map_, value, key);
}

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_
