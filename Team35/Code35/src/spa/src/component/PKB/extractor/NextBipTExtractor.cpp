#include <util/Utility.h>
#include "NextBipTExtractor.h"
#include "RuntimeExtractor.h"

NextBipTExtractor::NextBipTExtractor(RuntimeMediator* rtm, PKB* pkb) {
  pkb_ = pkb;
  rtm_ = rtm;
}

std::vector<Entity*> NextBipTExtractor::GetRelationship(RelDirection dir, int target) {
  PopulateRelationships();
  return pkb_->GetRelationship(GetPKBRelRef(dir), std::to_string(target));
}

std::vector<Entity*> NextBipTExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  if (next_design_entities.count(de) == 0) return {};
  PopulateRelationships();
  return pkb_->GetFirstEntityOfRelationship(GetPKBRelRef(dir), de);
}

std::vector<std::tuple<Entity*, Entity*>> NextBipTExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                    DesignEntity first,
                                                                                    DesignEntity second) {
  if (next_design_entities.count(first) == 0 || next_design_entities.count(second) == 0) return {};
  PopulateRelationships();
  return pkb_->GetRelationshipByTypes(GetPKBRelRef(dir), first, second);
}

bool NextBipTExtractor::HasRelationship(RelDirection dir) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, int target) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(target));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, int first, int second) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(first), std::to_string(second));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, DesignEntity first, DesignEntity second) {
  if (next_design_entities.count(first) == 0 || next_design_entities.count(second) == 0) return false;
  return HasRelationship(dir);
}

void NextBipTExtractor::PopulateRelationships() {
  if (pkb_->HasRelationship(PKBRelRefs::kNextBipT)) return;
  if (!pkb_->HasRelationship(PKBRelRefs::kNextBip)) {
    rtm_->HasRelationship(PKBRelRefs::kNextBip);
  }
  auto non_t_string_map = pkb_->GetRelationshipMap(PKBRelRefs::kNextBip);
  std::vector<Entity*> stmt_list = pkb_->GetDesignEntities(DesignEntity::kStmt);
  non_t_map_ = *Utility::ConvertStringToEntityMapping(stmt_list, non_t_string_map);
  PopulateNextBipT();
  pkb_->PopulateRelationship(&t_map_, PKBRelRefs::kNextBipT);
  pkb_->PopulateRelationship(&reverse_t_map_, PKBRelRefs::kPrevBipT);
}

PKBRelRefs NextBipTExtractor::GetPKBRelRef(RelDirection dir) {
  return dir == RelDirection::kForward ? PKBRelRefs::kNextBipT : PKBRelRefs::kPrevBipT;
}

void NextBipTExtractor::PopulateNextBipT() {
  for (auto[first_arg, second] : non_t_map_) {
    std::vector<Entity*> visited_nodes; // visited list
    ExtractRelationships(first_arg, first_arg, &visited_nodes);
  }
}

/**
 * Extracts NextBipT for the first_arg by recursing into the second arg and records visited_nodes.
 * Adds relationships to the local map.
 * @param first_arg First argument in NextBipT.
 * @param second_arg Second argument in NextBipT.
 * @param visited_nodes List of U that have been visited.
 */
void NextBipTExtractor::ExtractRelationships(Entity* first_arg,
                                             Entity* second_arg,
                                             std::vector<Entity*>* visited_nodes) {
  bool was_visited = std::find(visited_nodes->begin(), visited_nodes->end(), second_arg) != visited_nodes->end();
  if (non_t_map_.count(second_arg) == 0 || was_visited) {
    visited_nodes->push_back(second_arg);
    return;
  }

  // must record visited before recurring to avoid cycle
  visited_nodes->push_back(second_arg);

  std::list<Entity*>* non_t_list = non_t_map_.find(second_arg)->second;
  for (Entity* third_arg: *non_t_list) {
    AddRelationship(first_arg, third_arg);
    ExtractRelationships(first_arg, third_arg, visited_nodes);
  }
}

void NextBipTExtractor::AddRelationship(Entity* key, Entity* value) {
  Deliverable::AddRelationshipToMap(&t_map_, key, value);
  Deliverable::AddRelationshipToMap(&reverse_t_map_, value, key);
}
