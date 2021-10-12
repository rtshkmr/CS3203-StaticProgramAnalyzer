/**
 * This file contains the high level Entity object Entity, Procedure, Variable, Program.
 */
#ifndef AUTOTESTER_ENTITY_H
#define AUTOTESTER_ENTITY_H

#include <list>
#include <datatype/DataType.h>

enum class EntityEnum {
  kNone = 0,
  kIfEntity = 1,
  kWhileEntity = 2,
  kAssignEntity = 3,
  kCallEntity = 4,
  kPrintEntity = 5,
  kReadEntity = 6,
  kElseEntity = 7,
  kProcedureEntity = 8
};

/**
 * Entity is an abstract class, an overarching class that covers any entity found in the program.
 */
class Entity {
 protected:
  EntityEnum type;
 public:
  virtual ~Entity() {};
  EntityEnum getEntityEnum();
};

class Container;

/**
 * Statement is an abstract class and derived from Entity.
 * This class contains the essential attributes that every statement-type object has, such as, line number,
 *     statement number, parent-node (for tracking Parent), and before-node (for tracking Follow).
 */
class Statement : public Entity {
 protected:
  LineNumber* line_number_;
  StatementNumber* statement_number_;
  Container* parent_node_;
  Statement* before_node_;
 public:
  virtual ~Statement() {};
  void SetLineNumber(LineNumber* ln);
  void SetStatementNumber(StatementNumber* sn);
  void SetParentNode(Container* parent);
  void SetBeforeNode(Statement* before);
  StatementNumber* GetStatementNumber();
  LineNumber* GetLineNumber();
  Container* GetParentNode();
  Statement* GetBeforeNode();
};

/**
 * Container is an abstract class which applies to all classes that can contain statements.
 * The statement list should only contain the Entity's immediate children statements.
 * e.g. in the case of If, only the If statement is in this list and not the nested statements.
 */
class Container {
 private:
  std::list<Statement*> statement_list_;
 public:
  virtual ~Container() {};

  void AddStatement(Statement* stmt);

  std::list<Statement*>* GetStatementList();
};

/**
 * Procedure is a derived class of Entity and Container. This class contains the name and the list of statement
 *   within this procedure. The list of statement is defined in the inherited Container abstract class.
 */
class Procedure : public Entity, public Container {
 private:
  const ProcedureName* procedure_name_;
 public:
  Procedure(ProcedureName* procedureName);

  const ProcedureName* GetName();
};

/**
 * Variable is a derived class of Entity. This class contains the name of the variable.
 */
class Variable : public Entity {
 private:
  const VariableName* variable_name_;
 public:
  Variable(VariableName* variableName);

  const VariableName* GetName();

  static std::vector<Variable*> SortVariableVector(std::vector<Variable*> var_list);
};

/**
 * Constant is a derived class of Entity. This class contains the value of the constant.
 */
class Constant : public Entity {
 private:
  const ConstantValue* constant_value_;
 public:
  Constant(ConstantValue* constantValue);

  const ConstantValue* GetValue();
};


/**
 * Program is the root node of the AST. It contains the different Procedures found within the SIMPLE source code.
 */
class Program {
 private:
  std::list<Procedure*> procedure_list_; //must be one or more

 public:
  Program(Procedure* p);

  std::list<Procedure*>* GetProcedureList();

  void AddProcedure(Procedure* p);
};

#endif //AUTOTESTER_ENTITY_H
