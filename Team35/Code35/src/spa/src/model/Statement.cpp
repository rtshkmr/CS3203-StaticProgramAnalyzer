/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

#include <utility>

using std::string;
using std::vector;

IfEntity::IfEntity(const std::string& condition, vector<Variable*> control_variables,
                   vector<Constant*> control_constants) {
  type = EntityEnum::kIfEntity;
  cond_expr_ = new ConditionalExpression(condition, control_variables);
  this->control_variables = std::move(control_variables);
  this->control_constants = std::move(control_constants);
  for(auto* variable : this->control_variables) {
    variable->AddStatement(this);
  }
}

ConditionalExpression* IfEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> IfEntity::GetControlVariables() {
  return control_variables;
}

vector<Constant*> IfEntity::GetControlConstants() {
  return control_constants;
}

ElseEntity* IfEntity::GetElseEntity() {
  return else_entity_;
}

void IfEntity::SetElseEntity(ElseEntity* else_entity) {
  else_entity_ = else_entity;
}

std::list<Statement*>* IfEntity::GetElseStmtList() {
  return else_entity_->GetStatementList();
}
void IfEntity::AddStatementToElseEntity(Statement* statement) {
  else_entity_->AddStatement(statement);
}
int IfEntity::GetElseStatementListSize() {
  return else_entity_->GetStatementList()->size();
}

ElseEntity::ElseEntity() {
  type = EntityEnum::kElseEntity;
}

WhileEntity::WhileEntity(const std::string& condition,
                         vector<Variable*> control_variables,
                         vector<Constant*> control_constants) {
  type = EntityEnum::kWhileEntity;
  cond_expr_ = new ConditionalExpression(condition, control_variables);
  this->control_variables = std::move(control_variables);
  this->control_constants = std::move(control_constants);
  for(auto* variable : this->control_variables) {
    variable->AddStatement(this);
  }
}

ConditionalExpression* WhileEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> WhileEntity::GetControlVariables() {
  return control_variables;
}

vector<Constant*> WhileEntity::GetControlConstants() {
  return control_constants;
}

AssignEntity::AssignEntity(Variable* var,
                           std::string expression,
                           vector<Variable*> expr_variables,
                           vector<Constant*> expr_constants) {
  type = EntityEnum::kAssignEntity;
  assigned_to_ = var;
  expr_ = new AssignmentExpression(std::move(expression));
  this->expr_variables = std::move(expr_variables);
  this->expr_constants = std::move(expr_constants);

  var->AddStatement(this);
  for(auto* variable : this->expr_variables) {
    variable->AddStatement(this);
  }
}

Variable* AssignEntity::GetVariableObj() {
  return assigned_to_;
}

AssignmentExpression* AssignEntity::GetAssignmentExpr() {
  return expr_;
}

vector<Variable*> AssignEntity::GetExprVariables() {
  return expr_variables;
}

vector<Constant*> AssignEntity::GetExprConstants() {
  return expr_constants;
}
std::string AssignEntity::GetVariableString() {
  return assigned_to_->GetName();
}

CallEntity::CallEntity(Procedure* proc_name) {
  type = EntityEnum::kCallEntity;
  called_proc_name_ = proc_name;
}

Procedure* CallEntity::GetCalledProcedure() {
  return called_proc_name_;
}

PrintEntity::PrintEntity(Variable* var_name) {
  type = EntityEnum::kPrintEntity;
  var_name_ = var_name;
  var_name_->AddStatement(this);
}

Variable* PrintEntity::GetVariableObj() {
  return var_name_;
}
std::string PrintEntity::GetVariableString() {
  return var_name_->GetName();
}

ReadEntity::ReadEntity(Variable* var_name) {
  type = EntityEnum::kReadEntity;
  var_name_ = var_name;
  var_name_->AddStatement(this);
}

Variable* ReadEntity::GetVariableObj() {
  return var_name_;
}
std::string ReadEntity::GetVariableString() {
  return var_name_->GetName();
}
