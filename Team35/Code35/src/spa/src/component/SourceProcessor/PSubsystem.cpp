#include <stdexcept>
#include <cassert>
#include "PSubsystem.h"
#include "Tokenizer.h"
#include "exception/SyntaxException.h"
#include "exception/IterationOneException.h"

using namespace psub;

void PSubsystem::InitDataStructures() {

  /// INTERNAL STATE RESET
  valid_state = true;
  current_procedure_ = nullptr;
  current_node_ = nullptr;
  current_node_type_ = -1;
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
    //close brace, no need to create entity;

    //assertion: case 1: close brace for procedure --> type = 0 & parent_stack = empty
    //           case 2: close brace for others    --> type > 0 & parent_stack ! empty
    assert((current_node_type_ > 0 && !parent_stack_.empty()) || current_node_type_ == 0 && parent_stack_.empty());

    if (current_node_type_ == 2) return; // do not pop anything in if-close brace. pop 2 when finishing else.

    follow_stack_.pop(); //this is allowed because any stmtList must be 1..* statements. so no condition for if (...) { }

    // case 1
    if (current_node_type_ == 0 && parent_stack_.empty()) {
      current_node_type_ = -1;
      current_node_ = nullptr;
    } else {
      Container* current_nest = parent_stack_.top();
      parent_stack_.pop();

      if (current_node_type_ == 3) { //double pop for else clause
        current_nest = parent_stack_.top();
        parent_stack_.pop();
        follow_stack_.pop();
      }

      current_node_ = dynamic_cast<Statement*>(current_nest)->GetParentNode();

      if (parent_stack_.empty()) { //back to procedure stmtlist
        current_node_type_ = 0;
        current_node_ = current_procedure_;
      } else {
        current_nest = parent_stack_.top();
        if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(current_nest)) {
          current_node_type_ = 1;
        } else if (IfEntity* if_entity = dynamic_cast<IfEntity*>(current_nest)) {
          current_node_type_ = 2;
        } else if (ElseEntity* else_entity = dynamic_cast<ElseEntity*>(current_nest)) {
          current_node_type_ = 3;

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

  /// TO BE DELETED AFTER ITERATION 1;
  if (tokens[0].GetTokenTag() == TokenTag::kProcedureKeyword && !deliverable_->GetProcList()->empty()) {
    valid_state = false;
    throw IterationOneException("Multiple Procedure found!");
  } else if (tokens[0].GetTokenTag() == TokenTag::kCallKeyword) {
    valid_state = false;
    throw IterationOneException("Call statement found!");
  }

  Entity* entityObj = entity_factory_.CreateEntities(tokens);

  if (current_node_type_ == -1) { //when current_node_ is null. Only happens when not reading within a procedure.
    assert(current_procedure_ == nullptr && current_node_ == nullptr);
    if (Procedure* procedure = dynamic_cast<Procedure*>(entityObj)) {
      PerformNewProcedureSteps(procedure);
      return;
    } else {
      throw std::invalid_argument("[ERROR] Expected a procedure entity.");
    }
  }

  //From here onwards, Entity must be a Statement type;

  if (Statement* stmt = dynamic_cast<Statement*>(entityObj)) {
    SetStatementObject(stmt);

    //evaluate what type of stmt it is:
    //if, else, while, assign, call, print, read
    //TODO: convert multiple-if to Get type from EntityFactory
    if (IfEntity* if_entity = dynamic_cast<IfEntity*>(stmt)) {
      HandleIfStmt(if_entity);
    } else if (ElseEntity* else_entity = dynamic_cast<ElseEntity*>(stmt)) {
      HandleElseStmt(else_entity);
    } else if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(stmt)) {
      HandleWhileStmt(while_entity);
    } else if (AssignEntity* assign_entity = dynamic_cast<AssignEntity*>(stmt)) {
      HandleAssignStmt(assign_entity);
    } else if (CallEntity* call_entity = dynamic_cast<CallEntity*>(stmt)) {
      HandleCallStmt(call_entity);
    } else if (PrintEntity* print_entity = dynamic_cast<PrintEntity*>(stmt)) {
      HandlePrintStmt(print_entity);
    } else if (ReadEntity* read_entity = dynamic_cast<ReadEntity*>(stmt)) {
      HandleReadStmt(read_entity);
    } else {
      throw std::logic_error("This shouldn't happen... Not one of the Statement types.");
    }
  } else {
    throw std::invalid_argument("[ERROR] current_node_ is not null and Entity is not a Statement type.");
  }
//  LOG (spa_logger << "\n\n\n==========================  [EXIT] ProcessStatement ======================\n\n\n");
}

void PSubsystem::PerformNewProcedureSteps(Procedure* procedure) {
  current_procedure_ = procedure;
  current_node_ = procedure;
  current_node_type_ = 0;

  if (deliverable_->GetProgram() == nullptr) {
    Program* program = new Program(procedure);
    deliverable_->SetProgram(program);
  } else {
    throw IterationOneException("[2] Encountered multiple procedures"); //TODO: remove after Iteration 1
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
  if (current_node_type_ == 3) {
    IfEntity* if_entity = dynamic_cast<IfEntity*>(current_node_);
    assert (if_entity != nullptr);
    if_entity->GetElseStmtList()->push_back(statement);

    if (if_entity->GetElseStmtList()->size() == 1)
      new_else = true;
  } else {
    current_node_->AddStatement(statement);
  }

  if (!parent_stack_.empty()) {
    assert(current_node_type_ == 1 || current_node_type_ == 2 || current_node_type_ == 3);
    statement->SetParentNode(parent_stack_.top());
    deliverable_->AddParentRelationship(dynamic_cast<Statement*>(current_node_), statement);
  }

  if (current_node_type_ != 3 && current_node_->GetStatementList()->size() == 1
      || // 1 because this is newly added in Line curr - 13
          (current_node_type_ == 3 && new_else)) {
    //just entered a stack, follow nothing.
    follow_stack_.push(statement);
  } else {
    statement->SetBeforeNode(follow_stack_.top());
    deliverable_->AddFollowRelationship(follow_stack_.top(), statement);
    follow_stack_.pop();
    follow_stack_.push(statement);
  }

}

void PSubsystem::HandleIfStmt(IfEntity* if_entity) {
  deliverable_->AddIfEntity(if_entity);
  parent_stack_.push(if_entity);
  current_node_type_ = 2;
  current_node_ = if_entity;

  for (Variable* v: if_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(current_node_, v);   //container level which is this if-entity
  }
}

void PSubsystem::HandleElseStmt(ElseEntity* else_entity) {
  IfEntity* if_entity = dynamic_cast<IfEntity*>(parent_stack_.top());

  assert (if_entity != nullptr); //If assertion failed, Else did not follow If
  parent_stack_.push(else_entity);

  if_entity->SetElseEntity(else_entity);
  current_node_type_ = 3;
  current_node_ = if_entity;
}

void PSubsystem::HandleWhileStmt(WhileEntity* while_entity) {
  deliverable_->AddWhileEntity(while_entity);
  parent_stack_.push(while_entity);
  current_node_type_ = 1;
  current_node_ = while_entity;

  for (Variable* v: while_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(current_node_, v);   //container level which is this while-entity
  }
}

void PSubsystem::HandleAssignStmt(AssignEntity* assign_entity) {
  deliverable_->AddAssignEntity(assign_entity);
  deliverable_->AddModifiesRelationship(assign_entity, assign_entity->GetVariable());
  deliverable_->AddModifiesRelationship(current_node_, assign_entity->GetVariable());  //container level

  for (Variable* v: assign_entity->GetExpressionVariables()) {
    deliverable_->AddUsesRelationship(assign_entity, v);
    deliverable_->AddUsesRelationship(current_node_, v);   //container level
  }
}

void PSubsystem::HandleCallStmt(CallEntity* call_entity) {
  deliverable_->AddCallEntity(call_entity);
  //TODO: cater for cross procedure call modifies and uses
}

void PSubsystem::HandlePrintStmt(PrintEntity* print_entity) {
  deliverable_->AddPrintEntity(print_entity);
  deliverable_->AddUsesRelationship(print_entity, print_entity->GetVariable());
  deliverable_->AddUsesRelationship(current_node_, print_entity->GetVariable());   //container level
}

void PSubsystem::HandleReadStmt(ReadEntity* read_entity) {
  deliverable_->AddReadEntity(read_entity);
  deliverable_->AddModifiesRelationship(read_entity, read_entity->GetVariable());
  deliverable_->AddModifiesRelationship(current_node_, read_entity->GetVariable());  //container level
}

void PSubsystem::CheckForIfElseValidity() {
  for (auto const& i: * deliverable_->GetIfList()) {
    if (i->GetElseEntity() == nullptr) {
      throw SyntaxException("Encountered If statement without Else construct");
    }
  }
}

Deliverable* PSubsystem::GetDeliverables() {
  CheckForIfElseValidity(); //TODO: to put it within main handling if possible.

  valid_state = false; //to prevent further processsing.
  return deliverable_;
}

