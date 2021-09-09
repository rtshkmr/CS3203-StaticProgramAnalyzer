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
  if (parent_hash_.count(p1)) {
    parent_hash_.find(p1)->second->push_back(p2);
  } else {
    std::list<Statement*>* lst = new list<Statement*>();
    lst->push_back(p2);
    parent_hash_.insert(make_pair(p1, lst));
  }
}

void Deliverable::AddUsesRelationship(Statement* u1, Variable* u2) {
  if (use_hash_.count(u1)) {
    use_hash_.find(u1)->second->push_back(u2);
  } else {
    std::list<Variable*>* lst = new list<Variable*>();
    lst->push_back(u2);
    use_hash_.insert(make_pair(u1, lst));
  }
}

void Deliverable::AddUsesRelationship(Container* u1, Variable* u2) {
  if (container_use_hash_.count(u1)) {
    container_use_hash_.find(u1)->second->push_back(u2);
  } else {
    std::list<Variable*>* lst = new list<Variable*>();
    lst->push_back(u2);
    container_use_hash_.insert(make_pair(u1, lst));
  }
}

void Deliverable::AddModifiesRelationship(Statement* m1, Variable* m2) {
  if (modifies_hash_.count(m1)) {
    modifies_hash_.find(m1)->second->push_back(m2);
  } else {
    std::list<Variable*>* lst = new list<Variable*>();
    lst->push_back(m2);
    modifies_hash_.insert(make_pair(m1, lst));
  }
}

void Deliverable::AddModifiesRelationship(Container* m1, Variable* m2) {
  if (container_modifies_hash_.count(m1)) {
    container_modifies_hash_.find(m1)->second->push_back(m2);
  } else {
    std::list<Variable*>* lst = new list<Variable*>();
    lst->push_back(m2);
    container_modifies_hash_.insert(make_pair(m1, lst));
  }
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
