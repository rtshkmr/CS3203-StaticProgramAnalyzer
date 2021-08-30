/**
 * This file contains the high level Entity object Entity, Procedure, Variable, Program.
 */
#ifndef AUTOTESTER_ENTITY_H
#define AUTOTESTER_ENTITY_H

#include <datatype/DataType.h>
#include "Statement.h"

/**
 * Entity is an abstract class, an overarching class that covers any entity found in the program.
 */
class Entity {
public:
    virtual ~Entity() {};
};

/**
 * Procedure is a derived class of Entity. This class contains the name and the list of statement
 *   within this procedure.
 */
class Procedure : public Entity {
private:
    const ProcedureName* procedureName;
    list<Statement> statementList;
public:
    Procedure(ProcedureName const &procedureName);
    const ProcedureName* getName();
    list<Statement>* getStatementList();
};

/**
 * Variable is a derived class of Entity. This class contains the name of the variable.
 */
class Variable : public Entity {
private:
    const VariableName* variableName;
public:
    Variable(VariableName const &variableName);
    const VariableName* getName();
};

/**
 * Program is the root node of the AST. It contains the different Procedures found within the SIMPLE source code.
 */
class Program {
private:
    std::list<Procedure> procedureList; //must be one or more

public:
    Program(Procedure p);
    std::list<Procedure>* getProcedureList();
};

#endif //AUTOTESTER_ENTITY_H
