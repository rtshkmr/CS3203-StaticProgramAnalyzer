#include <stdexcept>
#include <cassert>
#include "PSubsystem.h"
#include "Tokenizer.h"
#include "exception/SyntaxException.h"

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
  syntax_validator_ = SyntaxValidator();
  entity_factory_ =
      EntityFactory(deliverable_->GetProcList(), deliverable_->GetVariableList(),
                    deliverable_->GetConstantValueList());
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
  if (Procedure* procedure = dynamic_cast<Procedure*>(entity)) {
    assert(entity->GetEntityEnum() == EntityEnum::kProcedureEntity && current_procedure_ == nullptr
               && current_node_ == nullptr && follow_stack_.empty() && parent_stack_.empty() && block_stack_.empty());
    return PerformNewProcedureSteps(procedure);
  } else {
    throw SyntaxException("Expected a procedure entity at this location but was given another type.");
  }
}

void PSubsystem::ProcessEntityAsStatement(Entity* entity) {
  //From here onwards, Entity must be a Statement type;
  if (Statement* stmt = dynamic_cast<Statement*>(entity)) {
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

/**
 * Handles the closing of if-blocks. Currently has no effects, method has been abstracted out to keep abstraction levels
 * in the caller function equal.
 */
void PSubsystem::CloseIfBlock() {
  if (current_node_type_ == NodeType::kIf) {
    // todo: probably have to update cluster's fields
    int x =
        1; // todo: get the if-body from the top of block stack, if cond from 2nd pos of block stack and the outer cluster from the cluster stack top
//    BodyBlock* if_body = block_stack_.top();
//    block_stack_.pop();
//    ConditionalBlock* if_cond = block_stack_.top();
    return;// do not pop anything in if-close brace. pop 2 when finishing else.
  }
}

void PSubsystem::CloseProcedureBlock() {
  if (current_node_type_ == NodeType::kProcedure && parent_stack_.empty()) {
    const Cluster* assigned_cluster_root = current_procedure_->GetClusterRoot();
    current_node_type_ = NodeType::kNone;
    current_node_ = nullptr;
    current_procedure_ = nullptr;
    Block* procedure_block = block_stack_.top();
    block_stack_.pop();
    assert(block_stack_.empty()); // FIXME this is the failing assertion now
    bool is_currently_in_outermost_cluster = cluster_stack_.size() == 1;
    assert(is_currently_in_outermost_cluster);

    // put the block root into the
    Cluster*outermost_cluster = cluster_stack_.top();
    if(!Block::IsExitBlock(procedure_block)) {
      outermost_cluster->AddChildCluster(procedure_block);
    }
    cluster_stack_.pop(); // cluster_stack is now empty since the Procedure has been closed.
    bool last_popped_equals_cluster_root = outermost_cluster == assigned_cluster_root;
    assert(last_popped_equals_cluster_root);
    outermost_cluster->UpdateClusterRange();
  }
}

void PSubsystem::CloseElseBlock() {
  if (current_node_type_ == NodeType::kElse) {
    parent_stack_.pop();
    follow_stack_.pop();
    Block* block_if_else_exit = Block::GetNewExitBlock(); // exit block, QQ: is this really needed!?
    Block* else_body_block = block_stack_.top(); // FIXME:
    else_body_block->next_blocks_.insert(block_if_else_exit); // else body block
    block_stack_.pop(); //pop the else body block
    Block* if_body_block = block_stack_.top();
    if_body_block->next_blocks_.insert(block_if_else_exit);
    block_stack_.pop(); //pop the if_body block
    Block* if_cond_block = block_stack_.top();
    if_cond_block->next_blocks_.insert(else_body_block);
    block_stack_.pop(); //pop the if_cond block
    block_stack_.push(block_if_else_exit);
    bool is_currently_in_nested_cluster = cluster_stack_.size() > 1;
    assert(is_currently_in_nested_cluster);
    Cluster* if_cluster = cluster_stack_.top();
    ///  add to if_cluster only if the if_cluster is currently empty.
    /// guarantee: There will be at most be 3 nested clusters in if cluster (ifcond, ifbody, elsebody):
    if(if_cluster->GetNestedClusters().empty()) {
      if_cluster->AddChildCluster(if_cond_block);
      if_cluster->AddChildCluster(if_body_block);
      if_cluster->AddChildCluster(else_body_block); // this is ok because there is at least 1 stmt
      if_cluster->UpdateClusterRange();
    } else {
//      if_cluster->AddChildCluster(if_cond_block);
      if_cluster->nested_clusters_.push_front(if_cond_block);
      if (else_body_block->size() > 0) {
        if_cluster->AddChildCluster(else_body_block); //append anything else FIXME -> bug might occur in w{ aaa if{} a if{} a } where middle a is missing.
      }
      if_cond_block->SetParentCluster(if_cluster);
      if_cluster->UpdateClusterRange();
      int x = 1;
    }
    cluster_stack_.pop(); // pops out the if_cluster
    assert(!cluster_stack_.empty());
    Cluster* outer_cluster = cluster_stack_.top();
    outer_cluster->AddChildCluster(if_cluster);
  }
}

// todo: handle while stmt, where is the while_body_block!?
void PSubsystem::CloseWhileBlock() {
  Block* while_body_block = dynamic_cast<Block*>(block_stack_.top()); //QQ is this a statement or is this the whole while block's body?
  block_stack_.pop(); // link the last stmt to the while_cond_block block, and pop it.
  //todo: change from Block* to ConditionalBlock*
  Block* while_cond_block = dynamic_cast<Block*>(block_stack_.top());
  assert(while_cond_block);

  bool is_currently_in_nested_cluster = cluster_stack_.size() > 1;
  assert(is_currently_in_nested_cluster);
  // add to cluster here:
  Cluster* while_cluster = cluster_stack_.top();
  while_cluster->nested_clusters_.push_front(while_cond_block);

  if (while_body_block->size() > 0) {
    while_cluster->AddChildCluster(while_body_block); //add only non empty tails
  }
  while_cluster->UpdateClusterRange(); //FIXME bug - w(cond) { .. not_accounted .. if() .. not_accounted .. if() ..}

  cluster_stack_.pop();
  assert(!cluster_stack_.empty());
  Cluster* outer_cluster = cluster_stack_.top();
  outer_cluster->AddChildCluster(while_cluster);
  outer_cluster->UpdateClusterRange();

  while_body_block->next_blocks_.insert(while_cond_block); // loop back to cond
  Block* block_while_exit = Block::GetNewExitBlock();
  while_cond_block->next_blocks_.insert(block_while_exit); // cond point to exit
  block_stack_.pop(); //pop the while_cond_block block
  block_stack_.push(block_while_exit);
}

void PSubsystem::ProcessOuterParentNode() {
  /// FOR ISSUE 3 -> current_next PLACEMENT IS CRUCIAL FOR IF-STATEMENT, because it pops the else first
  Container* current_nest = parent_stack_.top();
  parent_stack_.pop(); // QQ: pops out the if block?
  // get to outer node and process:
  current_node_ = dynamic_cast<Statement*>(current_nest)->GetParentNode();
  parent_stack_.empty() ? ProcessOuterNodeAsProcedure() : ProcessOuterNodeType(current_nest);
}

void PSubsystem::ProcessOuterNodeAsProcedure() {
  current_node_type_ = NodeType::kProcedure;
  current_node_ = current_procedure_;
}

void PSubsystem::ProcessOuterNodeType(Container* current_nest) {
  current_nest = parent_stack_.top();
  if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(current_nest)) {
    current_node_type_ = NodeType::kWhile;
  } else if (IfEntity* if_entity = dynamic_cast<IfEntity*>(current_nest)) {
    current_node_type_ = NodeType::kIf;
  } else if (ElseEntity* else_entity = dynamic_cast<ElseEntity*>(current_nest)) {
    current_node_type_ = NodeType::kElse;

    //dirty way to get the original if-statement (should be 2nd highest)
    parent_stack_.pop(); //pop this else;
    current_node_ = parent_stack_.top();
    assert (dynamic_cast<IfEntity*>(current_node_) != nullptr);
    parent_stack_.push(else_entity);
  } else {
    throw std::invalid_argument("[ERROR] Retrace error. There should not be any other types ");
  }
}

/**
 * Handles the effects of encountering a close brace character. Here are the following cases based on the current node: \n
 * IF node: do nothing (compensate when handling else block) \n
 * Procedure node and Empty parent stack (ready to close off the procedure block)
 *
 *
 * Precondition: If it's a close brace for an entire procedure, the parent stack should be empty  else for other node
 * types, the parent stack should be non-empty (since we expect the statement to be nested within some kind of cluster.
 */
void PSubsystem::HandleCloseBrace() {
  //assertion: case 1: close brace for procedure --> type = 0 & parent_stack = empty
  //           case 2: close brace for others    --> type > 0 & parent_stack ! empty
  assert((current_node_type_ != NodeType::kProcedure && !parent_stack_.empty()) ||
      current_node_type_ == NodeType::kProcedure && parent_stack_.empty());

  if (current_node_type_ == NodeType::kIf) {
    CloseIfBlock();
    return; // do not pop anything in if-close brace. pop 2 when finishing else.
  }

  follow_stack_.pop(); //this is allowed because any stmtList must be 1..* statements. so no condition for if (...) { }

  if (current_node_type_ == NodeType::kProcedure) {
    assert(parent_stack_.empty());
    CloseProcedureBlock();
  } else {
    // parent_stack_.pop();
    if (current_node_type_ == NodeType::kElse) { //double pop for else clause
      CloseElseBlock();
    } else if (current_node_type_ == NodeType::kWhile) {
      CloseWhileBlock();
    }
    ProcessOuterParentNode();
  }
}

/**
 * For a new procedure, does initialisations on Cluster and its nested clusters.
 * Every procedure has a block_root and a cluster_root. The block_root will actually be the first element in the
 * nested_clusters list within the cluster_root
 * @return
 */
void PSubsystem::InitRootClusterAndBlock(Procedure* procedure) {
  Cluster* cluster_root = new Cluster(); // the outer procedure
  cluster_root->SetParentCluster(nullptr);
  Block* block_root = new Block();
//  cluster_root->AddChildCluster(block_root);
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
  current_procedure_ = procedure;
  current_node_ = procedure;
  current_node_type_ = NodeType::kProcedure;
  InitRootClusterAndBlock(procedure);

  if (deliverable_->GetProgram() == nullptr) {
    Program* program = new Program(procedure);
    deliverable_->SetProgram(program);
  } else {
    // throw error if this name is duplicated
    for (auto const& proc: * deliverable_->GetProgram()->GetProcedureList()) {
      if (* proc->GetName() == * procedure->GetName()) { // uses the overloaded ==
        throw SyntaxException("Encountered 2 procedures with the same name.");
      }
    }
    deliverable_->GetProgram()->AddProcedure(procedure);
  }
}

// todo: rename function since this isn't a setter, suggestion --> ProcessStatementObject since it's doing a lot of linking of things...
void PSubsystem::SetStatementObject(Statement* statement) {
  if (dynamic_cast<ElseEntity*>(statement) != nullptr)
    return;

  program_counter_++;
  StatementNumber* statement_number = new StatementNumber(program_counter_);
  statement->SetStatementNumber(statement_number);
  deliverable_->AddStatement(statement);
  block_stack_.top()->AddStmt(StatementNumber(program_counter_));

  bool new_else = false; // QQ: what does "new_else" mean?
  //to check if adding stmt to Else block
  if (current_node_type_ == NodeType::kElse) {
    IfEntity* if_entity = dynamic_cast<IfEntity*>(current_node_);
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
  IfEntity* if_entity = dynamic_cast<IfEntity*>(entity);
  assert(if_entity);
  deliverable_->AddIfEntity(if_entity);
  parent_stack_.push(if_entity);
  current_node_type_ = NodeType::kIf;
  current_node_ = if_entity;
  Statement* conditional_statement = dynamic_cast<Statement*>(entity);
  ConditionalBlock* block_if_cond = CreateConditionalBlock(conditional_statement);
  BodyBlock* block_if_body = CreateBodyBlock(block_if_cond);
  // todo: make it into add control var + const later
  AddControlVariableRelationships(if_entity->GetControlVariables());
//  CreateNewNestedCluster(block_if_cond, block_if_body);
  Cluster* if_cluster = new Cluster();
  // fixme: suggestion: push nest the current statement into cluster because it's a condition block
  cluster_stack_.push(if_cluster);
}

/**
 * Updates internal state by linking the If and Else entities.
 * @param entity
 */
void PSubsystem::HandleElseStmt(Entity* entity) {
  ElseEntity* else_entity = dynamic_cast<ElseEntity*>(entity);
  assert(else_entity);
  IfEntity* if_entity = dynamic_cast<IfEntity*>(parent_stack_.top());
  if (if_entity == nullptr) {
    //If assertion failed, Else did not follow If
    throw SyntaxException("Encountered Else statement without If construct");
  }
  parent_stack_.push(else_entity);
  if_entity->SetElseEntity(else_entity);
  current_node_type_ = NodeType::kElse;
  current_node_ = if_entity;
  BodyBlock* block_else_body = CreateBodyBlock();
//  UpdateClusterWithElseBlock(block_else_body);
}

void PSubsystem::HandleWhileStmt(Entity* entity) {
  WhileEntity* while_entity = dynamic_cast<WhileEntity*>(entity);
  assert(while_entity);
  deliverable_->AddWhileEntity(while_entity);
  parent_stack_.push(while_entity);
  current_node_type_ = NodeType::kWhile;
  current_node_ = while_entity;

  Statement* conditional_statement = dynamic_cast<Statement*>(entity);
  ConditionalBlock* block_while_cond = CreateConditionalBlock(conditional_statement);
  block_while_cond->isWhile = true;
  BodyBlock* block_while_body = CreateBodyBlock(block_while_cond);
  AddControlVariableRelationships(while_entity->GetControlVariables());
  Cluster* while_cluster = new Cluster();
  cluster_stack_.push(while_cluster);
//  CreateNewNestedCluster(block_while_cond, block_while_body);
}

/**
 *
* Abstracted logic for handling the blocks if and while blocks:
 * @param entity
 * @param node_type
 * @return pointer to the newly created conditional block
 */
ConditionalBlock* PSubsystem::CreateConditionalBlock(Statement* conditional_statement) {
  int statement_num = conditional_statement->GetStatementNumber()->GetNum();
  // HANDLE THE CONDITION
  ConditionalBlock* conditional_block;
  // remove the stmtNumber from previous block and add it to the cond block if size > 1
  if (block_stack_.top()->size() > 1) {
    block_stack_.top()->RemoveStmt(StatementNumber(statement_num));
    conditional_block = new ConditionalBlock();
    conditional_block->AddStmt(StatementNumber(statement_num));
//    block_stack_.top()->GetNextBlocks().insert(conditional_block);
    block_stack_.top()->next_blocks_.insert(conditional_block);
    bool block_is_not_while = !dynamic_cast<Block*>(block_stack_.top())->isWhile;
    if (block_is_not_while) {
      block_stack_.pop(); // pop the previous progline if it isnt while (no loopback to care)
    }
    block_stack_.push(static_cast<Block* const>(conditional_block));
  } else {
    // QQ: need to convert this to dynamic_cast by adding a virtual destructor as WeiJie advised?
    conditional_block = static_cast<ConditionalBlock*>(block_stack_.top());
  }
  return conditional_block;
}

/**
* Abstracted logic for handling the blocks representing the body within if and while
 * @param conditional_block the conditional block that shall be linked to this newly created body block
 */
BodyBlock* PSubsystem::CreateBodyBlock(ConditionalBlock* conditional_block) {
  BodyBlock* body_block = new BodyBlock();
  conditional_block->next_blocks_.insert(dynamic_cast<Block*>(body_block));
  block_stack_.push(dynamic_cast<Block*>(body_block));
  return body_block;
}

/**
 *  Overloaded function to create Else block's body
 */
BodyBlock* PSubsystem::CreateBodyBlock() {
  Block* block_if_body = dynamic_cast<Block*>(block_stack_.top());
  block_stack_.pop(); //pop so that the if_cond can perform next_block map to else block
  BodyBlock* block_else_body = new BodyBlock();
  Block* block_if_cond = block_stack_.top();
//  block_if_cond->next_blocks_.insert(block_else_body);
  block_stack_.push(block_if_body);
  block_stack_.push(block_else_body);
  return block_else_body;
}

// todo: double check that only control variables are being handled here and not variables within the body
void PSubsystem::AddControlVariableRelationships(const std::vector<Variable*>& control_variables) {
  for (Variable* v: control_variables) {
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);
  }
}

void PSubsystem::HandleAssignStmt(Entity* entity) {
  AssignEntity* assign_entity = dynamic_cast<AssignEntity*>(entity);
  assert(assign_entity);
  deliverable_->AddAssignEntity(assign_entity);
  deliverable_->AddModifiesRelationship(assign_entity, assign_entity->GetVariable());
  deliverable_->AddModifiesRelationship(current_procedure_, assign_entity->GetVariable());  //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddModifiesRelationship(current_node_, assign_entity->GetVariable());  //container level

  // todo: add these variables into the respective sets in cluster/block
  for (Variable* v: assign_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(assign_entity, v);
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);   //container level
  }
}

void PSubsystem::HandleCallStmt(Entity* entity) {
  CallEntity* call_entity = dynamic_cast<CallEntity*>(entity);
  assert(call_entity);
  deliverable_->AddCallEntity(call_entity);
  //TODO: cater for cross procedure call modifies and uses
}

void PSubsystem::HandlePrintStmt(Entity* entity) {
  PrintEntity* print_entity = dynamic_cast<PrintEntity*>(entity);
  assert(print_entity);
  deliverable_->AddPrintEntity(print_entity);
  // todo: consider adding to the cluster/block sets
  deliverable_->AddUsesRelationship(print_entity, print_entity->GetVariable());
  deliverable_->AddUsesRelationship(current_procedure_, print_entity->GetVariable()); //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddUsesRelationship(current_node_, print_entity->GetVariable());   //container level
}

void PSubsystem::HandleReadStmt(Entity* entity) {
  ReadEntity* read_entity = dynamic_cast<ReadEntity*>(entity);
  assert(read_entity);
  // todo: consider adding to the cluster/block sets
  deliverable_->AddReadEntity(read_entity);
  deliverable_->AddModifiesRelationship(read_entity, read_entity->GetVariable());
  deliverable_->AddUsesRelationship(current_procedure_, read_entity->GetVariable()); //procedure level
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
  // Check if Program.procList === Deliverables.procList
  // assumption: EntityFactory will create into Deliverables.procList whenever encountered
  //             Program.procList will be created when "procedure x {" is encountered.
  //   So, if both are same, it would be correct.
  //   Note that Program.procList is strictly <= Deliverables.procList because EntityFactory will create procedure too.
  //  NOTE:: This method does not check for self-call & recursive call. TODO: this

  // Note that using dup to ensure that my Program.procList still has the original procedure structure.
  // Note that from this method onwards, deliverables.procList will be in sorted order.
  std::list<Procedure*> program_proclist_dup(* deliverable_->GetProgram()->GetProcedureList());
  std::list<Procedure*>* del_proclist = deliverable_->GetProcList();

  program_proclist_dup.sort([](Procedure* a, Procedure* b) { return * a->GetName() < * b->GetName(); });
  del_proclist->sort([](Procedure* a, Procedure* b) { return * a->GetName() < * b->GetName(); });
  bool equal =
      std::equal(program_proclist_dup.begin(), program_proclist_dup.end(), del_proclist->begin(), del_proclist->end());

  if (!equal) {
    throw SyntaxException("A call is made to unreferenced procedure");
  }
}

Deliverable* PSubsystem::GetDeliverables() {
  CheckForIfElseValidity(); //TODO: to put it within main handling if possible.
  CheckForExistingProcedure();
  valid_state = false; //to prevent further processsing.
  return deliverable_;
}


