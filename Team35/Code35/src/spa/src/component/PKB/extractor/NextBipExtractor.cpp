#include "NextBipExtractor.h"

#include <utility>
#include <util/Utility.h>
#include "model/CFG.h"

/**
 * This class extracts all NextBip in the first query and populates the pkb.
 * @param pkb
 */
NextBipExtractor::NextBipExtractor(PKB* pkb) {
  pkb_ = pkb;
  stmt_list_ = pkb->GetDesignEntities(DesignEntity::kStmt);
  call_list_ = pkb->GetDesignEntities(DesignEntity::kCall);
}

/**
 * Gets all s from NextBip(target, s) by using relinking all the Next of Call Statements,
 * and recording in the hashmaps in this class.
 * @param target Statement number of the first argument of NextBip.
 * @return Vector of Entities that are NextBip of the target.
 */
std::vector<Entity*> NextBipExtractor::GetRelationship(RelDirection dir, int target) {
  PopulateBipMaps();
  return pkb_->GetRelationship(GetPKBRelRef(dir), std::to_string(target));
}

/**
 * Gets all rhs of NextBip(s, s)
 * @return Vector of entities satisfying the relationship.
 */
std::vector<Entity*> NextBipExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  PopulateBipMaps();
  return pkb_->GetFirstEntityOfRelationship(GetPKBRelRef(dir), DesignEntity::kStmt);
}

std::vector<std::tuple<Entity*, Entity*>> NextBipExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                   DesignEntity first,
                                                                                   DesignEntity second) {
  PopulateBipMaps();
  return pkb_->GetRelationshipByTypes(GetPKBRelRef(dir), DesignEntity::kStmt, DesignEntity::kStmt);
}

bool NextBipExtractor::HasRelationship(RelDirection dir) {
  PopulateBipMaps();
  return pkb_->HasRelationship(GetPKBRelRef(dir));
}

bool NextBipExtractor::HasRelationship(RelDirection dir, int target) {
  PopulateBipMaps();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(target));
}

bool NextBipExtractor::HasRelationship(RelDirection dir, int first, int second) {
  PopulateBipMaps();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(first), std::to_string(second));
}

void NextBipExtractor::PopulateBipMaps() {
  if (pkb_->HasRelationship(PKBRelRefs::kNextBip)) return;
  next_bip_map_ = pkb_->ConvertStringToEntityMapping(pkb_->GetRelationshipMap(PKBRelRefs::kNext));
  prev_bip_map_ = pkb_->ConvertStringToEntityMapping(pkb_->GetRelationshipMap(PKBRelRefs::kPrevious));
  for (Entity* call_entity : call_list_) {
    if (auto* call_statement = dynamic_cast<Statement*>(call_entity)) {
      std::list<Entity*> next_entities = GetRelFromMap(call_entity, PKBRelRefs::kNextBip);
      Procedure* called_proc = dynamic_cast<CallEntity*>(call_statement)->GetProcedure();
      if (!next_entities.empty()) {
        EraseNextRelationship(call_entity);
        JoinEndToEnd(called_proc, next_entities);
      }
      JoinStartToStart(called_proc, call_entity);
    }
  }
  pkb_->PopulateRelationship<Entity, Entity>(next_bip_map_, PKBRelRefs::kNextBip);
  pkb_->PopulateRelationship<Entity, Entity>(prev_bip_map_, PKBRelRefs::kPrevBip);
}

std::list<Entity*> NextBipExtractor::GetRelFromMap(Entity* target, PKBRelRefs rel) {
  auto map = rel == PKBRelRefs::kNextBip ? next_bip_map_ : prev_bip_map_;
  if (map->count(target) == 0) {
    return std::list<Entity*>{};
  } else {
    return *map->find(target)->second;
  }
}

void NextBipExtractor::JoinStartToStart(Procedure* called_proc, Entity* prev_entity) {
  auto* proc_cluster = const_cast<Cluster*>(called_proc->GetClusterRoot());
  int proc_first_stmt = proc_cluster->GetStartEndRange().first;
  AddNext(prev_entity, stmt_list_[proc_first_stmt - 1]);
}

void NextBipExtractor::JoinEndToEnd(Procedure* called_proc, const std::list<Entity*> &next_entities) {
  for (Entity* next_entity : next_entities) {
    auto* root_block = const_cast<Block*>(called_proc->GetBlockRoot());
    std::list<int> last_stmts = GetBipLastStmts(root_block);
    for (int last_stmt : last_stmts) {
      AddNext(stmt_list_[last_stmt - 1], next_entity);
    }
  }
}

void NextBipExtractor::AddNext(Entity* prev_entity, Entity* next_entity) {
  AddRelationship(prev_entity, next_entity, next_bip_map_);
  AddRelationship(next_entity, prev_entity, prev_bip_map_);
}

void NextBipExtractor::AddRelationship(Entity* first_arg,
                                       Entity* second_arg,
                                       std::unordered_map<Entity*, std::list<Entity*>*>* map) {
  if (map->count(first_arg)) {
    std::list<Entity*>* values = map->find(first_arg)->second;
    if (std::find(values->begin(), values->end(), second_arg) == values->end()) {
      values->push_back(second_arg);
    }
  } else {
    auto* values = new std::list<Entity*>();
    values->push_back(second_arg);
    map->insert(std::make_pair(first_arg, values));
  }
}

void NextBipExtractor::EraseNextRelationship(Entity* entity) {
  std::list<Entity*> next_stmts = GetRelFromMap(entity, PKBRelRefs::kNextBip);
  if (!next_stmts.empty()) {
    next_bip_map_->erase(next_bip_map_->find(entity));
    for (Entity* next_stmt : next_stmts) {
      ErasePrevRelationship(next_stmt, entity);
    }
  }
}

void NextBipExtractor::ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt) {
  auto* entities_to_keep = new std::list<Entity*>{};
  for (Entity* old_prev_stmt: *prev_bip_map_->find(next_stmt)->second) {
    if (old_prev_stmt != prev_stmt) {
      entities_to_keep->push_back(old_prev_stmt);
    }
  }
  prev_bip_map_->erase(prev_bip_map_->find(next_stmt));
  prev_bip_map_->insert({next_stmt, entities_to_keep});
}

std::list<int> NextBipExtractor::GetBipLastStmts(Block* block) {
  std::list<int> last_stmts = block->GetCFGLastStmts();
  bool call_exists = true;
  while (call_exists) {
    for (int last_stmt : last_stmts) {
      Entity* last_entity = stmt_list_[last_stmt - 1];
      if (std::find(call_list_.begin(), call_list_.end(), last_entity) != call_list_.end()) {
        last_stmts = HandleCallLastStmt(last_stmts);
        goto found_call;
      }
    }
    call_exists = false;
    found_call:;
  }
  return last_stmts;
}

std::list<int> NextBipExtractor::HandleCallLastStmt(const std::list<int> &last_stmts) {
  std::list<int> nested_last_stmts;
  for (int last_stmt : last_stmts) {
    Entity* last_entity = stmt_list_[last_stmt - 1];
    if (std::find(call_list_.begin(), call_list_.end(), last_entity) != call_list_.end()) {
      auto* call_entity = dynamic_cast<CallEntity*>(last_entity);
      Procedure* called_proc = call_entity->GetProcedure();
      auto* root_block = const_cast<Block*>(called_proc->GetBlockRoot());
      std::list<int> called_last_stmts = root_block->GetCFGLastStmts();
      nested_last_stmts.insert(nested_last_stmts.end(), called_last_stmts.begin(), called_last_stmts.end());
    } else {
      nested_last_stmts.push_back(last_stmt);
    }
  }
  nested_last_stmts.sort();
  nested_last_stmts.unique();
  return nested_last_stmts;
}

PKBRelRefs NextBipExtractor::GetPKBRelRef(RelDirection dir) {
  return dir == RelDirection::kForward ? PKBRelRefs::kNextBip : PKBRelRefs::kPrevBip;
}
