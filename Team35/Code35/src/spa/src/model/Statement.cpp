/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

using std::string;
using std::vector;

IfEntity::IfEntity(std::string condition, vector<Variable*> expr_variables, vector<ConstantValue*> expr_constants) {
  cond_expr_ = new ConditionalExpression(condition);
  this->expr_variables = std::move(expr_variables);
  this->expr_constants = std::move(expr_constants);
}

ConditionalExpression* IfEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> IfEntity::GetExpressionVariables() {
  return expr_variables;
}

vector<ConstantValue*> IfEntity::GetExpressionConstants() {
  return expr_constants;
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

}

WhileEntity::WhileEntity(std::string condition,
                         vector<Variable*> expr_variables,
                         vector<ConstantValue*> expr_constants) {
  cond_expr_ = new ConditionalExpression(condition);
  this->expr_variables = std::move(expr_variables);
  this->expr_constants = std::move(expr_constants);
}

ConditionalExpression* WhileEntity::GetCondExpr() {
  return cond_expr_;
}

vector<Variable*> WhileEntity::GetExpressionVariables() {
  return expr_variables;
}

vector<ConstantValue*> WhileEntity::GetExpressionConstants() {
  return expr_constants;
}

AssignEntity::AssignEntity(Variable* var,
                           std::string expression,
                           vector<Variable*> expr_variables,
                           vector<ConstantValue*> expr_constants) {
  assigned_to_ = var;
  expr_ = new AssignmentExpression(expression);
  this->expr_variables = std::move(expr_variables);
  this->expr_constants = std::move(expr_constants);
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
  proc_name_ = proc_name;
}

Procedure* CallEntity::GetProcedure() {
  return proc_name_;
}

PrintEntity::PrintEntity(Variable* var_name) {
  var_name_ = var_name;
}

Variable* PrintEntity::GetVariable() {
  return var_name_;
}

ReadEntity::ReadEntity(Variable* var_name) {
  var_name_ = var_name;
}

Variable* ReadEntity::GetVariable() {
  return var_name_;
}
