/**
 * This file contains the commonly used statement types in SIMPLE program.
 * Objects created from here should be of valid syntax and stored in their abstract form.
 */

#include <vector>
#include "datatype/DataType.h"
#include "datatype/AssignmentExpression.h"
#include "datatype/ConditionalExpression.h"

#include "Entity.h"

#ifndef AUTOTESTER_STATEMENT_H
#define AUTOTESTER_STATEMENT_H

class Else;

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
  ConditionalExpression* cond_expr_;
  std::vector<Statement> if_stmt_list_;
  Else* else_stmt_list_; //TODO: check if keeping ELSE as object or merge ELSE object into IF object

 public:
  If(std::string condition);

  ConditionalExpression* getCondExpr();

  std::vector<Statement>* getIfStmtList();

  std::vector<Statement>* getElseStmtList();

  bool setElseStmtList(Else* else_stmt);
};

/**
 * Else is a derived class of Statement and a composition object of If object.
 */
class Else : public Statement {
 private:
  std::vector<Statement> else_stmt_list_;
 public:
  Else();

  std::vector<Statement>* getElseStmtList();
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
  ConditionalExpression* cond_expr_;
  std::vector<Statement> stmt_list_;
 public:
  While(std::string condition);

  ConditionalExpression* getCondExpr();

  std::vector<Statement>* getStmtList();

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
  Variable* assigned_to_;
  AssignmentExpression* expr_;
 public:
  Assign(Variable* var, std::string expression);

  Variable* getVariable();

  AssignmentExpression* getAssignmentExpr();
};

/**
 * Call is a derived class of Statement.
 * This object represents the abstract syntax grammar for a call-statement.
 * This class contains the following:
 *   -  Procedure which is the procedure that this statement is calling.
 */
class Call : public Statement {
 private:
  Procedure* proc_name_;

 public:
  Call(Procedure* proc_name);

  Procedure* getProcedure();
};

/**
 * Print is a derived class of Statement.
 * This object represents the abstract syntax grammar for a print-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to print.
 */
class Print : public Statement {
 private:
  Variable* var_name_;
 public:
  Print(Variable* var_name);

  Variable* getVariable();
};

/**
 * Read is a derived class of Statement.
 * This object represents the abstract syntax grammar for a read-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to read.
 */
class Read : public Statement {
 private:
  Variable* var_name_;
 public:
  Read(Variable* var_name);

  Variable* getVariable();
};

#endif //AUTOTESTER_STATEMENT_H
