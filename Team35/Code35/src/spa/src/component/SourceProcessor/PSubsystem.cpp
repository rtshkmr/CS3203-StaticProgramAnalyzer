#include <stdexcept>
#include <cassert>
#include "PSubsystem.h"
#include "Tokenizer.h"
#include "exception/SpaException.h"

using namespace psub;

void PSubsystem::InitDataStructures() {

  /// INTERNAL STATE RESET
  valid_state = true;
  current_procedure_ = nullptr;
  current_node_ = nullptr;
  current_node_type_ = NodeType::kNone;
  parent_stack_ = std::stack<Container*>();
  follow_stack_ = std::stack<Statement*>();
  block_stack_ = std::stack<Block*>();
  cluster_stack_ = std::stack<Cluster*>();
  program_counter_ = 0;

  deliverable_ = new Deliverable(); //created in heap so that can pass this object to PKB
  SyntaxValidator();
  entity_factory_ =
      EntityFactory(deliverable_->GetProcList(), deliverable_->GetVariableList(),
                    deliverable_->GetConstantList());
}

bool PSubsystem::CheckForStacksEmpty() {
  return parent_stack_.empty() && follow_stack_.empty()
          && block_stack_.empty() && cluster_stack_.empty();
}

/**
 *  Tokenizes and validates tokens (syntax validation) in a statement-level.
 * @param statement string representation of a statement
 * @return a validated vector of tokens
 * @throws SyntaxException if there's a syntax exception found during this stage
 */
std::vector<Token> PSubsystem::GetValidatedTokens(const std::string& statement) {
  if (!valid_state) {
    throw SyntaxException("Unable to process statement due to an earlier syntax error found, or closed");
  }

  std::vector<Token> tokens = Tokenizer::CreateTokens(statement);
  bool valid = SyntaxValidator::ValidateSyntax(tokens);

  if (!valid) {
    valid_state = false;
    throw SyntaxException("Invalid syntax found.");
  }
  return tokens;
}

void PSubsystem::ProcessEntityAsNewProcedure(Entity* entity) {
  if (auto* procedure = dynamic_cast<Procedure*>(entity)) {
    assert(entity->GetEntityEnum() == EntityEnum::kProcedureEntity && current_procedure_ == nullptr
               && current_node_ == nullptr && follow_stack_.empty() && parent_stack_.empty() && block_stack_.empty());
    return PerformNewProcedureSteps(procedure);
  } else {
    throw SyntaxException("Expected a procedure entity at this location but was given another type.");
  }
}

void PSubsystem::ProcessEntityAsStatement(Entity* entity) {
  //From here onwards, Entity must be a Statement type;
  if (auto* stmt = dynamic_cast<Statement*>(entity)) {
    SetStatementObject(stmt);
    StatementHandler handler = statement_handlers_[static_cast<int>(entity->GetEntityEnum())];
    (this->*handler)(entity);
  } else {
    throw SyntaxException("Expected a Statement but was given a procedure declaration.");
  }
}

/**
 * Process 1 statement at a time, and append relationship into Deliverables map.
 * @param statement Statement to be processed
 * @throws SyntaxException throws when a syntax error was found in this statement or any previous statement
 * @throws IterationOneException throws when Call statement was called
 * @throws IterationOneException throws when more than one procedure is processed.
 */
void PSubsystem::ProcessStatement(const std::string& statement) {
//  LOG (spa_logger << "\n\n\n==========================  [ENTER] ProcessStatement ======================\n\n\n");
  std::vector<Token> tokens = GetValidatedTokens(statement);
  if (tokens[0].GetTokenTag() == TokenTag::kCloseBrace) {
    return HandleCloseBrace(); // Close Brace can early return without creating entity.
  }
  Entity* entity = entity_factory_.CreateEntity(tokens);
  return (current_node_type_ == NodeType::kNone)// Only happens when not reading within a procedure.
         ? ProcessEntityAsNewProcedure(entity)
         : ProcessEntityAsStatement(entity);
}

void PSubsystem::CloseProcedureBlock() {
  if (current_node_type_ == NodeType::kProcedure && parent_stack_.empty()) {
    Block* proc_tail = Block::GetNewExitBlock();
    current_procedure_->SetBlockTail(proc_tail);

    current_node_type_ = NodeType::kNone;
    current_node_ = nullptr;
    current_procedure_ = nullptr;
    Block* last_block = block_stack_.top();
    block_stack_.pop();
    Cluster* outermost_cluster = cluster_stack_.top();
    outermost_cluster->SetClusterTag(ClusterTag::kProcedureCluster);
    if(!Block::IsExitBlock(last_block)) {
      outermost_cluster->AddChildClusterToBack(last_block);
      outermost_cluster->UpdateClusterRange();

      last_block->AddNextBlock(proc_tail);
    } else {
      Block::PatchEmptyBlocks(last_block, proc_tail);
    }

    cluster_stack_.pop(); // cluster_stack is now empty since the Procedure has been closed.
  }
}

void PSubsystem::CloseElseBlock() {
  if (current_node_type_ == NodeType::kElse) {
    parent_stack_.pop();
    follow_stack_.pop();
    Block* block_if_else_exit = Block::GetNewExitBlock(); // exit block
    Block* else_body_block = block_stack_.top();
    block_stack_.pop(); //pop the else body block
    Block* if_body_block = block_stack_.top();
    block_stack_.pop(); //pop the if_body block

    Block* if_cond_block = block_stack_.top();

    else_body_block->SetClusterTag(ClusterTag::kElseBody);
    if_body_block->SetClusterTag(ClusterTag::kIfBody);
    if_cond_block->SetClusterTag(ClusterTag::kIfCond);

    if (Block::IsExitBlock(else_body_block)) {
      Block::PatchEmptyBlocks(else_body_block, block_if_else_exit);
    } else {
      else_body_block->AddNextBlock(block_if_else_exit);
    }

    if (Block::IsExitBlock(if_body_block)) {
      Block::PatchEmptyBlocks(if_body_block, block_if_else_exit);
    } else {
      if_body_block->AddNextBlock(block_if_else_exit);
    }

    if_cond_block->UpdateClusterRange();

    block_stack_.pop(); //pop the if_cond block
    block_stack_.push(block_if_else_exit);
    bool is_currently_in_nested_cluster = cluster_stack_.size() > 1;
    assert(is_currently_in_nested_cluster);
    Cluster* else_body_cluster = cluster_stack_.top();
    if (else_body_block->size() > 0) {
      else_body_cluster->AddChildClusterToBack(else_body_block); //append anything else
    }
    else_body_cluster->UpdateClusterRange();
    cluster_stack_.pop(); // pops out the else_body_cluster
    Cluster* if_cluster = cluster_stack_.top();
    if_cluster->UpdateClusterRange();
    assert(if_cluster->GetNestedClusters().size() == 3);
    cluster_stack_.pop(); // pops out the if_cluster
    assert(!cluster_stack_.empty());
    Cluster* outer_cluster = cluster_stack_.top();
    outer_cluster->AddChildClusterToBack(if_cluster);
    outer_cluster->UpdateClusterRange();
  }
}

void PSubsystem::CloseWhileBlock() {
  auto* while_body_block = dynamic_cast<Block*>(block_stack_.top());
  block_stack_.pop(); // link the last stmt to the while_cond_block block, and pop it.
  //todo: change from Block* to ConditionalBlock*
  auto* while_cond_block = dynamic_cast<Block*>(block_stack_.top());
  while_cond_block->SetClusterTag(ClusterTag::kWhileCond);
  assert(while_cond_block);

  bool is_currently_in_nested_cluster = cluster_stack_.size() > 1;
  assert(is_currently_in_nested_cluster);
  // add to cluster here:
  Cluster* while_body_cluster = cluster_stack_.top();

  if (while_body_block->size() > 0) {
    while_body_cluster->AddChildClusterToBack(while_body_block); //add only non empty tails
  }
  while_body_cluster->UpdateClusterRange();

  cluster_stack_.pop();
  Cluster* while_cluster = cluster_stack_.top();
  while_cluster->UpdateClusterRange();
  assert(while_cluster->GetNestedClusters().size() == 2);
  cluster_stack_.pop();
  assert(!cluster_stack_.empty());
  Cluster* outer_cluster = cluster_stack_.top();
  outer_cluster->AddChildClusterToBack(while_cluster);
  outer_cluster->UpdateClusterRange();

  Block* block_while_exit = Block::GetNewExitBlock();

  if (Block::IsExitBlock(while_body_block)) {
    Block::PatchEmptyBlocks(while_body_block, while_cond_block);
  } else {
    while_body_block->AddNextBlock(while_cond_block); // loop back to cond
  }

  while_cond_block->AddNextBlock(block_while_exit); // cond point to exit
  block_stack_.pop(); //pop the while_cond_block block
  block_stack_.push(block_while_exit);

}

/**
 * Processes the outer node when handling the closing of a brace by either handling the outer node as a procedure
 * or as a non-procedure code container type.
 */
void PSubsystem::ProcessParentNode() {
  Container* current_nest = parent_stack_.top();
  parent_stack_.pop();
  // get to outer node and process:
  current_node_ = dynamic_cast<Statement*>(current_nest)->GetParentNode();
  parent_stack_.empty() ? ProcessParentNodeAsProcedure() : ProcessParentNodeType(current_nest);
}

void PSubsystem::ProcessParentNodeAsProcedure() {
  current_node_type_ = NodeType::kProcedure;
  current_node_ = current_procedure_;
}

void PSubsystem::ProcessParentNodeType(Container* current_nest) {
  current_nest = parent_stack_.top();
  if (dynamic_cast<WhileEntity*>(current_nest)) {
    current_node_type_ = NodeType::kWhile;
  } else if (dynamic_cast<IfEntity*>(current_nest)) {
    current_node_type_ = NodeType::kIf;
  } else if (auto* else_entity = dynamic_cast<ElseEntity*>(current_nest)) {
    current_node_type_ = NodeType::kElse;

    //dirty way to get the original if-statement (should be 2nd highest)
    parent_stack_.pop(); //pop this else;
    current_node_ = parent_stack_.top();
    assert (dynamic_cast<IfEntity*>(current_node_));
    parent_stack_.push(else_entity);
  } else {
    throw std::invalid_argument("[ERROR] Retrace error. There should not be any other types ");
  }
}

/**
 * Handles the effects of encountering a close brace character. Here are the following cases based on the current node: \n
 * IF node: do nothing (compensate when handling else block) \n
 *
 *
 * Precondition: If it's a close brace for an entire procedure, the parent stack should be empty  else for other node
 * types, the parent stack should be non-empty (since we expect the statement to be nested within some kind of cluster.
 */
void PSubsystem::HandleCloseBrace() {
  //enforce that every stmtList must have at least 1 statement
  if (current_node_->GetStatementList()->empty()) {
    throw SyntaxException("Empty statement list found"); //TODO check for ELSE.empty_stmtList;
  }
  
  //assertion: case 1: close brace for procedure --> type = 0 & parent_stack = empty
  //           case 2: close brace for others    --> type > 0 & parent_stack ! empty
  assert((current_node_type_ != NodeType::kProcedure && !parent_stack_.empty()) ||
      current_node_type_ == NodeType::kProcedure && parent_stack_.empty());

  if (current_node_type_ == NodeType::kIf) {
    if(!Block::IsExitBlock(block_stack_.top())) {
      cluster_stack_.top()->AddChildClusterToBack(block_stack_.top());
    }
    cluster_stack_.top()->UpdateClusterRange();
    cluster_stack_.pop();
    return; // do not pop anything in if-close brace. pop 2 when finishing else.
  }

  follow_stack_.pop(); //this is allowed because any stmtList must be 1..* statements. so no condition for if (...) { }

  if (current_node_type_ == NodeType::kProcedure) {
    CloseProcedureBlock();
    if (!CheckForStacksEmpty()) {
      throw SyntaxException("Difficulty in processing procedure. This could be caused by mismatch braces.");
    }
  } else {
    if (current_node_type_ == NodeType::kElse) { //double pop for else clause
      CloseElseBlock();
    } else if (current_node_type_ == NodeType::kWhile) {
      CloseWhileBlock();
    }
    ProcessParentNode();
  }
}

/**
 * For a new procedure, does initialisations on internal state: \n
 * Cluster root, block root, cluster and block stacks, procedure, node and node_type.
 * @return
 */
void PSubsystem::InitInternalState(Procedure* procedure) {
  current_procedure_ = procedure;
  current_node_ = procedure;
  current_node_type_ = NodeType::kProcedure;
  auto* cluster_root = new Cluster(); // the outer procedure
  cluster_root->SetParentCluster(nullptr);
  auto* block_root = new Block();
  block_stack_.push(block_root);
  cluster_stack_.push(cluster_root);
  procedure->SetBlockRoot(block_root);
  procedure->SetClusterRoot(cluster_root);
}

/**
 * Given a new procedure, inits the cluster root, inits program if this is the first
 * procedure encountered so far, and finally, throws an error if this procedure name is a duplicate
 * declaration of procedure.
 * @param procedure procedure entity created from the line that represents the start of a procedure
 */
void PSubsystem::PerformNewProcedureSteps(Procedure* procedure) {
  InitInternalState(procedure);

  if (deliverable_->GetProgram() == nullptr) {
    auto* program = new Program(procedure);
    deliverable_->SetProgram(program);
  } else {
    // throw error if this name is duplicated
    for (auto const& proc: * deliverable_->GetProgram()->GetProcedureList()) {
      if (* proc->GetName() == * procedure->GetName()) { // uses the overloaded ==
        throw SemanticException("Encountered 2 procedures with the same name.");
      }
    }
    deliverable_->GetProgram()->AddProcedure(procedure);
  }
}

void PSubsystem::SetStatementObject(Statement* statement) {
  statement->SetProcedureNode(current_procedure_);

  if (dynamic_cast<ElseEntity*>(statement) != nullptr)
    return;

  program_counter_++;
  auto* statement_number = new StatementNumber(program_counter_);
  statement->SetStatementNumber(statement_number);
  deliverable_->AddStatement(statement);
  block_stack_.top()->AddStmt(StatementNumber(program_counter_));

  bool new_else = false;
  //to check if adding stmt to Else block
  if (current_node_type_ == NodeType::kElse) {
    auto* if_entity = dynamic_cast<IfEntity*>(current_node_);
    assert (if_entity != nullptr);
    if_entity->GetElseStmtList()->push_back(statement);

    if (if_entity->GetElseStmtList()->size() == 1)
      new_else = true;
  } else {
    current_node_->AddStatement(statement);
  }

  if (!parent_stack_.empty()) {
    assert(current_node_type_ != NodeType::kNone);
    statement->SetParentNode(parent_stack_.top());
    deliverable_->AddParentRelationship(dynamic_cast<Statement*>(current_node_), statement);
  }

  if (current_node_type_ != NodeType::kElse && current_node_->GetStatementList()->size() == 1
      || // 1 because this is newly added in Line curr - 13
          (current_node_type_ == NodeType::kElse && new_else)) {
    //just entered a stack, follow nothing.
    follow_stack_.push(statement);
  } else {
    statement->SetBeforeNode(follow_stack_.top());
    deliverable_->AddFollowRelationship(follow_stack_.top(), statement);
    follow_stack_.pop();
    follow_stack_.push(statement);
  }

}

void PSubsystem::HandleError(Entity* entity) {
  throw SyntaxException("Statement is not cast into objects. Check EntityFactory::CreateEntity");
}

/**
 * Updates PSubsystem's internal state to reflect parent-child relationships,
 * sets up conditional and body blocks and then encapsulates them within a new cluster.
 * @param entity
 */
void PSubsystem::HandleIfStmt(Entity* entity) {
  auto* if_entity = dynamic_cast<IfEntity*>(entity);
  assert(if_entity);
  deliverable_->AddIfEntity(if_entity);
  parent_stack_.push(if_entity);
  current_node_type_ = NodeType::kIf;
  current_node_ = if_entity;
  auto* conditional_statement = dynamic_cast<Statement*>(entity);
  ConditionalBlock* block_if_cond = CreateConditionalBlock(conditional_statement);
  block_if_cond->SetClusterTag(ClusterTag::kIfCond);
  CreateBodyBlock(block_if_cond);
  AddControlVariableRelationships(if_entity->GetControlVariables());
  auto* if_cluster = new Cluster();
  if_cluster->SetClusterTag(ClusterTag::kIfCluster);
  cluster_stack_.push(if_cluster);
  if_cluster->AddChildClusterToFront(block_if_cond);
  auto* if_body_cluster = new Cluster();
  if_body_cluster->SetClusterTag(ClusterTag::kIfBody);
  if_cluster->AddChildClusterToBack(if_body_cluster);
  cluster_stack_.push(if_body_cluster);

}

/**
 * Updates internal state by linking the If and Else entities.
 * @param entity
 */
void PSubsystem::HandleElseStmt(Entity* entity) {
  auto* else_entity = dynamic_cast<ElseEntity*>(entity);
  assert(else_entity);
  auto* if_entity = dynamic_cast<IfEntity*>(parent_stack_.top());
  if (if_entity == nullptr) {
    throw SyntaxException("Encountered Else statement without If construct");
  }
  parent_stack_.push(else_entity);
  if_entity->SetElseEntity(else_entity);
  current_node_type_ = NodeType::kElse;
  current_node_ = if_entity;
  CreateBodyBlock();
  auto* else_body_cluster = new Cluster();
  else_body_cluster->SetClusterTag(ClusterTag::kElseBody);
  cluster_stack_.top()->AddChildClusterToBack(else_body_cluster);
  cluster_stack_.push(else_body_cluster);
}

void PSubsystem::HandleWhileStmt(Entity* entity) {
  auto* while_entity = dynamic_cast<WhileEntity*>(entity);
  assert(while_entity);
  deliverable_->AddWhileEntity(while_entity);
  parent_stack_.push(while_entity);
  current_node_type_ = NodeType::kWhile;
  current_node_ = while_entity;

  auto* conditional_statement = dynamic_cast<Statement*>(entity);
  ConditionalBlock* block_while_cond = CreateConditionalBlock(conditional_statement);
  block_while_cond->SetClusterTag(ClusterTag::kWhileCond);
  block_while_cond->isWhile = true;
  CreateBodyBlock(block_while_cond);
  AddControlVariableRelationships(while_entity->GetControlVariables());
  auto* while_cluster = new Cluster();
  while_cluster->SetClusterTag(ClusterTag::kWhileCluster);
  cluster_stack_.push(while_cluster);
  while_cluster->AddChildClusterToFront(block_while_cond);
  auto* while_body_cluster = new Cluster();
  while_body_cluster->SetClusterTag(ClusterTag::kWhileBody);
  while_cluster->AddChildClusterToBack(while_body_cluster);
  cluster_stack_.push(while_body_cluster);
}

/**
 *
* Abstracted logic for handling the blocks if and while blocks:
 * @param entity
 * @param node_type
 * @return pointer to the newly created conditional block
 */
ConditionalBlock* PSubsystem::CreateConditionalBlock(Statement* conditional_statement) {
  int statement_num = conditional_statement->GetStatementNumber();
  ConditionalBlock* conditional_block;
  Block* block_before_cond = block_stack_.top();
  if (block_before_cond->size() > 1) {
    block_before_cond->RemoveStmt(StatementNumber(statement_num));
    conditional_block = new ConditionalBlock();
    conditional_block->AddStmt(StatementNumber(statement_num));
    block_before_cond->AddNextBlock(conditional_block);
    bool prior_block_is_not_while = !dynamic_cast<Block*>(block_before_cond)->isWhile;
    if (prior_block_is_not_while) {
      cluster_stack_.top()->AddChildClusterToBack(block_before_cond);
      block_stack_.pop(); // pop the previous progline if it isnt while (no loopback to care)
    }
    block_stack_.push(static_cast<Block* const>(conditional_block));
  } else {
    conditional_block = static_cast<ConditionalBlock*>(block_stack_.top());
  }
  return conditional_block;
}

/**
* Abstracted logic for handling the blocks representing the body within if and while
 * @param conditional_block the conditional block that shall be linked to this newly created body block
 */
BodyBlock* PSubsystem::CreateBodyBlock(ConditionalBlock* conditional_block) {
  auto* body_block = new BodyBlock();
  conditional_block->AddNextBlock(dynamic_cast<Block*>(body_block));
  block_stack_.push(dynamic_cast<Block*>(body_block));
  return body_block;
}

/**
 *  Creates the Else block's body
 */
BodyBlock* PSubsystem::CreateBodyBlock() {
  auto* block_if_body = dynamic_cast<Block*>(block_stack_.top());
  block_stack_.pop(); //pop so that the if_cond can perform next_block map to else block
  auto* block_else_body = new BodyBlock();
  Block* block_if_cond = block_stack_.top();
  block_if_cond->AddNextBlock(block_else_body);
  block_stack_.push(block_if_body);
  block_stack_.push(block_else_body);
  return block_else_body;
}

void PSubsystem::AddControlVariableRelationships(const std::vector<Variable*>& control_variables) {
  for (Variable* v: control_variables) {
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);
  }
}

void PSubsystem::HandleAssignStmt(Entity* entity) {
  auto* assign_entity = dynamic_cast<AssignEntity*>(entity);
  assert(assign_entity);
  deliverable_->AddAssignEntity(assign_entity);
  deliverable_->AddModifiesRelationship(assign_entity, assign_entity->GetVariableObj());
  deliverable_->AddModifiesRelationship(current_procedure_, assign_entity->GetVariableObj());  //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddModifiesRelationship(current_node_, assign_entity->GetVariableObj());  //container level

  // todo: add these variables into the respective sets in cluster/block
  for (Variable* v: assign_entity->GetExprVariables()) {
    deliverable_->AddUsesRelationship(assign_entity, v);
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);   //container level
  }
}

void PSubsystem::HandleCallStmt(Entity* entity) {
  auto* call_entity = dynamic_cast<CallEntity*>(entity);
  assert(call_entity);
  deliverable_->AddCallEntity(call_entity);
  deliverable_->AddCallsRelationship(current_procedure_, call_entity->GetProcedure());
}

void PSubsystem::HandlePrintStmt(Entity* entity) {
  auto* print_entity = dynamic_cast<PrintEntity*>(entity);
  assert(print_entity);
  deliverable_->AddPrintEntity(print_entity);
  deliverable_->AddUsesRelationship(print_entity, print_entity->GetVariable());
  deliverable_->AddUsesRelationship(current_procedure_, print_entity->GetVariable()); //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddUsesRelationship(current_node_, print_entity->GetVariable());   //container level
}

void PSubsystem::HandleReadStmt(Entity* entity) {
  auto* read_entity = dynamic_cast<ReadEntity*>(entity);
  assert(read_entity);
  deliverable_->AddReadEntity(read_entity);
  deliverable_->AddModifiesRelationship(read_entity, read_entity->GetVariable());
  deliverable_->AddModifiesRelationship(current_procedure_, read_entity->GetVariable()); //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddModifiesRelationship(current_node_, read_entity->GetVariable());  //container level
}

void PSubsystem::CheckForIfElseValidity() {
  for (auto const& i: * deliverable_->GetIfList()) {
    if (i->GetElseEntity() == nullptr) {
      throw SyntaxException("Encountered If statement without Else construct");
    }
  }
}

void PSubsystem::CheckForExistingProcedure() {
  std::list<Procedure*> program_proclist_dup(* deliverable_->GetProgram()->GetProcedureList());
  std::list<Procedure*>* del_proclist = deliverable_->GetProcList();

  program_proclist_dup.sort([](Procedure* a, Procedure* b) { return * a->GetName() < * b->GetName(); });
  del_proclist->sort([](Procedure* a, Procedure* b) { return * a->GetName() < * b->GetName(); });
  bool equal =
      std::equal(program_proclist_dup.begin(), program_proclist_dup.end(), del_proclist->begin(), del_proclist->end());

  if (!equal) {
    throw SemanticException("A call is made to unreferenced procedure");
  }
}

/**
 * Verifies the validity of the finial state.
 */
void PSubsystem::FiniStateCheck() {
  if (deliverable_->GetStatementList()->empty()) {
    throw SyntaxException("A blank simple file is encountered.");
  }

  CheckForIfElseValidity();
  CheckForExistingProcedure();
  if (!CheckForStacksEmpty()) {
    throw SyntaxException("Difficulty in processing simple file. This could be caused by mismatch braces.");
  }
}

Deliverable* PSubsystem::GetDeliverables() {
  FiniStateCheck();
  valid_state = false; //to prevent further processing.
  return deliverable_;
}


