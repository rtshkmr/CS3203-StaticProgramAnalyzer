#include "Deliverable.h"

void Deliverable::AddStatement(Statement* stmt) {
  stmt_list_.push_back(stmt);
}

void Deliverable::AddIfEntity(IfEntity* if_entity) {
  if_list_.push_back(if_entity);
}

void Deliverable::AddWhileEntity(WhileEntity* while_entity) {
  while_list_.push_back(while_entity);
}

void Deliverable::AddAssignEntity(AssignEntity* assign_entity) {
  assign_list_.push_back(assign_entity);
}

void Deliverable::AddCallEntity(CallEntity* call_entity) {
  call_list_.push_back(call_entity);
}

void Deliverable::AddPrintEntity(PrintEntity* print_entity) {
  print_list_.push_back(print_entity);
}

void Deliverable::AddReadEntity(ReadEntity* read_entity) {
  read_list_.push_back(read_entity);
}

void Deliverable::AddFollowRelationship(Statement* f1, Statement* f2) {
  follow_hash_.insert({f1, f2});
}

void Deliverable::AddParentRelationship(Statement* p1, Statement* p2) {
  parent_hash_.insert({p1, p2});
}

std::list<Procedure*>* Deliverable::GetProcList() {
  return &proc_list_;
}

std::list<Variable*>* Deliverable::GetVariableList() {
  return &var_list_;
}
std::list<ConstantValue*>* Deliverable::GetConstantValue() {
  return &const_list_;
}
