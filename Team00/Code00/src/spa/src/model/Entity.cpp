/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"

Procedure::Procedure(ProcedureName* pName) {
  procedure_name_ = pName;
}

const ProcedureName* Procedure::getName() {
  return procedure_name_;
}

std::list<Statement>* Procedure::getStatementList() {
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
