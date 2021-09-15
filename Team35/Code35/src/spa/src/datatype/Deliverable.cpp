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
  // Follows is a 1-to-1 relationship so if f1 was not inserted into follow_hash_,
  // f2 cannot be inserted into followed_by_hash
  if (follow_hash_.insert({f1, f2}).second) {   // only inserts if the key is unique
    followed_by_hash_.insert({f2, f1}); // only inserts if the insertion into follow_hash is true
  };
}

void Deliverable::AddFollowsTransitiveRelationship(Statement* before, Statement* after) {
  if (follows_T_hash_.count(before)) {
    std::list<Statement*>* afters = follows_T_hash_.find(before)->second;
    if (std::find(afters->begin(), afters->end(), after) == afters->end()) {
      // add after if it does not exist in afters
      afters->push_back(after);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(after);
    follows_T_hash_.insert(std::make_pair(before, list));
  }

  if (followed_by_T_hash_.count(after)) {
    std::list<Statement*>* befores = followed_by_T_hash_.find(after)->second;
    if (std::find(befores->begin(), befores->end(), before) == befores->end()) {
      // add before if it does not exist in befores
      befores->push_back(before);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(before);
    followed_by_T_hash_.insert(std::make_pair(after, list));
  }
}

void Deliverable::AddFollowsTransitiveRelationshipForList(Statement* before, std::list<Statement*>* afters) {
  for (Statement* after: *afters) {
    AddFollowsTransitiveRelationship(before, after);
  }
}

void Deliverable::AddParentRelationship(Statement* parent, Statement* child) {
  if (parent_to_child_hash_.count(parent)) {
    std::list<Statement*>* children = parent_to_child_hash_.find(parent)->second;
    if (std::find(children->begin(), children->end(), child) == children->end()) {
      // add child if it does not exist in children
      children->push_back(child);
    }
  } else {
    std::list<Statement*>* lst = new std::list<Statement*>();
    lst->push_back(child);
    parent_to_child_hash_.insert(make_pair(parent, lst));
  }

  child_to_parent_hash_.insert({child, parent}); // only inserts if the key is unique
}

void Deliverable::AddParentTransitiveRelationship(Statement* parent, Statement* child) {
  if (parent_to_child_T_hash_.count(parent)) {
    std::list<Statement*>* children = parent_to_child_T_hash_.find(parent)->second;
    if (std::find(children->begin(), children->end(), child) == children->end()) {
      // add child if it does not exist in children
      children->push_back(child);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(child);
    parent_to_child_T_hash_.insert(std::make_pair(parent, list));
  }

  if (child_to_parent_T_hash_.count(child)) {
    std::list<Statement*>* parents = child_to_parent_T_hash_.find(child)->second;
    if (std::find(parents->begin(), parents->end(), parent) == parents->end()) {
      // add parent if it does not exist in parents
      parents->push_back(parent);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(parent);
    child_to_parent_T_hash_.insert(std::make_pair(child, list));
  }
}

void Deliverable::AddParentTransitiveRelationshipForList(Statement* parent, std::list<Statement*>* children) {
  for (Statement* child: *children) {
    AddParentTransitiveRelationship(parent, child);
  }
}

void Deliverable::AddUsesRelationship(Statement* u1, Variable* u2) {
  if (use_hash_.count(u1)) {
    std::list<Variable*>* used_vars = use_hash_.find(u1)->second;
    if (std::find(used_vars->begin(), used_vars->end(), u2) == used_vars->end()) {
      // add var u2 if it does not exist in used_vars
      used_vars->push_back(u2);
    }
  } else {
    std::list<Variable*>* lst = new std::list<Variable*>();
    lst->push_back(u2);
    use_hash_.insert(make_pair(u1, lst));
  }

  if (used_by_hash_.count(u2)) {
    std::list<Statement*>* used_by_stmts = used_by_hash_.find(u2)->second;
    if (std::find(used_by_stmts->begin(), used_by_stmts->end(), u1) == used_by_stmts->end()) {
      // add statement u2 if it does not exist in used_by_stmts
      used_by_stmts->push_back(u1);
    }
  } else {
    std::list<Statement*>* lst = new std::list<Statement*>();
    lst->push_back(u1);
    used_by_hash_.insert(make_pair(u2, lst));
  }
}

void Deliverable::AddUsesRelationship(Container* u1, Variable* u2) {
  if (container_use_hash_.count(u1)) {
    std::list<Variable*>* used_vars = container_use_hash_.find(u1)->second;
    if (std::find(used_vars->begin(), used_vars->end(), u2) == used_vars->end()) {
      // add var u2 if it does not exist in used_vars
      used_vars->push_back(u2);
    }
  } else {
    std::list<Variable*>* lst = new std::list<Variable*>();
    lst->push_back(u2);
    container_use_hash_.insert(make_pair(u1, lst));
  }

  if (container_used_by_hash_.count(u2)) {
    std::list<Container*>* used_by_conts = container_used_by_hash_.find(u2)->second;
    if (std::find(used_by_conts->begin(), used_by_conts->end(), u1) == used_by_conts->end()) {
      // add container u1 if it does not exist in used_by_conts
      used_by_conts->push_back(u1);
    }
  } else {
    std::list<Container*>* lst = new std::list<Container*>();
    lst->push_back(u1);
    container_used_by_hash_.insert(make_pair(u2, lst));
  }
}

void Deliverable::AddUsesRelationship(Container* container, std::list<Variable*>* var_list) {
  for (Variable* var: *var_list) {
    AddUsesRelationship(container, var);
  }
}

void Deliverable::AddModifiesRelationship(Statement* m1, Variable* m2) {
  if (modifies_hash_.count(m1)) {
    std::list<Variable*>* modified_vars = modifies_hash_.find(m1)->second;
    if (std::find(modified_vars->begin(), modified_vars->end(), m2) == modified_vars->end()) {
      // add var m2 if it does not exist in modified_vars
      modified_vars->push_back(m2);
    }
  } else {
    std::list<Variable*>* lst = new std::list<Variable*>();
    lst->push_back(m2);
    modifies_hash_.insert(make_pair(m1, lst));
  }

  if (modified_by_hash_.count(m2)) {
    std::list<Statement*>* modified_by_stmts = modified_by_hash_.find(m2)->second;
    if (std::find(modified_by_stmts->begin(), modified_by_stmts->end(), m1) == modified_by_stmts->end()) {
      // add statement m1 if it does not exist in modified_by_stmts
      modified_by_stmts->push_back(m1);
    }
  } else {
    std::list<Statement*>* lst = new std::list<Statement*>();
    lst->push_back(m1);
    modified_by_hash_.insert(make_pair(m2, lst));
  }
}

void Deliverable::AddModifiesRelationship(Container* m1, Variable* m2) {
  if (container_modifies_hash_.count(m1)) {
    std::list<Variable*>* modified_vars = container_modifies_hash_.find(m1)->second;
    if (std::find(modified_vars->begin(), modified_vars->end(), m2) == modified_vars->end()) {
      // add var m2 if it does not exist in modified_vars
      modified_vars->push_back(m2);
    }
  } else {
    std::list<Variable*>* lst = new std::list<Variable*>();
    lst->push_back(m2);
    container_modifies_hash_.insert(make_pair(m1, lst));
  }

  if (container_modified_by_hash_.count(m2)) {
    std::list<Container*>* modified_by_conts = container_modified_by_hash_.find(m2)->second;
    if (std::find(modified_by_conts->begin(), modified_by_conts->end(), m1) == modified_by_conts->end()) {
      // add container m1 if it does not exist in modified_by_conts
      modified_by_conts->push_back(m1);
    }
  } else {
    std::list<Container*>* lst = new std::list<Container*>();
    lst->push_back(m1);
    container_modified_by_hash_.insert(make_pair(m2, lst));
  }
}

void Deliverable::AddModifiesRelationship(Container* container, std::list<Variable*>* var_list) {
  for (Variable* var: *var_list) {
    AddModifiesRelationship(container, var);
  }
}

void Deliverable::SetProgram(Program* program) {
  this->program_ = program;
}

Program* Deliverable::GetProgram() {
  return program_;
}

std::list<Procedure*>* Deliverable::GetProcList() {
  return &proc_list_;
}

std::list<Variable*>* Deliverable::GetVariableList() {
  return &var_list_;
}
std::list<ConstantValue*>* Deliverable::GetConstantValueList() {
  return &const_list_;
}

std::list<Statement*>* Deliverable::GetStatementList() {
  return &stmt_list_;
}

std::list<IfEntity*>* Deliverable::GetIfList() {
  return &if_list_;
}

std::list<WhileEntity*>* Deliverable::GetWhileList() {
  return &while_list_;
}

std::list<AssignEntity*>* Deliverable::GetAssignList() {
  return &assign_list_;
}

std::list<CallEntity*>* Deliverable::GetCallList() {
  return &call_list_;
}

std::list<PrintEntity*>* Deliverable::GetPrintList() {
  return &print_list_;
}

std::list<ReadEntity*>* Deliverable::GetReadList() {
  return &read_list_;
}
