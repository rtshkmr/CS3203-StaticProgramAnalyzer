/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

If::If(std::string condition) {
  cond_expr_ = new ConditionalExpression(condition);
}

ConditionalExpression* If::getCondExpr() {
  return cond_expr_;
}

std::vector<Statement>* If::getIfStmtList() {
  return &if_stmt_list_;
}

std::vector<Statement>* If::getElseStmtList() {
  return else_stmt_list_->getElseStmtList();
}

bool If::setElseStmtList(Else* else_stmt) {
  else_stmt_list_ = else_stmt;
  return true; //assume success setter
}

Else::Else() {

}

std::vector<Statement>* Else::getElseStmtList() {
  return &else_stmt_list_;
}

While::While(std::string condition) {
  cond_expr_ = new ConditionalExpression(condition);
}

ConditionalExpression* While::getCondExpr() {
  return cond_expr_;
}

std::vector<Statement>* While::getStmtList() {
  return &stmt_list_;
}

Assign::Assign(Variable* var, std::string expression) {
  assigned_to_ = var;
  expr_ = new AssignmentExpression(expression);
}

Variable* Assign::getVariable() {
  return assigned_to_;
}

AssignmentExpression* Assign::getAssignmentExpr() {
  return expr_;
}

Call::Call(Procedure* proc_name) {
  proc_name_ = proc_name;
}

Procedure* Call::getProcedure() {
  return proc_name_;
}

Print::Print(Variable* var_name) {
  var_name_ = var_name;
}

Variable* Print::getVariable() {
  return var_name_;
}

Read::Read(Variable* var_name) {
  var_name_ = var_name;
}

Variable* Read::getVariable() {
  return var_name_;
}
