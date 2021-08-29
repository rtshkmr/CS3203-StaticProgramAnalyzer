/**
 * This file contains the high level Entity object @code{Entity}, @code{Procedure}, @code{Variable}, @code{Program}.
 */
#ifndef AUTOTESTER_ENTITY_H
#define AUTOTESTER_ENTITY_H

/**
 * @code{Entity} is an abstract class, an overarching class that covers any entity found in the program.
 */
class Entity {
public:
    virtual ~Entity() {};
};

/**
 * @code{Procedure} is a derived class of @code{Entity}. This class contains the name and the list of statement
 *   within this procedure.
 */
class Procedure : public Entity {
private:
    ProcedureName procedureName;
    list<Statement> statementList;
public:
    Procedure(ProcedureName procedureName);
};

/**
 * @code{Variable} is a derived class of @code{Entity}. This class contains the name of the variable.
 */
class Variable : public Entity {
private:
    VariableName variableName;
public:
    Variable(string variableName);
};

/**
 * @code{Program} is the root node of the AST. It contains the different Procedures found within the SIMPLE source code.
 */
class Program {
private:
    list<Procedure> procedureList; //must be one or more

public:
    Program(Procedure p) {
        procedureList = { p };
    }
};

#endif //AUTOTESTER_ENTITY_H
