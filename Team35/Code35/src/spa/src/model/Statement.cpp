/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

using std::string;
using std::vector;

IfEntity::IfEntity(std::string condition, vector<Variable*> expr_variables, vector<ConstantValue*> expr_constants) {
  type = EntityEnum::kIfEntity;
  cond_expr_ = new ConditionalExpression(condition, expr_variables);
  this->control_variables = std::move(expr_variables);
  this->control_constants = std::move(expr_constants);

  for(auto* variable : control_variables) {
    variable->AddEntity(this);
  }
}

ConditionalExpression* IfEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> IfEntity::GetExpressionVariables() {
  return control_variables;
}

vector<ConstantValue*> IfEntity::GetExpressionConstants() {
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

ElseEntity::ElseEntity() {
  type = EntityEnum::kElseEntity;
}

WhileEntity::WhileEntity(std::string condition,
                         vector<Variable*> expr_variables,
                         vector<ConstantValue*> expr_constants) {
  type = EntityEnum::kWhileEntity;
  cond_expr_ = new ConditionalExpression(condition, expr_variables);
  this->control_variables = std::move(expr_variables);
  this->control_constants = std::move(expr_constants);

  for(auto* variable : control_variables) {
    variable->AddEntity(this);
  }
}

ConditionalExpression* WhileEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> WhileEntity::GetExpressionVariables() {
  return control_variables;
}

vector<ConstantValue*> WhileEntity::GetExpressionConstants() {
  return control_constants;
}

AssignEntity::AssignEntity(Variable* var,
                           std::string expression,
                           vector<Variable*> expr_variables,
                           vector<ConstantValue*> expr_constants) {
  type = EntityEnum::kAssignEntity;
  assigned_to_ = var;
  expr_ = new AssignmentExpression(expression);
  this->expr_variables = std::move(expr_variables);
  this->expr_constants = std::move(expr_constants);

  var->AddEntity(this);
  for(auto* variable : this->expr_variables) {
    variable->AddEntity(this);
  }
}

Variable* AssignEntity::GetVariable() {
  return assigned_to_;
}

AssignmentExpression* AssignEntity::GetAssignmentExpr() {
  return expr_;
}

vector<Variable*> AssignEntity::GetExpressionVariables() {
  return expr_variables;
}

vector<ConstantValue*> AssignEntity::GetExpressionConstants() {
  return expr_constants;
}

CallEntity::CallEntity(Procedure* proc_name) {
  type = EntityEnum::kCallEntity;
  proc_name_ = proc_name;
}

Procedure* CallEntity::GetProcedure() {
  return proc_name_;
}

PrintEntity::PrintEntity(Variable* var_name) {
  type = EntityEnum::kPrintEntity;
  var_name_ = var_name;
  var_name_->AddEntity(this);
}

Variable* PrintEntity::GetVariable() {
  return var_name_;
}

ReadEntity::ReadEntity(Variable* var_name) {
  type = EntityEnum::kReadEntity;
  var_name_ = var_name;
  var_name_->AddEntity(this);
}

Variable* ReadEntity::GetVariable() {
  return var_name_;
}
