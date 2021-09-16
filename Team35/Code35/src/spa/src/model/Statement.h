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
 * IfEntity is a derived class of Statement and Container.
 * This object represents the abstract syntax grammar for an if-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true, which is defined in the inherited Container
 *   abstract class.
 *   -  An ElseEntity object which contains the statements to execute if the conditional expression evaluates to false.
 */
class IfEntity : public Statement, public Container {
 private:
  ConditionalExpression* cond_expr_;
  std::vector<Variable*> expr_variables;
  std::vector<ConstantValue*> expr_constants;
  ElseEntity* else_entity_ = nullptr; //TODO: check if keeping ELSE as object or merge ELSE object into IF object

 public:
  IfEntity(std::string condition, std::vector<Variable*> expr_variables, std::vector<ConstantValue*> expr_constants);

  ConditionalExpression* GetCondExpr();

  std::vector<Variable*> GetExpressionVariables();

  std::vector<ConstantValue*> GetExpressionConstants();

  ElseEntity* GetElseEntity();

  void SetElseEntity(ElseEntity* else_entity);

  std::list<Statement*>* GetElseStmtList();
};

/**
 * ElseEntity is a derived class of Statement and Container, and a composition object of IfEntity object.
 * It contains a list of statement defined in the inherited Container abstract class.
 */
class ElseEntity : public Statement, public Container {
 public:
  ElseEntity();
};

/**
 * WhileEntity is a derived class of Statement and Container.
 * This object represents the abstract syntax grammar for a while-statement.
 * This class contains the following:
 *   -  ConditionalExpression which is the conditional expression for the if-statement.
 *   -  A list of Statement to execute when the above condition is true, which is defined in the inherited Container
 *   abstract class.
 */
class WhileEntity : public Statement, public Container {
 private:
  ConditionalExpression* cond_expr_;
  std::vector<Variable*> expr_variables;
  std::vector<ConstantValue*> expr_constants;
 public:
  WhileEntity(std::string condition, std::vector<Variable*> expr_variables, std::vector<ConstantValue*> expr_constants);

  ConditionalExpression* GetCondExpr();

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

  Variable* GetVariable();

  AssignmentExpression* GetAssignmentExpr();

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

  Procedure* GetProcedure();
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

  Variable* GetVariable();
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

  Variable* GetVariable();
};

#endif //AUTOTESTER_STATEMENT_H
