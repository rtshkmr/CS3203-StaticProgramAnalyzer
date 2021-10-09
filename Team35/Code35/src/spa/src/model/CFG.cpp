/**
 * This file contains the implementation required for CFG.
 */

#include "CFG.h"

int Cluster::size() const {
  if (start_ == -1 && end_ == -1) return 0;
  return end_ - start_ + 1;
}

void ConditionalBlock::AddControlVariable(VariableName* control_variable) {
  this->control_variables_.insert(control_variable);
}

/**
 * Adds a new cluster to list of clusters within it.
 * @param new_cluster
 */
void Cluster::AddChildCluster(Cluster* new_cluster) {
  this->nested_clusters_.push_back(new_cluster);
}

bool Cluster::IsVariableModified(VariableName target_variable) {
  return false;
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
  // get the parent cluster if it exists
  Cluster* sibling = nullptr;
  Cluster* parent_cluster = this->GetParentCluster();
  if(parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    std::list<Cluster*>::iterator itr = std::find(siblings.begin(), siblings.end(),this);
    if(itr!=end(siblings)){ // i.e. this exists, i can find myself using my parent
      int next_sibling_idx = std::distance(siblings.begin(), itr) + 1; // todo: check if 0 or 1 idx
      if(next_sibling_idx >= siblings.size()) {
        // there is no sibling to myself, this is not an error.
        return nullptr;
      } else {
        std::advance(itr, 1);
        return *itr;
      }
    } else {
      // todo: throw exception for this, not possible to have asituation where I'm looking for my next sibling but I can't find myself in my parent's list
      return nullptr;
    }
  } else {
    // todo: not sure if i need to throw an exception here
    return nullptr;
  }
//  Cluster* next


  // if doesn't exist, it means we are in outermost cluster, todo: not sure if do nothing or throw exception

  return nullptr;
}
std::list<Cluster*> Cluster::GetNestedClusters() {
  return this->nested_clusters_;
}
