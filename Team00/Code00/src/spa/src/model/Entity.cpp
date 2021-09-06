/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"

Procedure::Procedure(ProcedureName* pName) {
  procedure_name_ = pName;
}

void Procedure::AddStatement(Statement stmt) {
  statement_list_.push_back(stmt);
}

const ProcedureName* Procedure::getName() {
  return procedure_name_;
}

std::list<Statement>* Procedure::GetStatementList() {
  return &statement_list_;
}

Variable::Variable(VariableName* vName) {
  variable_name_ = vName;
}

const VariableName* Variable::getName() {
  return variable_name_;
}

Program::Program(Procedure p) {
  procedure_list_.push_back(p);
}

std::list<Procedure>* Program::getProcedureList() {
  return &procedure_list_;
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
