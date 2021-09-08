#ifndef AUTOTESTER_PSUBSYSTEM_H
#define AUTOTESTER_PSUBSYSTEM_H

#include <string>
#include <stack>
#include <datatype/Deliverable.h>
#include "SyntaxValidator.h"
#include "EntityFactory.h"

/**
 * This namespace provides a scope for PSubsystem related declarations.
 */
namespace psub {
/**
 * This class handles the interactions between sub-components that help parse the source code. It contains the
 * data structures that accumulate (AST, EntityTables, RelationshipTables, Helper Stacks that keep track of tokens
 * and determine how the Deliverables are being modified). It receives a single Source Statement from the Parser and
 * initiates a pipeline for processing it: the statement is tokenized then its syntax is validated, entity nodes are
 * created and added to the AST, relationships are added.
 */
class PSubsystem {
 private:
  Deliverable* deliverable_;
  SyntaxValidator syntax_validator_;
  EntityFactory entity_factory_;

  bool valid_state = true;
  Container* current_node_;
  int current_node_type_ = -1; // -1 => no current node; 0 = procedure; 1 = while; 2 = if; 3 = else;
  stack<Container*> parent_stack_;
  stack<Statement*> follow_stack_;
  int program_counter_ = 0;

  // private methods for selfcall
  void PerformNewProcedureSteps(Procedure* procedure);
  void SetStatementObject(Statement* statement);
  void HandleIfStmt(IfEntity* if_entity);
  void HandleElseStmt(ElseEntity* else_entity);
  void HandleWhileStmt(WhileEntity* while_entity);
  void HandleAssignStmt(AssignEntity* assign_entity);
  void HandleCallStmt(CallEntity* call_entity);
  void HandleReadStmt(ReadEntity* read_entity);
  void HandlePrintStmt(PrintEntity* print_entity);

 public:
  PSubsystem() = default;

  void InitDataStructures();

  void ProcessStatement(std::string statement);

  Deliverable* GetDeliverables();
};
}

#endif //AUTOTESTER_PSUBSYSTEM_H
