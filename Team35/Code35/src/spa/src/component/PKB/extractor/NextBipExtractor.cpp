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
std::vector<Entity*> NextBipExtractor::GetNextBip(int target) {
  return GetRel(NextBipRel::kNextBip, target);
}

std::vector<Entity*> NextBipExtractor::GetRel(NextBipRel rel, int target) {
  std::vector<Entity*> result = GetRelFromMap(target, rel);
  if (result.empty()) {
    //PopulateBipMaps(pkb_->GetRelationship(PKBRelRefs::kNext), pkb_->GetRelationship(PKBRelRefs::kPrevious));
    return GetRelFromMap(target, rel);
  } else {
    return result;
  }
}

void NextBipExtractor::PopulateBipMaps(std::unordered_map<int, std::vector<Entity*>*> next_map,
                                       std::unordered_map<int, std::vector<Entity*>*> prev_map) {
  next_bip_map_ = std::move(next_map);
  prev_bip_map_ = std::move(prev_map);
  for (Entity* call_entity : call_list_) {
    if (auto* call_statement = dynamic_cast<Statement*>(call_entity)) {
      int call_stmt_int = GetStmtNum(call_entity);
      std::vector<Entity*> next_entities = GetRelFromMap(call_stmt_int, NextBipRel::kPrevBip);
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
  AddNext(prev_entity, stmt_list_[proc_first_stmt]);
}

void NextBipExtractor::JoinEndToEnd(Procedure* called_proc, const std::vector<Entity*> &next_entities) {
  for (Entity* next_entity : next_entities) {
    auto* root_block = const_cast<Block*>(called_proc->GetBlockRoot());
    std::list<int> last_stmts = GetLastStmts(root_block);
    for (int last_stmt : last_stmts) {
      AddNext(stmt_list_[last_stmt], next_entity);
    }
  }
}

std::vector<Entity*> NextBipExtractor::GetRelFromMap(int target, NextBipRel rel) {
  auto map = rel == NextBipRel::kNextBip ? next_bip_map_ : prev_bip_map_;
  if (map.count(target) == 0) {
    return std::vector<Entity*>{};
  } else {
    return *map.find(target)->second;
  }
}

void NextBipExtractor::AddNext(Entity* prev_entity, Entity* next_entity) {
  AddRelationship(prev_entity, next_entity, next_bip_map_);
  AddRelationship(next_entity, prev_entity, prev_bip_map_);
}

void NextBipExtractor::AddRelationship(Entity* first_arg,
                                       Entity* second_arg,
                                       std::unordered_map<int, std::vector<Entity*>*> map) {
  int first_stmt_num = GetStmtNum(first_arg);
  if (map.count(first_stmt_num)) {
    std::vector<Entity*>* values = map.find(first_stmt_num)->second;
    if (std::find(values->begin(), values->end(), second_arg) == values->end()) {
      values->push_back(second_arg);
    }
  } else {
    auto* vector = new std::vector<Entity*>();
    vector->push_back(second_arg);
    map.insert(std::make_pair(first_stmt_num, vector));
  }
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
  int stmt_num = GetStmtNum(entity);
  std::vector<Entity*> next_stmts = GetRelFromMap(stmt_num, NextBipRel::kPrevBip);
  if (!next_stmts.empty()) {
    next_bip_map_.erase(next_bip_map_.find(stmt_num));
    for (Entity* next_stmt : next_stmts) {
      ErasePrevRelationship(next_stmt, entity);
    }
  }
}

void NextBipExtractor::ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt) {
  int next_stmt_num = GetStmtNum(next_stmt);
  auto* entities_to_keep = new std::vector<Entity*>{};
  for (Entity* old_prev_stmt: *prev_bip_map_.find(next_stmt_num)->second) {
    if (old_prev_stmt != prev_stmt) {
      entities_to_keep->push_back(old_prev_stmt);
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
    if (next == -1) { // exit block
      assert(next_block->GetNextBlocks().empty());
      last_stmts.push_back(block->GetStartEndRange().second);
      continue;
    }
    std::list<int> next_last_stmts = GetLastStmts(next_block);
    last_stmts.insert(last_stmts.end(), next_last_stmts.begin(), next_last_stmts.end());
  }
  if (next_blocks.empty()) {
    last_stmts.push_back(block->GetStartEndRange().second);
  }
  return last_stmts;
}

/**
 * Gets all s from NextBip(s, target).
 * @param target Target statement number.
 * @return Vector of entities satisfying the relationship.
 */
std::vector<Entity*> NextBipExtractor::GetPrevBip(int target) {
  return GetRel(NextBipRel::kPrevBip, target);
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
