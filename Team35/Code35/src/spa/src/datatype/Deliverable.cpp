#include "Deliverable.h"

void Deliverable::AddProcedure(Procedure* proc) {
  proc_list_.push_back(proc);
}

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
  if (follow_hash_.find(f1) == follow_hash_.end()) {
    AddRelationshipToMap(&follow_hash_, f1, f2);
    AddRelationshipToMap(&followed_by_hash_, f2, f1);
  }
}

void Deliverable::AddFollowsTransitiveRelationship(Statement* before, Statement* after) {
  AddRelationshipToMap(&follows_T_hash_, before, after);
  AddRelationshipToMap(&followed_by_T_hash_, after, before);
}

void Deliverable::AddFollowsTransitiveRelationshipForList(Statement* before, std::list<Statement*>* afters) {
  for (Statement* after: * afters) {
    AddFollowsTransitiveRelationship(before, after);
  }
}

void Deliverable::AddParentRelationship(Statement* parent, Statement* child) {
  AddRelationshipToMap(&parent_to_child_hash_, parent, child);
  AddRelationshipToMap(&child_to_parent_hash_, child, parent);
}

void Deliverable::AddParentTransitiveRelationship(Statement* parent, Statement* child) {
  AddRelationshipToMap(&parent_to_child_T_hash_, parent, child);
  AddRelationshipToMap(&child_to_parent_T_hash_, child, parent);
}

void Deliverable::AddParentTransitiveRelationshipForList(Statement* parent, std::list<Statement*>* children) {
  for (Statement* child: * children) {
    AddParentTransitiveRelationship(parent, child);
  }
}

void Deliverable::AddUsesRelationship(Statement* u1, Variable* u2) {
  AddRelationshipToMap(&use_hash_, u1, u2);
  AddRelationshipToMap(&used_by_hash_, u2, u1);
}

void Deliverable::AddUsesRelationship(Container* u1, Variable* u2) {
  AddRelationshipToMap(&container_use_hash_, u1, u2);
  AddRelationshipToMap(&container_used_by_hash_, u2, u1);
}

void Deliverable::AddUsesRelationship(Container* container, std::list<Variable*>* var_list) {
  for (Variable* var: * var_list) {
    AddUsesRelationship(container, var);
  }
}

void Deliverable::AddModifiesRelationship(Statement* m1, Variable* m2) {
  AddRelationshipToMap(&modifies_hash_, m1, m2);
  AddRelationshipToMap(&modified_by_hash_, m2, m1);
}

void Deliverable::AddModifiesRelationship(Container* m1, Variable* m2) {
  AddRelationshipToMap(&container_modifies_hash_, m1, m2);
  AddRelationshipToMap(&container_modified_by_hash_, m2, m1);
}

void Deliverable::AddModifiesRelationship(Container* container, std::list<Variable*>* var_list) {
  for (Variable* var: * var_list) {
    AddModifiesRelationship(container, var);
  }
}

void Deliverable::AddCallsRelationship(Procedure* p1, Procedure* p2) {
  AddRelationshipToMap(&calls_hash_, p1, p2);
  AddRelationshipToMap(&called_by_hash_, p2, p1);
}

void Deliverable::AddCallsTransitiveRelationship(Procedure* p1, Procedure* p2) {
  AddRelationshipToMap(&calls_T_hash_, p1, p2);
  AddRelationshipToMap(&called_by_T_hash_, p2, p1);
}

void Deliverable::AddCallsTransitiveRelationshipForList(Procedure* p1, std::list<Procedure*>* proc_list) {
  for (Procedure* proc: *proc_list) {
    AddCallsTransitiveRelationship(p1, proc);
  }
}

void Deliverable::AddNextRelationship(Statement* s1, Statement* s2) {
  AddRelationshipToMap(&next_hash_, s1, s2);
  AddRelationshipToMap(&previous_hash_, s2, s1);
}

void Deliverable::SetProgram(Program* program) {
  this->program_ = program;
}

Program* Deliverable::GetProgram() {
  return program_;
}

std::list<Procedure*>* Deliverable::GetProcList() {
  return & proc_list_;
}

std::list<Variable*>* Deliverable::GetVariableList() {
  return & var_list_;
}
std::list<Constant*>* Deliverable::GetConstantList() {
  return & const_list_;
}

std::vector<Statement*>* Deliverable::GetStatementList() {
  return & stmt_list_;
}

std::list<IfEntity*>* Deliverable::GetIfList() {
  return & if_list_;
}

std::list<WhileEntity*>* Deliverable::GetWhileList() {
  return & while_list_;
}

std::list<AssignEntity*>* Deliverable::GetAssignList() {
  return & assign_list_;
}

std::list<CallEntity*>* Deliverable::GetCallList() {
  return & call_list_;
}

std::list<PrintEntity*>* Deliverable::GetPrintList() {
  return & print_list_;
}

std::list<ReadEntity*>* Deliverable::GetReadList() {
  return & read_list_;
}

std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetFollowsMap() {
  return &follow_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetFollowsTMap() {
  return &follows_T_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetParentMap() {
  return &parent_to_child_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetParentTMap() {
  return &parent_to_child_T_hash_;
}
std::unordered_map<Statement*, std::list<Variable*>*>* Deliverable::GetUseSMap() {
  return &use_hash_;
}
std::unordered_map<Container*, std::list<Variable*>*>* Deliverable::GetUseCMap() {
  return &container_use_hash_;
}
std::unordered_map<Statement*, std::list<Variable*>*>* Deliverable::GetModifiesSMap() {
  return &modifies_hash_;
}
std::unordered_map<Container*, std::list<Variable*>*>* Deliverable::GetModifiesCMap() {
  return &container_modifies_hash_;
}
std::unordered_map<Procedure*, std::list<Procedure*>*>* Deliverable::GetCallsMap() {
  return &calls_hash_;
}
std::unordered_map<Procedure*, std::list<Procedure*>*>* Deliverable::GetCallsTMap() {
  return &calls_T_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetNextMap() {
  return &next_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetFollowedByMap() {
  return &followed_by_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetFollowedByTMap() {
  return &followed_by_T_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetChildMap() {
  return &child_to_parent_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetChildTMap() {
  return &child_to_parent_T_hash_;
}
std::unordered_map<Variable*, std::list<Statement*>*>* Deliverable::GetUsedBySMap() {
  return &used_by_hash_;
}
std::unordered_map<Variable*, std::list<Container*>*>* Deliverable::GetUsedByCMap() {
  return &container_used_by_hash_;
}
std::unordered_map<Variable*, std::list<Statement*>*>* Deliverable::GetModifiedBySMap() {
  return &modified_by_hash_;
}
std::unordered_map<Variable*, std::list<Container*>*>* Deliverable::GetModifiedByCMap() {
  return &container_modified_by_hash_;
}
std::unordered_map<Procedure*, std::list<Procedure*>*>* Deliverable::GetCalledByMap() {
  return &called_by_hash_;
}
std::unordered_map<Procedure*, std::list<Procedure*>*>* Deliverable::GetCalledByTMap() {
  return &called_by_T_hash_;
}
std::unordered_map<Statement*, std::list<Statement*>*>* Deliverable::GetPrevMap() {
  return &previous_hash_;
}
