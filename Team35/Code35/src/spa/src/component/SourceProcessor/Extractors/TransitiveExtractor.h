#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_

#include <unordered_map>
#include <exception/SpaException.h>
#include <datatype/Deliverable.h>

enum class TransitiveRel {
  kFollows,
  kParent,
  kCalls
};

/**
 * Encapsulates the extraction of transitive relationships from non-transitive relationships.
 * @tparam U Type of Entity that is involved in the relationship.
 */
template<class U>
class TransitiveExtractor {
 public:
  TransitiveExtractor(Deliverable* deliverable);
  void Extract(std::unordered_map<U*, std::list<U*>*>* t_map,
               std::unordered_map<U*, std::list<U*>*>* non_t_map,
               TransitiveRel rel_type);
 private:
  Deliverable* deliverable_;
  TransitiveRel rel_type_;
  std::unordered_map<U*, std::list<U*>*>* t_map_; // transitive relationship map
  std::unordered_map<U*, std::list<U*>*>* non_t_map_; // non-transitive relationship map

  std::list<U*>* GetExtractions(U* first_arg, std::vector<U*>* visited_nodes);
  void AddRelationship(U* key, U* value);
};

template<typename U>
TransitiveExtractor<U>::TransitiveExtractor(Deliverable* deliverable) {
  deliverable_ = deliverable;
}

/**
 * Extracts transitive relationships into t_map from non transitive relationships in non_t_map.
 * @tparam U Type of Entity that is involved in the relationship.
 * @param t_map Transitive relationship map.
 * @param non_t_map Non-transitive relationship map.
 * @param rel_type Type of relationship.
 * @throws SemanticException when there is cyclic relationship.
 *
 * pseudocode - dfs with U as nodes and relationships as edge
 * for all U in non_transitive_map
 *  recurse with first_arg if not in transitive_map and not visited
 *  for all second_arg
 *  if no second_arg
 *      + visited
 *      return list with this first_arg
 *  else if visited get transitive args
 *  else recurse on every second_arg not in t_map
 *      add second_arg to this first_arg with returned list
 *      if this first_arg is found in returned list, throw SemanticException
 *  + visited
 *  return this second_args
 */
template<typename U>
void TransitiveExtractor<U>::Extract(std::unordered_map<U*, std::list<U*>*>* t_map,
                                     std::unordered_map<U*, std::list<U*>*>* non_t_map,
                                     TransitiveRel rel_type) {
  t_map_ = t_map;
  non_t_map_ = non_t_map;
  rel_type_ = rel_type;

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
      if (extracted_t->GetName() == first_arg->GetName()) { // uses overloaded ==
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
  if (rel_type_ == TransitiveRel::kCalls) {
    deliverable_->AddCallsTransitiveRelationship(key, value);
//  } else if (rel_type_ == TransitiveRel::kFollows) {
//    deliverable_->AddFollowsTransitiveRelationship(key, value);
//  } else if (rel_type_ == TransitiveRel::kParent) {
//    deliverable_->AddParentTransitiveRelationship(key, value);
  }
}

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_TRANSITIVEEXTRACTOR_H_
