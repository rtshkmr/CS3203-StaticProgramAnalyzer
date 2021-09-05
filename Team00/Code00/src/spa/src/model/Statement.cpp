/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

IfEntity::IfEntity(std::string condition) {
  cond_expr_ = new ConditionalExpression(condition);
}

ConditionalExpression* IfEntity::getCondExpr() {
  return cond_expr_;
}

std::list<Statement>* IfEntity::GetStatementList() {
  return &if_stmt_list_;
}

void IfEntity::AddStatement(Statement stmt) {
  if_stmt_list_.push_back(stmt);
}

std::list<Statement>* IfEntity::getElseStmtList() {
  return else_stmt_list_->GetStatementList();
}

bool IfEntity::setElseStmtList(ElseEntity* else_stmt) {
  else_stmt_list_ = else_stmt;
  return true; //assume success setter
}

ElseEntity::ElseEntity() {

}

std::list<Statement>* ElseEntity::GetStatementList() {
  return &else_stmt_list_;
}

void ElseEntity::AddStatement(Statement stmt) {
  else_stmt_list_.push_back(stmt);
}

WhileEntity::WhileEntity(std::string condition) {
  cond_expr_ = new ConditionalExpression(condition);
}

ConditionalExpression* WhileEntity::getCondExpr() {
  return cond_expr_;
}

std::list<Statement>* WhileEntity::GetStatementList() {
  return &stmt_list_;
}

void WhileEntity::AddStatement(Statement stmt) {
  stmt_list_.push_back(stmt);
}

AssignEntity::AssignEntity(Variable* var, std::string expression) {
  assigned_to_ = var;
  expr_ = new AssignmentExpression(expression);
}

Variable* AssignEntity::getVariable() {
  return assigned_to_;
}

AssignmentExpression* AssignEntity::getAssignmentExpr() {
  return expr_;
}

CallEntity::CallEntity(Procedure* proc_name) {
  proc_name_ = proc_name;
}

Procedure* CallEntity::getProcedure() {
  return proc_name_;
}

PrintEntity::PrintEntity(Variable* var_name) {
  var_name_ = var_name;
}

Variable* PrintEntity::getVariable() {
  return var_name_;
}

ReadEntity::ReadEntity(Variable* var_name) {
  var_name_ = var_name;
}

Variable* ReadEntity::getVariable() {
  return var_name_;
}
