/**
 * This file contains the high level Entity object Entity, Procedure, Variable, Program.
 */
#ifndef AUTOTESTER_ENTITY_H
#define AUTOTESTER_ENTITY_H

#include <list>
#include <datatype/DataType.h>
#include <set>
#include "CFG.h"

enum class EntityEnum {
  kNone = 0,
  kIfEntity = 1,
  kWhileEntity = 2,
  kAssignEntity = 3,
  kCallEntity = 4,
  kPrintEntity = 5,
  kReadEntity = 6,
  kElseEntity = 7,
  kProcedureEntity = 8,
  kVariableEntity = 9,
  kConstantEntity = 10
};

/**
 * Entity is an abstract class, an overarching class that covers any entity found in the program.
 */
class Entity {
 protected:
  EntityEnum type;
 public:
  virtual ~Entity() {};
  EntityEnum GetEntityEnum();
};

class Container;
class Procedure;

/**
 * Statement is an abstract class and derived from Entity.
 * This class contains the essential attributes that every statement-type object has, such as, line number,
 *     statement number, parent-node (for tracking Parent), and before-node (for tracking Follow).
 */
class Statement : public Entity {
 protected:
  LineNumber* line_number_;
  StatementNumber* statement_number_;
  Procedure* procedure_node_;
  Container* parent_node_;
  Statement* before_node_;
 public:
  virtual ~Statement() {};
  void SetLineNumber(LineNumber* ln);
  void SetStatementNumber(StatementNumber* sn);
  void SetProcedureNode(Procedure* procedure);
  void SetParentNode(Container* parent);
  void SetBeforeNode(Statement* before);
  StatementNumber* GetStatementNumber();
  LineNumber* GetLineNumber();
  std::string GetLineNumberString();
  Procedure* GetProcedureNode();
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

class Cluster;
class Block;
/**
 * Procedure is a derived class of Entity and Container. This class contains the name and the list of statement
 *   within this procedure. The list of statement is defined in the inherited Container abstract class.
 */
class Procedure : public Entity, public Container {
 private:
  const ProcedureName* procedure_name_;
  const Cluster* cluster_root_ = nullptr;
  const Block* block_root_ = nullptr;
  const Block* block_tail_ = nullptr;
 public:
  Procedure(ProcedureName* procedureName);
  const ProcedureName* GetName();
  Cluster* GetInnermostCluster(int first_stmt, int second_stmt, Cluster* prev_cluster);
  const void SetClusterRoot(Cluster* cluster);
  const void SetBlockRoot(Block* block_root);
  const void SetBlockTail(Block* block_tail);
  const Cluster* GetClusterRoot();
  const Block* GetBlockRoot();
};

/**
 * Variable is a derived class of Entity. This class contains the name of the variable.
 */
class Variable : public Entity {
 private:
  const VariableName* variable_name_;
  std::vector<std::set<Statement*>> var_to_statement = { {}, {}, {}, {}, {}, {} }; //If, While, Assign, Call (empty), Print, Read
 public:
  Variable(VariableName* variableName);

  const VariableName* GetName();

  void AddStatement(Statement* stmt);

  std::vector<std::set<Statement*>> GetStatementTable();

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
  Cluster* GetProcClusterForLineNum(int line_num);
  Procedure* GetProcForLineNum(int line_num);
  Cluster* GetEncapsulatingCluster(int first_stmt, int second_stmt, Cluster* prev_cluster);

  void AddProcedure(Procedure* p);
};

#endif //AUTOTESTER_ENTITY_H
