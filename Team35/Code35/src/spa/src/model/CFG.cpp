/**
 * This file contains the implementation required for CFG.
 */

#include <cassert>
#include "CFG.h"

int Cluster::size() const {
  if (start_ == -1 && end_ == -1) return 0;
  return end_ - start_ + 1;
}

/**
 * Adds a new cluster to list of clusters within it.
 * @param new_nested_cluster
 */
void Cluster::AddChildClusterToBack(Cluster* new_nested_cluster) {
  this->nested_clusters_.push_back(new_nested_cluster);
  new_nested_cluster->SetParentCluster(this);
}

void Cluster::AddChildClusterToFront(Cluster* new_nested_cluster) {
  this->nested_clusters_.push_front(new_nested_cluster);
  new_nested_cluster->SetParentCluster(this);
}

void Cluster::AddStmt(StatementNumber statement_number) {
  if (start_ == end_ && start_ == -1) { //new block
    start_ = end_ = statement_number.GetNum();
  } else {
    //acts as a deque.
    if (statement_number.GetNum() == start_ - 1) {
      start_ -= 1;
    } else if (statement_number.GetNum() == end_ + 1) {
      end_ += 1;
    } else {
      throw std::invalid_argument("An input is only valid if statement numbers are continuous with existing ones");
    }
  }
}
void Cluster::RemoveStmt(StatementNumber statement_number) {
  if (start_ == end_ && end_ == statement_number.GetNum()) {
    start_ = end_ = -1;
  } else if (statement_number.GetNum() == end_) {
    end_--;
  } else if (statement_number.GetNum() == start_) {
    start_++;
  } else {
    throw std::invalid_argument("Expectation of continuous numbers");
  }
}

/**
 * Checks if a statement number is within the range of statements [start, end]
 * @param sn
 * @return true if statement number falls in the inclusive range of start_ and end_
 */
bool Cluster::CheckIfStatementInRange(StatementNumber sn) const {
  if (start_ == -1 && end_ == -1) return false;
  int num = sn.GetNum();
  return start_ <= num && num <= end_;
}

bool Cluster::CheckIfStmtNumInRange(int num) {
  return start_ <= num && num <= end_;
}

Cluster* Cluster::GetParentCluster() {
  return this->parent_cluster_;
}
void Cluster::SetParentCluster(Cluster* parent_cluster) {
  this->parent_cluster_ = parent_cluster;

}

/**
 * A cluster will be contained within another parent cluster (except if it's the outermost procedure cluster) as an element
 * of a list of nested_clusters. Hence, a sibling cluster refers to a cluster
 * adjacent to current cluster in the list.
 * @return
 */
Cluster* Cluster::GetNextSiblingCluster() {
  Cluster* parent_cluster = this->GetParentCluster();
  if (parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    auto itr = std::find(siblings.begin(), siblings.end(), this);
    if (itr != end(siblings)) { // i.e. this exists, i can find myself using my parent
      int next_sibling_idx = std::distance(siblings.begin(), itr) + 1;
      if (next_sibling_idx >= siblings.size()) {
        return nullptr;
      } else {
        std::advance(itr, 1);
        return * itr;
      }
    } else {
      assert(false);
    }
  } else {
    return nullptr;
  }
}

Cluster* Cluster::GetPrevSiblingCluster() {
  Cluster* parent_cluster = this->GetParentCluster();
  if (parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    std::list<Cluster*>::iterator itr = std::find(siblings.begin(), siblings.end(), this);
    if (itr != end(siblings)) { // i.e. this exists, i can find myself using my parent
      int prev_sibling_idx = std::distance(siblings.begin(), itr) - 1;
      if (prev_sibling_idx < 0) {
        return nullptr; // no previous sibling exists:
      } else {
        return * (--itr);
      }
    } else {
      assert(false);
    }
  } else {
    return nullptr;
  }
  return nullptr;
}

std::list<Cluster*> Cluster::GetNestedClusters() const {
  return this->nested_clusters_;
}

void Cluster::AddSiblingCluster(Cluster* new_sibling_cluster) {
  if (parent_cluster_ == nullptr) { // the outermost cluster can't have any siblings
    throw std::invalid_argument("The outermost cluster (representing a procedure) should never have any siblings");
  }
  parent_cluster_->AddChildClusterToBack(new_sibling_cluster);
}

/**
 * When a nested_cluster is added to a parent cluster, the parent cluster's range of values for statements, [start_, end_]
 * needs to be updated with logic similar to AddSmt to reflect the range expanded by the child.
 * @param nested_cluster
 */
void Cluster::UpdateRange(Cluster* nested_cluster) {
  int new_cluster_start = nested_cluster->start_;
  int new_cluster_end = nested_cluster->end_;
  bool is_valid_new_cluster_range = new_cluster_start <= new_cluster_end
      && new_cluster_start >= -1 && new_cluster_end >= -1;
  assert(is_valid_new_cluster_range);
  bool this_cluster_range_is_unassigned = this->start_ == end_ && this->start_ == -1;
  if (this_cluster_range_is_unassigned) {
    bool this_has_nested_clusters = !this->nested_clusters_.empty();
    if (this_has_nested_clusters) {
      int start_of_first_nested_cluster = this->nested_clusters_.front()->start_;
      int end_of_last_nested_cluster = this->nested_clusters_.back()->end_;
      this->start_ = start_of_first_nested_cluster;
      this->end_ = end_of_last_nested_cluster;
    } else { // set the range to be same as the incoming:
      this->start_ = new_cluster_start;
      this->end_ = new_cluster_end;
    }
  } else { // there are nested clusters within, assume the start and end range already updated
    bool new_cluster_appears_before_this = new_cluster_end + 1 == this->start_;
    bool new_cluster_appears_after_this = new_cluster_start == this->end_ + 1;
    if (new_cluster_appears_before_this) {
      this->start_ = new_cluster_start;
    } else if (new_cluster_appears_after_this) {
      this->end_ = new_cluster_end;
    } else {
      throw std::invalid_argument(
          "[UpdateClusterRange] An input is only valid if statement numbers are continuous with existing ones");
    }
  }
}
std::pair<int, int> Cluster::GetStartEndRange() const {
  return std::pair<int, int>(this->start_, this->end_);
}

void Cluster::UpdateClusterRange() {
  if (nested_clusters_.empty()) {
    return;
  }
  for (auto nested_cluster: this->nested_clusters_) {
    nested_cluster->UpdateClusterRange();
    bool nested_cluster_range_already_considered =
        this->start_ <= nested_cluster->start_ && this->end_ >= nested_cluster->end_;
    if (!nested_cluster_range_already_considered) {
      this->UpdateRange(nested_cluster);
    }
  }
}
bool Cluster::CheckIfStatementsInRange(int first_stmt, int second_stmt) const {
  if (first_stmt < second_stmt) {
    return first_stmt >= start_ && second_stmt <= end_;
  } else {
    return second_stmt >= start_ && first_stmt <= end_;
  }
}

ClusterTag Cluster::GetClusterTag() const {
  return this->cluster_tag_;
}
void Cluster::SetClusterTag(ClusterTag cluster_tag) {
  this->cluster_tag_ = cluster_tag;
}

/**
 * Traverses cluster from first to second statement number, using the given pkb to retrieve
 * data where necessary. Is recursive in nature, where we try to call on a small subproblem by
 * restricting the start statement number where possible. The goal is to find any valid unmodidied path between the
 * first and the second values in the target range (non - inclusive range).
 * @param pkb_rel_refs
 * @param scoped_cluster
 * @param target_range
 * @param pkb
 * @param lhs_var
 * @return
 */
bool Cluster::TraverseScopedCluster(PKBRelRefs rel_ref,
                                    Cluster* scoped_cluster,
                                    std::pair<int, int> target_range,
                                    PKB* pkb,
                                    const std::string& lhs_var) {
  switch (rel_ref) {
    case PKBRelRefs::kAffects: {
      return Cluster::CheckScopeClusterForAffects(scoped_cluster, target_range, pkb, lhs_var);
    };
    default: {
      return false;
    };

  }
  return false;
}

// todo: switching to case swithc makes it easier
//bool TraverseIfClusterForAffects(Cluster* if_cluster,
//                                 std::pair<int,int> target_range,
//                                 PKB* pkb,
//                                 const std::string& lhs_var) {
//  // need to consider different branches:
//  Cluster* if_body = child->GetClusterConstituent(ClusterTag::kIfBody);
//  Cluster* else_body = child->GetClusterConstituent(ClusterTag::kElseBody);
//  auto if_body_range = if_body->GetStartEndRange();
//  auto else_body_range = else_body->GetStartEndRange();
//  bool is_target_in_if_cluster = child->CheckIfStmtNumInRange(target_range.second);
//  if (is_target_in_if_cluster) {
//    // case 1: child contains second statement, identify if it's if body or else body
//    bool is_target_in_if_body = if_body->CheckIfStmtNumInRange(target_range.second);
//    bool is_target_in_else_body = else_body->CheckIfStmtNumInRange(target_range.second);
//    assert(is_target_in_else_body ^ is_target_in_if_body);
//    int new_start = is_target_in_if_body ? if_body_range.first : else_body_range.first;
//    auto new_target_range = std::make_pair(new_start, target_range.second);
//    return TraverseScopedClusterForAffects((is_target_in_if_body
//                                            ? if_body
//                                            : else_body),
//                                           new_target_range,
//                                           pkb,
//                                           lhs_var);
//  } else {
//    // case 2: child does not contain second statement, then just have to at least one that gives unmod path
//    bool if_body_is_unmodified_path = TraverseScopedClusterForAffects(if_body, if_body_range, pkb, lhs_var);
//    if (if_body_is_unmodified_path) continue;
//    bool else_body_is_unmodified_path = TraverseScopedClusterForAffects(else_body, else_body_range, pkb, lhs_var);
//    bool has_no_unmod_path = !if_body_is_unmodified_path && !else_body_is_unmodified_path;
//    if(has_no_unmod_path) {
//      scoped_cluster_does_not_modify_var = false;
//      break; // breaks outer for loop
//    }
//  }
//
//}

/**
 * Treats the child as a normal block and traverses it to check if the lhs_var has been modified.
 * @param child
 * @param target_range
 * @param pkb
 * @param lhs_var
 * @return
 */
std::pair<bool, bool> TraverseNormalBlockForAffects(Cluster* child,
                                                    std::pair<int, int> target_range,
                                                    PKB* pkb,
                                                    const std::string& lhs_var) {
  // =================================== HANDLE CHILDREN THAT ARE SIMPLE BLOCKS =======================
//  assert(child->GetClusterTag() == ClusterTag::kNormalBlock);
  bool child_does_not_modify_var = true;
  bool is_second_stmt_checked = false;
  auto child_range = child->GetStartEndRange();
  for (int stmt_num = child_range.first;
       stmt_num <= std::min(child_range.second, target_range.second);
       stmt_num++) { // bring this inside
    if (stmt_num <= target_range.first) continue; // skip what you can
    child_does_not_modify_var = !pkb->HasRelationship(PKBRelRefs::kModifies, std::to_string(stmt_num), lhs_var);
    is_second_stmt_checked = (stmt_num == target_range.second);
    if (!child_does_not_modify_var) break; // break this for loop for normal block if child actually modifies var
  }
  return std::make_pair(child_does_not_modify_var, is_second_stmt_checked);
//      ========================  END OF TRAVERSING NORMAL BLOCK FOR AFFECTS =============
}

bool Cluster::CheckScopeClusterForAffects(Cluster* scoped_cluster,
                                          std::pair<int, int> target_range,
                                          PKB* pkb,
                                          const std::string& lhs_var) {
  if (target_range.first >= target_range.second) {
    Cluster* current_scope = scoped_cluster;
    while (current_scope
        && current_scope->start_ <= target_range.second
        && current_scope->GetClusterTag() != ClusterTag::kWhileCluster) {
      current_scope = current_scope->GetParentCluster();
    }
    if (!current_scope) return false;
    auto current_scope_range = current_scope->GetStartEndRange();
    bool first_stmt_to_while_end_is_unmodified = (target_range.second == current_scope_range.second)
        || TraverseScopedClusterForAffects(current_scope,
                                           {target_range.first, current_scope_range.second},
                                           pkb,
                                           lhs_var);
    bool while_start_to_second_stmt_is_unmodified =
        TraverseScopedClusterForAffects(current_scope, {current_scope->start_, target_range.second}, pkb, lhs_var);
    return while_start_to_first_stmt_is_unmodified && second_stmt_to_while_end_is_unmodified;
  } else {
    TraverseScopedClusterForAffects(scoped_cluster, target_range, pkb, lhs_var);
  }
}

/**
 * Traverses a scoped cluster moving from first_stmt towards second_stmt in the target range and checks if the lhs_var remains unmodified along the way
 * @param scoped_cluster a scoped cluster is guaranteed to have the target second statement inside it.
 * @param target_range the first item in pair is just a starting point and the second item in pair is always the second statement that we work towards. Note that we are always checking b/w the target range i.e [first+1, second - 1] to see if they form up a valid unmod path. If second is inside this child, then we assert that second does modified the var
 * @param pkb
 * @param lhs_var
 * @return
 */
bool Cluster::TraverseScopedClusterForAffects(Cluster* scoped_cluster,
                                              std::pair<int, int> target_range,
                                              PKB* pkb,
                                              const std::string& lhs_var) {
  assert(scoped_cluster->CheckIfStmtNumInRange(target_range.second));
  bool scoped_cluster_does_not_modify_var = true;
  std::list<Cluster*> children = scoped_cluster->nested_clusters_;
  for (auto child: children) {
    auto child_range = child->GetStartEndRange();
    ClusterTag tag = child->GetClusterTag();
    bool child_is_cond_block = tag == ClusterTag::kIfCond || tag == ClusterTag::kWhileCond;
    bool child_does_not_contain_first_stmt = child_range.second < target_range.first;
    if (child_does_not_contain_first_stmt || child_is_cond_block) continue; // skips this child

    // =========================== HANDLE IF CLUSTER ====================================
    if (tag == ClusterTag::kIfCluster) {
      // need to consider different branches:
      Cluster* if_body = child->GetClusterConstituent(ClusterTag::kIfBody);
      Cluster* else_body = child->GetClusterConstituent(ClusterTag::kElseBody);
      auto if_body_range = if_body->GetStartEndRange();
      auto else_body_range = else_body->GetStartEndRange();
      bool is_target_in_if_cluster = child->CheckIfStmtNumInRange(target_range.second);
      if (is_target_in_if_cluster) {
        // case 1: child contains second statement, identify if it's if body or else body
        bool is_target_in_if_body = if_body->CheckIfStmtNumInRange(target_range.second);
        bool is_target_in_else_body = else_body->CheckIfStmtNumInRange(target_range.second);
        assert(is_target_in_else_body ^ is_target_in_if_body);
        int new_start = is_target_in_if_body ? if_body_range.first : else_body_range.first;
        auto new_target_range = std::make_pair(new_start, target_range.second);
        return TraverseScopedClusterForAffects((is_target_in_if_body
                                                ? if_body
                                                : else_body),
                                               new_target_range,
                                               pkb,
                                               lhs_var);
      } else {
        // case 2: child does not contain second statement, then just have to at least one that gives unmod path
        bool if_body_is_unmodified_path = TraverseScopedClusterForAffects(if_body, if_body_range, pkb, lhs_var);
        if (if_body_is_unmodified_path) continue;
        bool else_body_is_unmodified_path = TraverseScopedClusterForAffects(else_body, else_body_range, pkb, lhs_var);
        bool has_no_unmod_path = !if_body_is_unmodified_path && !else_body_is_unmodified_path;
        if (has_no_unmod_path) {
          scoped_cluster_does_not_modify_var = false;
          break; // breaks outer for loop
        }
      }
    } else if (tag == ClusterTag::kWhileCluster) {
      //=================================== HANDLE WHILE CLUSTER =====================================
      bool is_target_in_while_cluster = child->CheckIfStmtNumInRange(target_range.second);
      if (is_target_in_while_cluster) {
        // the first thing in the while cluster will be the cond, so it's okay to say that it's been checked:
        auto new_target_range = std::make_pair(child_range.first - 1, target_range.second);
        return TraverseScopedClusterForAffects(child, new_target_range, pkb, lhs_var);
      }
    } else if (tag == ClusterTag::kWhileBody) {
      assert(child->CheckIfStmtNumInRange(target_range.second)); // will only enter a while body if the second statement is inside it, else will just skip
      // iterate into the while body:
      target_range.first = child_range.first - 1;
      return TraverseScopedClusterForAffects(child, target_range, pkb, lhs_var);
    } else if (tag == ClusterTag::kIfBody || tag == ClusterTag::kElseBody) {
      bool child_is_normal_block = child->nested_clusters_.empty();
      if (child_is_normal_block) {
        auto traversal_results = TraverseNormalBlockForAffects(child, target_range, pkb, lhs_var);
        if (!traversal_results.first) { // i.e. child modifies variable:
          scoped_cluster_does_not_modify_var =
              traversal_results.second; // check if it was target second stmt that modded it.
          break;
        } else {
          // update target start to equal the value that I've already checked until
          target_range.first = std::min(child_range.second, target_range.second);
        }
      } else {
        // todo: test this case: have to call in if{while{...}}else{..}
        target_range.first = child_range.first - 1;
        return TraverseScopedClusterForAffects(child, target_range, pkb, lhs_var);
      }
    } else { // it's a simple block, no alternative paths to consider:
      assert(child->GetClusterTag() == ClusterTag::kNormalBlock);
      // first: child does not modify? second: is second stmt checked:
      auto traversal_results = TraverseNormalBlockForAffects(child, target_range, pkb, lhs_var);
      if (!traversal_results.first) { // i.e. child modifies variable:
        scoped_cluster_does_not_modify_var =
            traversal_results.second; // check if it was target second stmt that modded it.
        break;
      } else {
        target_range.first = std::min(child_range.second, target_range.second);
      }
    }
  }
  return scoped_cluster_does_not_modify_var;
}

/**
 * Retrieves the correct tagged constituent from within a container cluster (if or while cluster)
 * @return
 */
Cluster* Cluster::GetClusterConstituent(ClusterTag constituent_tag) {
  auto constituents = this->GetNestedClusters();
  Cluster* first_constituent = constituents.front();
  switch (constituent_tag) {
    case ClusterTag::kIfCond: {
      assert(cluster_tag_ == ClusterTag::kIfCluster);
      assert(first_constituent->GetClusterTag() == ClusterTag::kIfCond);
      return first_constituent;
    };
    case ClusterTag::kIfBody: {
      assert(cluster_tag_ == ClusterTag::kIfCluster);
      assert(first_constituent->GetClusterTag() == ClusterTag::kIfCond);
      return first_constituent->FindNextSibling(ClusterTag::kIfBody);
    };
    case ClusterTag::kElseBody: {
      assert(cluster_tag_ == ClusterTag::kIfCluster);
      assert(first_constituent->GetClusterTag() == ClusterTag::kIfCond);
      return first_constituent->FindNextSibling(ClusterTag::kElseBody);
    };
    case ClusterTag::kWhileCond: {
      assert(cluster_tag_ == ClusterTag::kWhileCluster);
      assert(first_constituent->GetClusterTag() == ClusterTag::kWhileCond);
      return first_constituent;
    };
    case ClusterTag::kWhileBody: {
      assert(cluster_tag_ == ClusterTag::kWhileCluster);
      assert(first_constituent->GetClusterTag() == ClusterTag::kWhileCond);
      return first_constituent->FindNextSibling(ClusterTag::kWhileBody);
    };
    default: {
      assert(false);
    }
  }
}

/**
 * Retrieves the next sibling that matches the target cluster tag
 * @param target_tag
 * @return
 */
Cluster* Cluster::FindNextSibling(ClusterTag target_tag) {
  Cluster* next_sibling = this->GetNextSiblingCluster();
  while (next_sibling) {
    const ClusterTag next_sibling_tag = next_sibling->GetClusterTag();
    if (next_sibling_tag == target_tag) {
      return next_sibling;
    } else {
      next_sibling = next_sibling->GetNextSiblingCluster();
    }
  }
  return nullptr;
}

// default destructors:
Cluster::~Cluster() = default;
Block::~Block() = default;
ConditionalBlock::~ConditionalBlock() = default;
BodyBlock::~BodyBlock() = default;
/**
 * Returns unique exit blocks for every call.
 * @return
 */
Block* Block::GetNewExitBlock() {
  Block* exit_block = new Block();
  exit_block->start_ = -1;
  exit_block->end_ = -1;
  return exit_block;
}

bool Block::IsExitBlock(Block* block) {
  std::pair<int, int> range = block->GetStartEndRange();
  return std::get<0>(range) == -1 && std::get<1>(range) == -1;
}

void Block::PatchEmptyBlocks(Block* redundant, Block* to) {
  if (!Block::IsExitBlock(redundant)) {
    throw std::invalid_argument("Redundant block is non empty");
  }

  for (auto* block: redundant->prev_blocks_) {
    block->next_blocks_.erase(redundant);
    block->AddNextBlock(to);
  }
}

void Block::AddNextBlock(Block* next_block) {
  this->next_blocks_.insert(next_block);
  next_block->prev_blocks_.insert(this);
  assert(this->next_blocks_.size() <= 2);
}

std::set<Block*> Block::GetNextBlocks() const {
  return this->next_blocks_;
}

std::set<Block*> Block::GetPrevBlocks() const {
  return this->prev_blocks_;
}

std::list<int> Block::GetCFGLastStmts() {
  std::list<int> last_stmts = std::list<int>{};
  for (Block* next_block: next_blocks_) {
    int next = next_block->GetStartEndRange().first;
    if (isWhile && next == start_ + 1) {
      continue;
    }
    if (next == -1) { // exit block
      assert(next_block->GetNextBlocks().empty());
      last_stmts.push_back(end_);
      continue;
    }
    std::list<int> next_last_stmts = next_block->GetCFGLastStmts();
    last_stmts.insert(last_stmts.end(), next_last_stmts.begin(), next_last_stmts.end());
  }
  if (next_blocks_.empty() || (next_blocks_.size() == 1 && isWhile)) {
    last_stmts.push_back(end_);
  }
  return last_stmts;
}
