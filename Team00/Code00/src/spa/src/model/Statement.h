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

class ElseEntity;

/**
 * IfEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for an if-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true.
 *   -  An ElseEntity object which contains the statements to execute if the conditional expression evaluates to false.
 */
class IfEntity : public Statement, public Container {
 private:
  ConditionalExpression* cond_expr_;
  std::vector<Variable*> expr_variables;
  std::vector<ConstantValue*> expr_constants;
  std::list<Statement*> if_stmt_list_;
  ElseEntity* else_stmt_list_; //TODO: check if keeping ELSE as object or merge ELSE object into IF object

 public:
  IfEntity(std::string condition, std::vector<Variable*> expr_variables, std::vector<ConstantValue*> expr_constants);

  ConditionalExpression* getCondExpr();

  std::vector<Variable*> GetExpressionVariables();

  std::vector<ConstantValue*> GetExpressionConstants();

  std::list<Statement*>* GetStatementList();

  void AddStatement(Statement* stmt);

  std::list<Statement*>* getElseStmtList();

  bool setElseStmtList(ElseEntity* else_stmt);
};

/**
 * ElseEntity is a derived class of Statement and a composition object of IfEntity object.
 */
class ElseEntity : public Statement, public Container {
 private:
  std::list<Statement*> else_stmt_list_;
 public:
  ElseEntity();

  std::list<Statement*>* GetStatementList();

  void AddStatement(Statement* stmt);
};

/**
 * WhileEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for a while-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true.
 */
class WhileEntity : public Statement, public Container {
 private:
  ConditionalExpression* cond_expr_;
  std::list<Statement*> stmt_list_;
  std::vector<Variable*> expr_variables;
  std::vector<ConstantValue*> expr_constants;
 public:
  WhileEntity(std::string condition, std::vector<Variable*> expr_variables, std::vector<ConstantValue*> expr_constants);

  ConditionalExpression* getCondExpr();

  std::list<Statement*>* GetStatementList();

  void AddStatement(Statement* stmt);

  std::vector<Variable*> GetExpressionVariables();

  std::vector<ConstantValue*> GetExpressionConstants();
};

/**
 * AssignEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for an assign-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this expression is assigned to.
 *   -  An AssignmentExpression which contains the full expression to evaluate for this statement.
 */
class AssignEntity : public Statement {
 private:
  Variable* assigned_to_;
  AssignmentExpression* expr_;
  std::vector<Variable*> expr_variables;
  std::vector<ConstantValue*> expr_constants;
 public:
  AssignEntity(Variable* var,
               std::string expression,
               std::vector<Variable*> expr_variables,
               std::vector<ConstantValue*> expr_constants);

  Variable* getVariable();

  AssignmentExpression* getAssignmentExpr();

  std::vector<Variable*> GetExpressionVariables();

  std::vector<ConstantValue*> GetExpressionConstants();
};

/**
 * CallEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for a call-statement.
 * This class contains the following:
 *   -  Procedure which is the procedure that this statement is calling.
 */
class CallEntity : public Statement {
 private:
  Procedure* proc_name_;

 public:
  CallEntity(Procedure* proc_name);

  Procedure* getProcedure();
};

/**
 * PrintEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for a print-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to print.
 */
class PrintEntity : public Statement {
 private:
  Variable* var_name_;
 public:
  PrintEntity(Variable* var_name);

  Variable* getVariable();
};

/**
 * ReadEntity is a derived class of Statement.
 * This object represents the abstract syntax grammar for a read-statement.
 * This class contains the following:
 *   -  Variable which is the variable that this statement is going to read.
 */
class ReadEntity : public Statement {
 private:
  Variable* var_name_;
 public:
  ReadEntity(Variable* var_name);

  Variable* getVariable();
};

#endif //AUTOTESTER_STATEMENT_H
