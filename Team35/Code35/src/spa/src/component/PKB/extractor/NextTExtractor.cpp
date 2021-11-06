#include <cassert>
#include <utility>
#include <util/Utility.h>
#include "NextTExtractor.h"
#include "model/CFG.h"
#include "RuntimeExtractor.h"

NextTExtractor::NextTExtractor(PKB* pkb) {
  pkb_ = pkb;
  program_ = pkb->GetProgram();
  for (Entity* entity : pkb->GetDesignEntities(DesignEntity::kProcedure)) {
    proc_list_.push_back(dynamic_cast<Procedure*>(entity));
  }
  for (Entity* entity : pkb->GetDesignEntities(DesignEntity::kStmt)) {
    stmt_list_.push_back(dynamic_cast<Statement*>(entity));
  }
  int total = stmt_list_.size();
  next_t_2d_array_ = std::vector<std::vector<int>>(total, std::vector<int>(total));
}

NextTExtractor::NextTExtractor(std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list) {
  proc_list_ = std::move(proc_list);
  program_ = new Program(proc_list_[0]);
  for (int i = 1; i < proc_list_.size(); ++i) {
    program_->AddProcedure(proc_list_[i]);
  }
  stmt_list_ = std::move(stmt_list);
  int total = stmt_list_.size();
  next_t_2d_array_ = std::vector<std::vector<int>>(total, std::vector<int>(total));
}

void NextTExtractor::Delete() {
  proc_list_ = {};
  stmt_list_ = {};
  rel_table_ = {};
  first_arg_table_ = {};
  rel_by_types_table_ = {};
  next_t_2d_array_ = {};
}

/**
 * @return size of next_t_map
 */
int NextTExtractor::GetNextTSize() {
  return rel_table_[RelDirection::kForward].size();
}

int NextTExtractor::GetPrevTSize() {
  return rel_table_[RelDirection::kReverse].size();
}
/**
 * Extracts list of Next* of the target from the CFG. Caches Next* relationships for blocks traversed.
 *
 * @param target String of the statement number to look for.
 * @return List of Entities that are Next* of the target.
 *
 * pseudocode
 * get to the block of the target stmt
 * if any while loop, next* all statements within
 * else recurse into next blocks
 * return next* of recursed block
 * add list of next* to this block
 */
std::vector<Entity*> NextTExtractor::GetRelationship(RelDirection dir, int target) {
  rel_direction_ = dir;
  return GetRel(target, proc_list_, stmt_list_);
}

/**
 * Extracts list of Next* of the target from the CFG of the procedures provided in the proc_list.
 *
 * @param target String of the statement number to look for.
 * @param proc_list List of procedures to get the target from.
 * @return List of Entities that are Next* of the target.
 */
std::vector<Entity*> NextTExtractor::GetRelationship(RelDirection dir,
                                                     int target,
                                                     const std::vector<Procedure*> &proc_list) {
  rel_direction_ = dir;
  return GetRel(target, proc_list, stmt_list_);
}

/**
 * Gets all Entities that can be on the LHS/RHS of the relationship, i.e. Next*(_, s).
 * @return all Entities that can be on the LHS/RHS of the relationship.
 */
std::vector<Entity*> NextTExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  if (next_design_entities.count(de) == 0) return {};
  if (de == DesignEntity::kProgLine) {
    de = DesignEntity::kStmt;
  }
  rel_direction_ = dir;
  PopulateRelationshipMap(proc_list_);
  return first_arg_table_[rel_direction_][de];
}

std::vector<std::tuple<Entity*, Entity*>> NextTExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                 DesignEntity first,
                                                                                 DesignEntity second) {
  if (next_design_entities.count(first) == 0 || next_design_entities.count(second) == 0) return {};
  if (first == DesignEntity::kProgLine) {
    first = DesignEntity::kStmt;
  }
  if (second == DesignEntity::kProgLine) {
    second = DesignEntity::kStmt;
  }
  rel_direction_ = dir;
  if (got_all_pairs_) {
    return rel_by_types_table_[rel_direction_][{first, second}];
  }
  PopulateRelationshipMap(proc_list_);
  for (auto pair: rel_table_[rel_direction_]) {
    for (Statement* stmt: *pair.second) {
      AddRelByTypes(rel_direction_, pair.first, stmt);
    }
  }
  got_all_pairs_ = true;
  return rel_by_types_table_[rel_direction_][{first, second}];
}

bool NextTExtractor::HasRelationship(RelDirection dir) {
  return pkb_->HasRelationship(PKBRelRefs::kNext);
}

bool NextTExtractor::HasRelationship(RelDirection dir, int target) {
  if (dir == RelDirection::kForward) {
    return pkb_->HasRelationship(PKBRelRefs::kNext, std::to_string(target));
  } else {
    return pkb_->HasRelationship(PKBRelRefs::kPrevious, std::to_string(target));
  }
}

/**
 * Returns true if Next*(first, second).
 */
bool NextTExtractor::HasRelationship(RelDirection dir, int first, int second) {
  if (dir == RelDirection::kReverse) {
    rel_direction_ = RelDirection::kForward;
    int temp = first;
    first = second;
    second = temp;
  }
  int total_stmt = stmt_list_.size();
  if (first > total_stmt || first <= 0 || second > total_stmt || second <= 0) {
    return false;
  }
  if (next_t_2d_array_[first - 1][second - 1] == 1) {
    return true;
  }

  Cluster* proc_cluster = program_->GetProcClusterForLineNum(first);
  if (proc_cluster) {
    if (proc_cluster->CheckIfStmtNumInRange(second)) {
      Cluster* t_cluster = GetTargetCluster(proc_cluster, first);
      return HasNextTInFirstCluster(t_cluster, first, second);
    } else {  // first and second not in the same procedure
      return false;
    }
  }
  return false;
}

bool NextTExtractor::HasRelationship(RelDirection dir, DesignEntity first, DesignEntity second) {
  if (next_design_entities.count(first) == 0 || next_design_entities.count(second) == 0) return {};
  std::vector<Entity*> first_entities = pkb_->GetDesignEntities(first);
  std::vector<Entity*> second_entities = pkb_->GetDesignEntities(second);
  for (Entity* first_entity : first_entities) {
    int first_stmt_num = Utility::ConvertStringToInt(PKB::GetNameFromEntity(first_entity));
    for (Entity* second_entity : second_entities) {
      int second_stmt_num = Utility::ConvertStringToInt(PKB::GetNameFromEntity(second_entity));
      if (HasRelationship(dir, first_stmt_num, second_stmt_num)) {
        return true;
      }
    }
  }
  return false;
}

std::vector<Entity*> NextTExtractor::GetRel(int target,
                                            const std::vector<Procedure*> &proc_list,
                                            std::vector<Statement*> stmt_list) {
  bool is_out_of_bounds = target > stmt_list.size() || target <= 0;
  if (is_out_of_bounds) {
    return std::vector<Entity*>{};
  } else if (rel_table_[rel_direction_].count(stmt_list[target - 1]) == 1) {
    return Utility::ConvertListToVector(*rel_table_[rel_direction_].find(stmt_list[target - 1])->second);
  }

  Cluster* proc_cluster = program_->GetProcClusterForLineNum(target);
  if (proc_cluster) {
    Cluster* t_cluster = GetTargetCluster(proc_cluster, target);
    return Utility::ConvertListToVector(GetRelFromCluster(t_cluster, target));
  }
  return std::vector<Entity*>{};
}

/**
 * Go to innermost target block and start traversal. If any while loop is met, process from while.
 * Assumes that map entry is only updated once.
 */
std::list<Statement*> NextTExtractor::GetRelFromCluster(Cluster* cluster, int target) {
  if (rel_table_[rel_direction_].count(stmt_list_[target - 1]) == 1) {
    return *rel_table_[rel_direction_].find(stmt_list_[target - 1])->second;
  }

  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    GetRelByTraversal(dynamic_cast<Block*>(cluster), target);
  } else {
    Cluster* cluster_head = nested_clusters.front();
    auto* head_block = dynamic_cast<Block*>(cluster_head);
    if (head_block && head_block->isWhile) {
      GetRelFromWhile(cluster, target);
    } else {
      Cluster* t_cluster = GetTargetCluster(cluster, target);
      return GetRelFromCluster(t_cluster, target);
    }
  }
  return GetValueFromMap(rel_table_[rel_direction_], target);
}

/**
 * Start recursive traversal from the block following the while loop. Adds all up/downstream relationships to all stmts
 * in the while loop.
 *
 * @param w_cluster Cluster representing the while loop.
 * @param target Stmt num of stmt to get relationships for.
 * @return List of Statements that are Next* of the end of w_cluster.
 */
std::list<Statement*> NextTExtractor::GetRelFromWhile(Cluster* w_cluster, int target) {
  if (rel_table_[rel_direction_].count(stmt_list_[target - 1]) == 1) {
    return *rel_table_[rel_direction_].find(stmt_list_[target - 1])->second;
  }

  Block* w_block = dynamic_cast<Block*>(w_cluster->GetNestedClusters().front());
  std::list<Block*> following_blocks = GetFollowingBlocksAfterWhile(w_block);
  std::list<Statement*> rel_stmts;
  for (Block* following_block: following_blocks) {
    std::list<Statement*> traversed_stmts = GetRelByTraversal(following_block, target);
    rel_stmts.insert(rel_stmts.end(), traversed_stmts.begin(), traversed_stmts.end());
    rel_stmts = AddBlockStmtToRelList(rel_stmts, following_block);
  }

  std::pair<int, int> range = w_cluster->GetStartEndRange();
  if (following_blocks.size() >= 2) {
    rel_stmts = *MakeUniqueList(range.first, rel_stmts);
  }
  std::list<Statement*> w_statements = MakeStmtList(range.first, range.second);
  w_statements.insert(w_statements.end(), rel_stmts.begin(), rel_stmts.end());
  for (int i = range.first - 1; i < range.second; ++i) {
    AddRelationships(stmt_list_[i], w_statements);
  }
  // getting from the front or the back of w_cluster is the same
  return GetValueFromMap(rel_table_[rel_direction_], w_cluster->GetStartEndRange().first);
}

std::list<Statement*> NextTExtractor::MakeStmtList(int first_stmt, int last_stmt) {
  int count = last_stmt - first_stmt + 1;
  std::list<Statement*> w_statements(count);
  std::copy(&stmt_list_[first_stmt - 1], &stmt_list_[last_stmt - 1] + 1, w_statements.begin());
  return w_statements;
}

std::list<Block*> NextTExtractor::GetFollowingBlocksAfterWhile(Block* w_block) {
  std::list<Block*> following_blocks;
  int block_start = w_block->GetStartEndRange().first;
  for (Block* following_block: GetFollowingBlocks(w_block)) {
    int following_block_start = following_block->GetStartEndRange().first;
    bool is_prev_block = rel_direction_ == RelDirection::kReverse
        && (following_block_start < block_start);
    bool is_next_block = rel_direction_ == RelDirection::kForward
        && (following_block_start != block_start + 1);
    if (following_block_start != -1 && (is_next_block || is_prev_block)) {
      following_blocks.push_back(following_block);
    }
  }
  return following_blocks;
}

/**
 * Start recursive traversal
 *
 * @return List of Statements that Next* the Statement at the end of this block, or the target Statement.
 */
std::list<Statement*> NextTExtractor::GetRelByTraversal(Block* block, int target) {
  if (block->GetStartEndRange().first == -1) {
    return GetValueFromMap(rel_table_[rel_direction_], target);
  }
  if (rel_table_[rel_direction_].count(stmt_list_[target - 1]) == 1) {
    return *rel_table_[rel_direction_].find(stmt_list_[target - 1])->second;
  }
  if (block->isWhile) {
    return GetRelFromWhile(block->GetParentCluster(), target);
  }
  target = GetBlockTarget(block, target);
  std::list<Statement*> following_t = RecurseFollowingBlocks(block, target);

  AddRelationshipsFollowingBlock(following_t, block);
  AddRelationshipsInBlock(following_t, block, target);
  return GetValueFromMap(rel_table_[rel_direction_], target);
}

std::list<Statement*> NextTExtractor::RecurseFollowingBlocks(Block* block, int target) {
  std::list<Statement*> following_t;
  for (Block* following_block: GetFollowingBlocks(block)) {
    if (following_block->GetStartEndRange().first == -1) { // exit block
      assert(following_block->GetNextBlocks().empty());
      continue;
    }
    std::list<Statement*> traversed_stmts = GetRelByTraversal(following_block, target);
    following_t.insert(following_t.end(), traversed_stmts.begin(), traversed_stmts.end());
    if (following_t.empty() || !following_block->isWhile) {
      following_t = AddBlockStmtToRelList(following_t, following_block);
    }
  }
  return following_t;
}

int NextTExtractor::GetBlockTarget(Block* block, int target) {
  std::pair<int, int> range = block->GetStartEndRange();
  if (rel_direction_ == RelDirection::kForward) {
    return target < range.first ? range.first : target;
  } else {
    return target > range.second ? range.second : target;
  }
}

std::set<Block*> NextTExtractor::GetFollowingBlocks(Block* block) {
  return rel_direction_ == RelDirection::kForward ? block->GetNextBlocks() : block->GetPrevBlocks();
}

void NextTExtractor::AddRelationshipsFollowingBlock(const std::list<Statement*> &rel_stmts, Block* block) {
  int block_end;
  std::pair<int, int> range = block->GetStartEndRange();
  block_end = rel_direction_ == RelDirection::kForward ? range.second : range.first;
  if (GetFollowingBlocks(block).size() >= 2) { // if block because while was handled separately
    AddRelationshipsWithDup(stmt_list_[block_end - 1], rel_stmts);
  } else {
    AddRelationships(stmt_list_[block_end - 1], rel_stmts);
  }
}

void NextTExtractor::AddRelationshipsInBlock(std::list<Statement*> rel_stmts, Block* block, int target) {
  std::pair<int, int> range = block->GetStartEndRange();
  if (rel_direction_ == RelDirection::kForward) {
    for (int i = range.second - 1; i >= target; --i) {
      rel_stmts.push_back(stmt_list_[i]);
      AddRelationships(stmt_list_[i - 1], rel_stmts);
    }
  } else {
    for (int i = range.first - 1; i < target - 1; ++i) {
      rel_stmts.push_back(stmt_list_[i]);
      AddRelationships(stmt_list_[i + 1], rel_stmts);
    }
  }
}

std::list<Statement*> NextTExtractor::AddBlockStmtToRelList(std::list<Statement*> rel_list, Block* block) {
  if (rel_direction_ == RelDirection::kForward) {
    rel_list.push_back(stmt_list_[block->GetStartEndRange().first - 1]);
  } else {
    rel_list.push_back(stmt_list_[block->GetStartEndRange().second - 1]);
  }
  return rel_list;
}

/**
 * Assumes that there are no duplicates. Appending lists take O(1).
 * Assumes that next_t_map will only be updated once.
 */
void NextTExtractor::AddRelationships(Statement* first_arg, std::list<Statement*> second_args) {
  if (second_args.empty() || rel_table_[rel_direction_].count(first_arg) == 1) return;
  auto* list = new std::list<Statement*>();
  list->insert(list->begin(), second_args.begin(), second_args.end());
  rel_table_[rel_direction_].insert({first_arg, list});
  DesignEntity de = PKB::GetDesignEntityFromEntity(first_arg);
  first_arg_table_[rel_direction_][de].push_back(first_arg);
  first_arg_table_[rel_direction_][DesignEntity::kStmt].push_back(first_arg);
}

/**
 * Adds Next* while checking for duplicates. Uses an array for tracking duplicates, taking O(n) time.
 * Assumes that next_t_map will only be updated once.
 */
void NextTExtractor::AddRelationshipsWithDup(Statement* first_arg, const std::list<Statement*> &second_args) {
  if (second_args.empty() || rel_table_[rel_direction_].count(first_arg) == 1) return;
  int first_arg_num = first_arg->GetStatementNumber();
  auto* list = MakeUniqueList(first_arg_num, second_args);
  rel_table_[rel_direction_].insert({first_arg, list});
  DesignEntity de = PKB::GetDesignEntityFromEntity(first_arg);
  first_arg_table_[rel_direction_][de].push_back(first_arg);
  first_arg_table_[rel_direction_][DesignEntity::kStmt].push_back(first_arg);
}

/**
 * Traverses the cluster on surface level and returns the outermost cluster with the target number.
 */
Cluster* NextTExtractor::GetTargetCluster(Cluster* p_cluster, int target_num) {
  std::list<Cluster*> nested_clusters = p_cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return p_cluster;
  }

  auto cluster_iter = nested_clusters.begin();
  while (!(*cluster_iter)->CheckIfStmtNumInRange(target_num)) {
    std::advance(cluster_iter, 1);
  }
  return *cluster_iter;
}

std::list<Statement*> NextTExtractor::GetValueFromMap(std::unordered_map<Statement*, std::list<Statement*>*> map,
                                                      int stmt_num) {
  if (map.count(stmt_list_[stmt_num - 1]) == 0) {
    return std::list<Statement*>{};
  } else {
    return *map.find(stmt_list_[stmt_num - 1])->second;
  }
}

Block* NextTExtractor::GetNextBlockAfterWhile(Block* w_block) {
  Block* block_after_w = w_block;
  for (Block* next_block: w_block->GetNextBlocks()) {
    // not the immediate block after the w cond
    if (next_block->GetStartEndRange().first != w_block->GetStartEndRange().first + 1) {
      block_after_w = next_block;
    }
  }
  return block_after_w;
}

std::list<Statement*>* NextTExtractor::MakeUniqueList(int s1_num, const std::list<Statement*> &list) {
  auto new_list = new std::list<Statement*>();
  std::vector<int> duplicates(stmt_list_.size(), 0);
  for (Statement* s: list) {
    int s_num = s->GetStatementNumber();
    if (duplicates[s_num - 1] == 0) {
      duplicates[s_num - 1] = 1;
      next_t_2d_array_[s_num - 1][s1_num - 1] = 1;
      next_t_2d_array_[s1_num - 1][s_num - 1] = 1;
      new_list->push_back(s);
    }
  }
  return new_list;
}

/**
 * Checks if the outermost cluster is while, and the first and second is either both in the while loop or
 * the second is after first. Else find HasNextTInCluster.
 *
 * @param cluster cluster that contains first.
 * @param first statement number.
 * @param second statement number.
 * @return true if Next*(first, second).
 */
bool NextTExtractor::HasNextTInFirstCluster(Cluster* cluster, int first, int second) {
  assert(cluster->CheckIfStmtNumInRange(first));
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return IsNextTDownstream(first, second);
  } else {
    auto* first_block = dynamic_cast<Block*>(nested_clusters.front());
    if (first_block->isWhile) {
      if (first <= second || cluster->CheckIfStmtNumInRange(second)) {
        next_t_2d_array_[first - 1][second - 1] = 1;
        return true;
      } else {  // second is before first and not in the same while loop of the outermost cluster
        return false;
      }
    } else {  // first cluster not a while loop
      Cluster* inner_cluster = GetTargetCluster(cluster, first);
      return HasNextTInCluster(inner_cluster, first, second);
    }
  }
}

/**
 * Checks if second statement number is downstream of first statement number. Assumes that both are in the same path.
 */
bool NextTExtractor::IsNextTDownstream(int first, int second) {
  if (first < second) {
    next_t_2d_array_[first - 1][second - 1] = 1;
    return true;
  } else {
    return false;
  }
}

/**
  * Gets to the innermost cluster containing first to find if Next*(first, second).
  * cluster must contain first. first and second must be in the same procedure.
  * Must traverse to conclusively determine Next* as this cluster is nested.
  * Pseudocode:
  * first >= second:
  * check for while loop then traverse
  * first < second:
  * traverse through clusters
  */
bool NextTExtractor::HasNextTInCluster(Cluster* cluster, int first, int second) {
  assert(cluster->CheckIfStmtNumInRange(first));
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return HasNextTByTraversal(dynamic_cast<Block*>(cluster), first, second);
  } else {
    auto* first_block = dynamic_cast<Block*>(nested_clusters.front());
    if (first_block && first_block->isWhile) {
      if (cluster->CheckIfStmtNumInRange(second)) {
        next_t_2d_array_[first - 1][second - 1] = 1;
        return true;
      } else if (second < first) {
        return false;
      } else {  // second > first
        Block* block_after_w = GetNextBlockAfterWhile(first_block);
        return HasNextTByTraversal(block_after_w, first, second);
      }
    } else {
      Cluster* inner_cluster = GetTargetCluster(cluster, first);
      return HasNextTInCluster(inner_cluster, first, second);
    }
  }
}

/**
 * Traverse and check. first should be guaranteed to be in this cluster or a previous cluster.
 * Cluster traversal cannot work on a nested level because of else block being the next sibling of if body block.
 */
bool NextTExtractor::HasNextTByTraversal(Block* block, int first, int second) {
  if (block->CheckIfStmtNumInRange(second)) {
    return IsNextTDownstream(first, second);
  } else {
    bool result = false;
    if (!block->CheckIfStmtNumInRange(first)) {
      next_t_2d_array_[first - 1][block->GetStartEndRange().first - 1] = 1;
    }
    for (Block* next_block: block->GetNextBlocks()) {
      if (next_block->GetStartEndRange().first == -1) {
        continue;
      }
      if (next_t_2d_array_[first - 1][next_block->GetStartEndRange().first - 1] != 1) {
        result = result || HasNextTByTraversal(next_block, first, second);
      }
    }
    return result;
  }
}

/**
 * Only populates the relationship map in the rel_table_ for the current rel_direction_,
 * by getting all relationships for all procedures.
 * @param proc_list List of procedures to get relationships from.
 */
void NextTExtractor::PopulateRelationshipMap(const std::vector<Procedure*> &proc_list) {
  bool forward_and_populated = rel_direction_ == RelDirection::kForward && next_t_populated_;
  bool reverse_and_populated = rel_direction_ == RelDirection::kReverse && prev_t_populated_;
  if (forward_and_populated || reverse_and_populated) {
    return;
  }
  for (Procedure* proc: proc_list) {
    std::pair<int, int> range = const_cast<Cluster*>(proc->GetClusterRoot())->GetStartEndRange();
    if (rel_direction_ == RelDirection::kForward) {
      GetRelationship(rel_direction_, range.first, {proc});
    } else {
      for (Block* tail_block : proc->GetTailBlocks()) {
        GetRelationship(rel_direction_, tail_block->GetStartEndRange().second, {proc});
      }
    }
  }
  if (rel_direction_ == RelDirection::kForward) {
    next_t_populated_ = true;
  } else {
    prev_t_populated_ = true;
  }
}

void NextTExtractor::AddRelByTypes(RelDirection dir, Entity* first_arg, Entity* second_arg) {
  RelDirection other_direction = rel_direction_ == RelDirection::kForward
                                 ? RelDirection::kReverse
                                 : RelDirection::kForward;
  DesignEntity first_de = PKB::GetDesignEntityFromEntity(first_arg);
  DesignEntity second_de = PKB::GetDesignEntityFromEntity(second_arg);
  std::vector<type_combo> first_combis = {
      {first_de, second_de},
      {DesignEntity::kStmt, second_de},
      {first_de, DesignEntity::kStmt},
      {DesignEntity::kStmt, DesignEntity::kStmt}
  };
  for (const type_combo &types : first_combis) {
    rel_by_types_table_[rel_direction_][types].emplace_back(first_arg, second_arg);
  }
  std::vector<type_combo> second_combis = {
      {second_de, first_de},
      {DesignEntity::kStmt, first_de},
      {second_de, DesignEntity::kStmt},
      {DesignEntity::kStmt, DesignEntity::kStmt}
  };
  for (const type_combo &types : first_combis) {
    rel_by_types_table_[other_direction][types].emplace_back(second_arg, first_arg);
  }
}
