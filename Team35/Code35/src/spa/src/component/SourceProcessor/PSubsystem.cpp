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
  program_counter_ = 0;

  deliverable_ = new Deliverable(); //created in heap so that can pass this object to PKB
  syntax_validator_ = SyntaxValidator();
  entity_factory_ =
      EntityFactory(deliverable_->GetProcList(), deliverable_->GetVariableList(),
                    deliverable_->GetConstantValueList());
}

/**
 * Process 1 statement at a time, and append relationship into Deliverables map.
 * @param statement Statement to be processed
 * @throws SyntaxException throws when a syntax error was found in this statement or any previous statement
 * @throws IterationOneException throws when Call statement was called
 * @throws IterationOneException throws when more than one procedure is processed.
 */
void PSubsystem::ProcessStatement(std::string statement) {
//  LOG (spa_logger << "\n\n\n==========================  [ENTER] ProcessStatement ======================\n\n\n");
  if (!valid_state) {
    throw SyntaxException("Unable to process statement due to an earlier syntax error found, or closed");
  }

  std::vector<Token> tokens = Tokenizer::CreateTokens(statement);
  bool valid = SyntaxValidator::ValidateSyntax(tokens);

  if (!valid) {
    valid_state = false;
    throw SyntaxException("Invalid syntax found.");
  }

  if (tokens[0].GetTokenTag() == TokenTag::kCloseBrace) {
    return HandleCloseBrace(); // Close Brace can early return without creating entity.
  }

  Entity* entityObj = entity_factory_.CreateEntities(tokens);

  if (current_node_type_ == NodeType::kNone) { //when current_node_ is null. Only happens when not reading within a procedure.
    if (Procedure* procedure = dynamic_cast<Procedure*>(entityObj)) {
      assert(entityObj->getEntityEnum() == EntityEnum::kProcedureEntity && current_procedure_ == nullptr && current_node_ == nullptr && follow_stack_.empty() && parent_stack_.empty());
      return PerformNewProcedureSteps(procedure);
    } else {
      throw SyntaxException("Expected a procedure entity at this location but was given another type.");
    }
  }

  //From here onwards, Entity must be a Statement type;
  if (Statement* stmt = dynamic_cast<Statement*>(entityObj)) {
    SetStatementObject(stmt);
    HandleStatement statement = statement_pointer_[static_cast<int>(entityObj->getEntityEnum())];
    (this->*statement)(entityObj);
  } else {
    throw SyntaxException("Expected a Statement but was given a procedure declaration.");
  }
//  LOG (spa_logger << "\n\n\n==========================  [EXIT] ProcessStatement ======================\n\n\n");
}

void PSubsystem::HandleCloseBrace() {
  //assertion: case 1: close brace for procedure --> type = 0 & parent_stack = empty
  //           case 2: close brace for others    --> type > 0 & parent_stack ! empty
  assert((current_node_type_ != NodeType::kProcedure && !parent_stack_.empty()) ||
              current_node_type_ == NodeType::kProcedure && parent_stack_.empty());

  if (current_node_type_ == NodeType::kIf) return; // do not pop anything in if-close brace. pop 2 when finishing else.

  follow_stack_.pop(); //this is allowed because any stmtList must be 1..* statements. so no condition for if (...) { }

  // case 1
  if (current_node_type_ == NodeType::kProcedure && parent_stack_.empty()) {
    current_node_type_ = NodeType::kNone;
    current_node_ = nullptr;
    current_procedure_ = nullptr;
  } else {
    Container* current_nest = parent_stack_.top();
    parent_stack_.pop();

    if (current_node_type_ == NodeType::kElse) { //double pop for else clause
      current_nest = parent_stack_.top();
      parent_stack_.pop();
      follow_stack_.pop();
    }

    current_node_ = dynamic_cast<Statement*>(current_nest)->GetParentNode();

    if (parent_stack_.empty()) { //back to procedure stmtlist
      current_node_type_ = NodeType::kProcedure;
      current_node_ = current_procedure_;
    } else {
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
  }
  return;
}

void PSubsystem::PerformNewProcedureSteps(Procedure* procedure) {
  current_procedure_ = procedure;
  current_node_ = procedure;
  current_node_type_ = NodeType::kProcedure;

  if (deliverable_->GetProgram() == nullptr) {
    Program* program = new Program(procedure);
    deliverable_->SetProgram(program);
  } else {
    // throw error if this name is duplicated
    for (auto const& proc: * deliverable_->GetProgram()->GetProcedureList()) {
      if (*proc->GetName() == *procedure->GetName()) { // uses the overloaded ==
        throw SyntaxException("Encountered 2 procedures with the same name.");
      }
    }

    deliverable_->GetProgram()->AddProcedure(procedure);
  }
}

void PSubsystem::SetStatementObject(Statement* statement) {
  if (dynamic_cast<ElseEntity*>(statement) != nullptr)
    return;

  program_counter_++;
  StatementNumber* statement_number = new StatementNumber(program_counter_);
  statement->SetStatementNumber(statement_number);
  deliverable_->AddStatement(statement);

  bool new_else = false;
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
  throw SyntaxException("Statement is not cast into objects. Check EntityFactory::CreateEntities");
}

void PSubsystem::HandleIfStmt(Entity* entity) {
  IfEntity* if_entity = dynamic_cast<IfEntity*>(entity);
  assert(if_entity);
  deliverable_->AddIfEntity(if_entity);
  parent_stack_.push(if_entity);
  current_node_type_ = NodeType::kIf;
  current_node_ = if_entity;

  for (Variable* v: if_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);   //container level which is this if-entity
  }
}

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
}

void PSubsystem::HandleWhileStmt(Entity* entity) {
  WhileEntity* while_entity = dynamic_cast<WhileEntity*>(entity);
  assert(while_entity);
  deliverable_->AddWhileEntity(while_entity);
  parent_stack_.push(while_entity);
  current_node_type_ = NodeType::kWhile;
  current_node_ = while_entity;

  for (Variable* v: while_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(current_procedure_, v); //procedure level
    if (current_procedure_ != current_node_)
      deliverable_->AddUsesRelationship(current_node_, v);   //container level which is this while-entity
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
  deliverable_->AddUsesRelationship(print_entity, print_entity->GetVariable());
  deliverable_->AddUsesRelationship(current_procedure_, print_entity->GetVariable()); //procedure level
  if (current_procedure_ != current_node_)
    deliverable_->AddUsesRelationship(current_node_, print_entity->GetVariable());   //container level
}

void PSubsystem::HandleReadStmt(Entity* entity) {
  ReadEntity* read_entity = dynamic_cast<ReadEntity*>(entity);
  assert(read_entity);
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

void PSubsystem::CheckForProcedureExisting() {
  // Check if Program.procList === Deliverables.procList
  // assumption: EntityFactory will create into Deliverables.procList whenever encountered
  //             Program.procList will be created when "procedure x {" is encountered.
  //   So, if both are same, it would be correct.
  //   Note that Program.procList is strictly <= Deliverables.procList because EntityFactory will create procedure too.
  //  NOTE:: This method does not check for self-call & recursive call. TODO: this

  // Note that using dup to ensure that my Program.procList still has the original procedure structure.
  // Note that from this method onwards, deliverables.procList will be in sorted order.
  std::list<Procedure*> program_proclist_dup(*deliverable_->GetProgram()->GetProcedureList());
  std::list<Procedure*>* del_proclist = deliverable_->GetProcList();

  program_proclist_dup.sort([](Procedure* a, Procedure* b) { return *a->GetName() < *b->GetName(); } );
  del_proclist->sort([](Procedure* a, Procedure* b) { return *a->GetName() < *b->GetName(); } );
  bool equal = std::equal(program_proclist_dup.begin(), program_proclist_dup.end(), del_proclist->begin(), del_proclist->end());

  if (!equal) {
    throw SyntaxException("A call is made to unreferenced procedure");
  }
}

Deliverable* PSubsystem::GetDeliverables() {
  CheckForIfElseValidity(); //TODO: to put it within main handling if possible.
  CheckForProcedureExisting();

  valid_state = false; //to prevent further processsing.
  return deliverable_;
}

