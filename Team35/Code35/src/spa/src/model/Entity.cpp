/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"

void Container::AddStatement(Statement* stmt) {
  statement_list_.push_back(stmt);
}

std::list<Statement*>* Container::GetStatementList() {
  return & statement_list_;
}

Procedure::Procedure(ProcedureName* pName) {
  procedure_name_ = pName;
}

const ProcedureName* Procedure::GetName() {
  return procedure_name_;
}

Variable::Variable(VariableName* vName) {
  variable_name_ = vName;
}

const VariableName* Variable::GetName() {
  return variable_name_;
}

Program::Program(Procedure* p) {
  procedure_list_.push_back(p);
}

std::list<Procedure*>* Program::GetProcedureList() {
  return & procedure_list_;
}

void Program::AddProcedure(Procedure* p) {
  procedure_list_.push_back(p);
}

void Statement::SetStatementNumber(StatementNumber* sn) {
  statement_number_ = sn;
}

void Statement::SetLineNumber(LineNumber* ln) {
  line_number_ = ln;
}

void Statement::SetParentNode(Container* parent) {
  parent_node_ = parent;
}

void Statement::SetBeforeNode(Statement* before) {
  before_node_ = before;
}

Container* Statement::GetParentNode() {
  return parent_node_;
}

Statement* Statement::GetBeforeNode() {
  return before_node_;
}

StatementNumber* Statement::GetStatementNumber() {
  return statement_number_;
}

LineNumber* Statement::GetLineNumber() {
  return line_number_;
}
