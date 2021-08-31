/**
 * This file contains the high level Entity object Entity, Procedure, Variable, Program.
 */
#ifndef AUTOTESTER_ENTITY_H
#define AUTOTESTER_ENTITY_H

#include <list>
#include <datatype/DataType.h>

/**
 * Entity is an abstract class, an overarching class that covers any entity found in the program.
 */
class Entity {
public:
    virtual ~Entity() {};
};

/**
 * Statement is an abstract class and derived from Entity.
 * This class contains the essential attributes that every statement-type object has, such as, line number,
 *     statement number, parent-node (for tracking Parent), and before-node (for tracking Follow).
 */
class Statement : public Entity {
protected:
    LineNumber *lineNumber;
    StatementNumber *statementNumber;
    Statement *parentNode;
    Statement *beforeNode;
public:
    virtual ~Statement() {};
};

/**
 * Procedure is a derived class of Entity. This class contains the name and the list of statement
 *   within this procedure.
 */
class Procedure : public Entity {
private:
    const ProcedureName *procedureName;
    std::list<Statement> statementList;
public:
    Procedure(ProcedureName *procedureName);

    const ProcedureName *getName();

    std::list<Statement> *getStatementList();
};

/**
 * Variable is a derived class of Entity. This class contains the name of the variable.
 */
class Variable : public Entity {
private:
    const VariableName *variableName;
public:
    Variable(VariableName *variableName);

    const VariableName *getName();
};

/**
 * Program is the root node of the AST. It contains the different Procedures found within the SIMPLE source code.
 */
class Program {
private:
    std::list<Procedure> procedureList; //must be one or more

public:
    Program(Procedure p);

    std::list<Procedure> *getProcedureList();
};

#endif //AUTOTESTER_ENTITY_H
