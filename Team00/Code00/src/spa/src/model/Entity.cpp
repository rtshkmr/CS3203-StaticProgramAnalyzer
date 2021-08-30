/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"

Procedure::Procedure(ProcedureName const &pName) {
    procedureName = &pName;
}

const ProcedureName* Procedure::getName() {
    return procedureName;
}

std::list<Statement>* Procedure::getStatementList() {
    return &statementList;
}

Variable::Variable(const VariableName &vName) {
    variableName = &vName;
}

const VariableName* Variable::getName() {
    return variableName;
}

Program::Program(Procedure p) {
    procedureList.emplace_back(p);
}

std::list<Procedure>* Program::getProcedureList() {
    return &procedureList;
}
