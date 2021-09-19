#include <cassert>
#include "ParentTExtractor.h"

/**
 * Extracts Parent* recursively for each procedure.
 *
 * pseudocode:
 * for each proc in proc list
 *  for each stmt in stmt list
 *    recurse helper(stmt):
 *      Get children list
 *      base case: no children list, return empty list
 *      for each child
 *        recurse(child) and Get childrenT list
 *        add this child and its childrenT list to parentT
 *      return all childrenT of this stmt
 */
void ParentTExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  for (Procedure* proc: deliverable_->proc_list_) {
    for (Statement* stmt: * proc->GetStatementList()) {
      ExtractChildrenTFromParent(stmt);
    }
  }
}

/**
 * Extracts ChildrenT recursively from a parent Statement. Adds all intermediate ParentT relationship to the deliverable
 * and returns the ChildrenT list.
 *
 * @param parent Statement that may have children.
 * @return List of Statements that are the childrenT of parent.
 */
std::list<Statement*>* ParentTExtractor::ExtractChildrenTFromParent(Statement* parent) {
  std::unordered_map<Statement*, std::list<Statement*>*> ptc = deliverable_->parent_to_child_hash_;

  // base case: children list not found
  if (ptc.find(parent) == ptc.end()) {
    // does not return itself bcos this function is meant to return its childrenT
    return new std::list<Statement*>();
  }

  std::list<Statement*>* children = ptc.find(parent)->second;
  for (Statement* child: * children) {
    assert(child != parent); // erroneous non transitive relationship

    std::list<Statement*>* children_T_list = ExtractChildrenTFromParent(child);
    // add this child to the parent
    deliverable_->AddParentTransitiveRelationship(parent, child);
    // add this child's childrenT to the parent
    deliverable_->AddParentTransitiveRelationshipForList(parent, children_T_list);
  }

  std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable_->parent_to_child_T_hash_;
  assert(ptct.find(parent) != ptct.end());  // there must be some ParentT bcos there is a children list
  // return all childrenT
  return ptct.find(parent)->second;
}