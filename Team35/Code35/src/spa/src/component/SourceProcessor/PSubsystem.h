#ifndef AUTOTESTER_PSUBSYSTEM_H
#define AUTOTESTER_PSUBSYSTEM_H

#include <string>
#include <stack>
#include <datatype/Deliverable.h>
#include <model/CFG.h>
#include "SyntaxValidator.h"
#include "EntityFactory.h"

/**
 * This namespace provides a scope for PSubsystem related declarations.
 */
namespace psub {

enum class NodeType {
  kNone = -1,
  kProcedure = 0,
  kWhile = 1,
  kIf = 2,
  kElse = 3
};

/**
 * This class handles the interactions between sub-components that help parse the source code. It contains the
 * data structures that accumulate (AST, EntityTables, RelationshipTables, Helper Stacks that keep track of tokens
 * and determine how the Deliverables are being modified). It receives a single Source Statement from the Parser and
 * initiates a pipeline for processing it: the statement is tokenized then its syntax is validated, entity nodes are
 * created and added to the AST, relationships are added.
 */
class PSubsystem {
  typedef void (PSubsystem::*StatementHandler)(Entity*);

 private:
  std::vector<StatementHandler> statement_handlers_ = {& PSubsystem::HandleError, & PSubsystem::HandleIfStmt,
                                                       & PSubsystem::HandleWhileStmt, & PSubsystem::HandleAssignStmt,
                                                       & PSubsystem::HandleCallStmt, & PSubsystem::HandlePrintStmt,
                                                       & PSubsystem::HandleReadStmt, & PSubsystem::HandleElseStmt};
  Procedure* current_procedure_;
  Deliverable* deliverable_;
  SyntaxValidator syntax_validator_;
  EntityFactory entity_factory_;

  bool valid_state = true;
  Container* current_node_;
  NodeType current_node_type_ = NodeType::kNone; // -1 => no current node; 0 = procedure; 1 = while; 2 = if; 3 = else;
  std::stack<Container*> parent_stack_;
  std::stack<Statement*> follow_stack_;
  /// ISSUE 1: Trying to add Cluster as a Block.
  std::stack<Cluster*> block_stack_;
  int program_counter_ = 0;

  // private methods for selfcall
  void PerformNewProcedureSteps(Procedure* procedure);
  void SetStatementObject(Statement* statement);
  void HandleCloseBrace();
  void HandleError(Entity* entity);
  void HandleIfStmt(Entity* entity);
  void HandleElseStmt(Entity* entity);
  void HandleWhileStmt(Entity* entity);
  void HandleAssignStmt(Entity* entity);
  void HandleCallStmt(Entity* entity);
  void HandleReadStmt(Entity* entity);
  void HandlePrintStmt(Entity* entity);
  ConditionalBlock* CreateConditionalBlock(Statement* conditional_statement);
  void CreateBodyBlock(ConditionalBlock* conditional_block);
  void CreateBodyBlock(); // for else body
  void AddControlVariableRelationships(const std::vector<Variable*>& control_variables);
  void CheckForIfElseValidity();
  void CheckForExistingProcedure();
  void CloseIfBlock();
  void CloseProcedureBlock();
  void CloseElseBlock(Container* current_nest);
  void CloseWhileBlock();
  void ProcessOuterNodeAsProcedure();
  void ProcessOuterParentNode(Container* current_nest);
  void ProcessOuterNodeType(Container* current_nest);
  std::vector<Token> GetValidatedTokens(const std::string& statement);
 public:
  PSubsystem() = default;

  void InitDataStructures();

  void ProcessStatement(const std::string& statement);

  Deliverable* GetDeliverables();

  void ProcessEntityAsNewProcedure(Entity* entity);
  void ProcessEntityAsStatement(Entity* entityObj);
};
}

#endif //AUTOTESTER_PSUBSYSTEM_H
