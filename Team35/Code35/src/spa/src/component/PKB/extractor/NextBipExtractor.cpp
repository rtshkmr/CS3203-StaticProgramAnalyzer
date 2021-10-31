#include "NextBipExtractor.h"

#include <utility>
#include <cassert>

NextBipExtractor::NextBipExtractor(PKB* pkb) {
  pkb_ = pkb;
  //proc_list_ = pkb->GetDesignEntities(DesignEntity::kProcedure);
  stmt_list_ = pkb->GetDesignEntities(DesignEntity::kStmt);
  call_list_ = pkb->GetDesignEntities(DesignEntity::kCall);
}

/**
 * Gets all s from NextBip(target, s) by using relinking all the Next of Call Statements,
 * and recording in the hashmaps in this class.
 * @param target Statement number of the first argument of NextBip.
 * @return Vector of Entities that are NextBip of the target.
 */
std::vector<Entity*> NextBipExtractor::GetNextBip(std::string target) {
  return GetRelationship(NextBipRel::kNextBip, target);
}

std::vector<Entity*> NextBipExtractor::GetRelationship(NextBipRel rel, std::string target) {
  std::vector<Entity*> result = GetRelFromMap(target, rel);
  if (result.empty()) {
    PopulateBipMaps(pkb_->GetRelationshipMap(PKBRelRefs::kNext), pkb_->GetRelationshipMap(PKBRelRefs::kPrevious));
    return GetRelFromMap(target, rel);
  } else {
    return result;
  }
}

void NextBipExtractor::PopulateBipMaps(std::unordered_map<std::string, std::vector<Entity*>> next_map,
                                       std::unordered_map<std::string, std::vector<Entity*>> prev_map) {
  next_bip_map_ = std::move(next_map);
  prev_bip_map_ = std::move(prev_map);
  for (Entity* call_entity : call_list_) {
    if (auto* call_statement = dynamic_cast<Statement*>(call_entity)) {
      std::string call_stmt_int = std::to_string(GetStmtNum(call_entity));
      std::vector<Entity*> next_entities = GetRelFromMap(call_stmt_int, NextBipRel::kNextBip);
      Procedure* called_proc = dynamic_cast<CallEntity*>(call_statement)->GetProcedure();
      if (!next_entities.empty()) {
        EraseNextRelationship(call_entity);
        JoinEndToEnd(called_proc, next_entities);
      }
      JoinStartToStart(called_proc, call_entity);
    }
  }
}

void NextBipExtractor::JoinStartToStart(Procedure* called_proc, Entity* prev_entity) {
  auto* proc_cluster = const_cast<Cluster*>(called_proc->GetClusterRoot());
  int proc_first_stmt = proc_cluster->GetStartEndRange().first;
  AddNext(prev_entity, stmt_list_[proc_first_stmt - 1]);
}

void NextBipExtractor::JoinEndToEnd(Procedure* called_proc, const std::vector<Entity*> &next_entities) {
  for (Entity* next_entity : next_entities) {
    auto* root_block = const_cast<Block*>(called_proc->GetBlockRoot());
    std::list<int> last_stmts = GetLastStmts(root_block);
    for (int last_stmt : last_stmts) {
      AddNext(stmt_list_[last_stmt - 1], next_entity);
    }
  }
}

std::vector<Entity*> NextBipExtractor::GetRelFromMap(const std::string& target, NextBipRel rel) {
  auto map = rel == NextBipRel::kNextBip ? next_bip_map_ : prev_bip_map_;
  if (map.count(target) == 0) {
    return std::vector<Entity*>{};
  } else {
    return map.find(target)->second;
  }
}

void NextBipExtractor::AddNext(Entity* prev_entity, Entity* next_entity) {
  AddRelationship(prev_entity, next_entity, &next_bip_map_);
  AddRelationship(next_entity, prev_entity, &prev_bip_map_);
}

void NextBipExtractor::AddRelationship(Entity* first_arg,
                                       Entity* second_arg,
                                       std::unordered_map<std::string, std::vector<Entity*>>* map) {
  std::string first_stmt_num = std::to_string(GetStmtNum(first_arg));
  std::vector<Entity*> values;
  if (map->count(first_stmt_num)) {
    values = map->find(first_stmt_num)->second;
    if (std::find(values.begin(), values.end(), second_arg) == values.end()) {
      values.push_back(second_arg);
    }
    map->erase(first_stmt_num);
  } else {
    values = std::vector<Entity*>();
    values.push_back(second_arg);
  }
  map->insert(std::make_pair(first_stmt_num, values));
}

int NextBipExtractor::GetStmtNum(Entity* entity) {
  if (auto* statement = dynamic_cast<Statement*>(entity)) {
    StatementNumber* statement_number = statement->GetStatementNumber();
    return statement_number->GetNum();
  } else {
    assert(false);
  }
}

void NextBipExtractor::EraseNextRelationship(Entity* entity) {
  std::string stmt_num = std::to_string(GetStmtNum(entity));
  std::vector<Entity*> next_stmts = GetRelFromMap(stmt_num, NextBipRel::kNextBip);
  if (!next_stmts.empty()) {
    next_bip_map_.erase(next_bip_map_.find(stmt_num));
    for (Entity* next_stmt : next_stmts) {
      ErasePrevRelationship(next_stmt, entity);
    }
  }
}

void NextBipExtractor::ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt) {
  std::string next_stmt_num = std::to_string(GetStmtNum(next_stmt));
  auto entities_to_keep = std::vector<Entity*>{};
  for (Entity* old_prev_stmt: prev_bip_map_.find(next_stmt_num)->second) {
    if (old_prev_stmt != prev_stmt) {
      entities_to_keep.push_back(old_prev_stmt);
    }
  }
  prev_bip_map_.erase(prev_bip_map_.find(next_stmt_num));
  prev_bip_map_.insert({next_stmt_num, entities_to_keep});
}

std::list<int> NextBipExtractor::GetLastStmts(Block* block) {
  std::set<Block*> next_blocks = block->GetNextBlocks();
  std::list<int> last_stmts = std::list<int>{};
  for (Block* next_block: next_blocks) {
    int next = next_block->GetStartEndRange().first;
    if (block->isWhile && next == block->GetStartEndRange().first + 1) {
      continue;
    }
    if (next == -1) { // exit block
      assert(next_block->GetNextBlocks().empty());
      last_stmts.push_back(block->GetStartEndRange().second);
      continue;
    }
    std::list<int> next_last_stmts = GetLastStmts(next_block);
    last_stmts.insert(last_stmts.end(), next_last_stmts.begin(), next_last_stmts.end());
  }
  if (next_blocks.empty() || (next_blocks.size() == 1 && block->isWhile)) {
    last_stmts.push_back(block->GetStartEndRange().second);
  }
  last_stmts = HandleCallLastStmt(last_stmts);
  return last_stmts;
}

std::list<int> NextBipExtractor::HandleCallLastStmt(const std::list<int>& last_stmts) {
  std::list<int> nested_last_stmts;
  for (int last_stmt : last_stmts) {
    Entity* last_entity = stmt_list_[last_stmt - 1];
    if (std::find(call_list_.begin(), call_list_.end(), last_entity) != call_list_.end()) {
      auto* call_entity = dynamic_cast<CallEntity*>(last_entity);
      Procedure* called_proc = call_entity->GetProcedure();
      auto* root_block = const_cast<Block*>(called_proc->GetBlockRoot());
      std::list<int> called_last_stmts = GetLastStmts(root_block);
      nested_last_stmts.insert(nested_last_stmts.end(), called_last_stmts.begin(), called_last_stmts.end());
    } else {
      nested_last_stmts.push_back(last_stmt);
    }
  }
  nested_last_stmts.sort();
  nested_last_stmts.unique();
  return nested_last_stmts;
}

/**
 * Gets all s from NextBip(s, target).
 * @param target Target statement number.
 * @return Vector of entities satisfying the relationship.
 */
std::vector<Entity*> NextBipExtractor::GetPrevBip(std::string target) {
  return GetRelationship(NextBipRel::kPrevBip, target);
}

/**
 * Gets all rhs of NextBip(s, s)
 * @return Vector of entities satisfying the relationship.
 */
std::vector<Entity*> NextBipExtractor::GetAllNextBipRHS() {
  //PopulateBipMaps(pkb_->GetRelationship(PKBRelRefs::kNext), pkb_->GetRelationship(PKBRelRefs::kPrevious));
  return std::vector<Entity*>();
}

/**
 * Gets all lhs of NextBip(s, s)
 * @return Vector of entities satisfying the relationship.
 */
std::vector<Entity*> NextBipExtractor::GetAllNextBipLHS() {
  //PopulateBipMaps(pkb_->GetRelationship(PKBRelRefs::kNext), pkb_->GetRelationship(PKBRelRefs::kPrevious));
  return std::vector<Entity*>();
}
std::vector<std::tuple<Entity*, Entity*>> NextBipExtractor::GetAllNextBip() {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> NextBipExtractor::GetAllPrevBip() {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
bool NextBipExtractor::HasNextBip() {
  return false;
}
bool NextBipExtractor::HasPrevBip() {
  return false;
}
bool NextBipExtractor::HasNextBip(std::string target) {
  return false;
}
bool NextBipExtractor::HasPrevBip(std::string target) {
  return false;
}
bool NextBipExtractor::HasNextBip(std::string first, std::string second) {
  return false;
}
