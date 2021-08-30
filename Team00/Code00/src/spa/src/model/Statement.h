/**
 * This file contains the commonly used statement types in SIMPLE program.
 * Objects created from here should be of valid syntax and stored in their abstract form.
 */

#include "Entity.h"
#include "datatype/ConditionalExpression.h"
#include "datatype/DataType.h"
#include "datatype/AssignmentExpression.h"

#ifndef AUTOTESTER_STATEMENT_H
#define AUTOTESTER_STATEMENT_H

/**
 * Statement is an abstract class and derived from Entity.
 * This class contains the essential attributes that every statement-type object has, such as, line number,
 *     statement number, parent-node (for tracking Parent), and before-node (for tracking Follow).
 */
class Statement : public Entity {
protected:
    LineNumber lineNumber;
    StatementNumber statementNumber;
    Statement parentNode;
    Statement beforeNode;
public:
    virtual ~Statement() {};
};

/**
 * If is a derived class of Statement.
 * This object represents the abstract syntax grammar for an if-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true.
 *   -  An Else object which contains the statements to execute if the conditional expression evaluates to false.
 */
class If : public Statement {
private:
    ConditionalExpression condExpr;
    vector<Statement> ifStmtList;
    Else elseStmtList; //TODO: check if keeping ELSE as object or merge ELSE object into IF object

public:
    If(std::string condition);
};

/**
 * Else is a derived class of Statement and a composition object of If object.
 */
class Else : public Statement {
private:
    vector<Statement> elseStmtList;
public:
    Else();
};

/**
 * While is a derived class of Statement.
 * This object represents the abstract syntax grammar for a while-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true.
 */
class While : public Statement {
private:
    ConditionalExpression condExpr;
    vector<Statement> stmtList;
public:
    While(std::string condition);
};

/**
 * Assign is a derived class of Statement.
 * This object represents the abstract syntax grammar for an assign-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this expression is assigned to.
 *   -  An AssignmentExpression which contains the full expression to evaluate for this statement.
 */
class Assign : public Statement {
private:
    Variable assignTo;
    AssignmentExpression expr;
public:
    Assign(string variable, string expression);
};

/**
 * Call is a derived class of Statement.
 * This object represents the abstract syntax grammar for a call-statement.
 * This class contains the following:
 *   -  Procedure which is the procedure that this statement is calling.
 */
class Call : public Statement {
private:
    Procedure proc;

public:
    Call(Procedure procName);
};

/**
 * Print is a derived class of Statement.
 * This object represents the abstract syntax grammar for a print-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to print.
 */
class Print : public Statement {
private:
    Variable var;
public:
    Print(Variable var);
};

/**
 * Read is a derived class of Statement.
 * This object represents the abstract syntax grammar for a read-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to read.
 */
class Read : public Statement {
private:
    Variable var;
public:
    Read(Variable var);
};

#endif //AUTOTESTER_STATEMENT_H
