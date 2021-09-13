#include <stdexcept>
#include <cassert>
#include <iostream>
#include "PSubsystem.h"
#include "Tokenizer.h"
#include "EntityFactory.h"

using namespace psub;

void PSubsystem::InitDataStructures() {
  deliverable_ = new Deliverable(); //created in heap so that can pass this object to PKB
  syntax_validator_ = SyntaxValidator();
  entity_factory_ =
      EntityFactory(deliverable_->GetProcList(), deliverable_->GetVariableList(),
                    deliverable_->GetConstantValueList());
}

void PSubsystem::ProcessStatement(std::string statement) {
  if (!valid_state) {
    return; //syntax error had occured. No processing required; TODO: probably throw an error
  }

  std::vector<Token> tokens = Tokenizer::CreateTokens(statement);
  bool valid = syntax_validator_.ValidateSemanticSyntax(tokens);
  valid = true; //TODO: remove this after validation check is complete

  if (tokens[0].GetTokenTag() == TokenTag::kCloseBrace) {
    //close brace, no need to create entity;

    //assertion: case 1: close brace for procedure --> type = 0 & parent_stack = empty
    //           case 2: close brace for others    --> type > 0 & parent_stack ! empty
    assert((current_node_type_ > 0 && !parent_stack_.empty()) || current_node_type_ == 0 && parent_stack_.empty());
    follow_stack_.pop(); //this is allowed because any stmtList must be 1..* statements. so no condition for if (...) { }

    // case 1
    if (current_node_type_ == 0 && parent_stack_.empty()) {
      current_node_type_ = -1;
      current_node_ = nullptr;
    } else {
      Container* current_nest = parent_stack_.top();
      parent_stack_.pop();
      current_node_ = dynamic_cast<Statement*>(current_nest)->GetParentNode();

      /* [INVALID LOGIC]
      //why? Else is wrap within If. So it is 2 steps in AST, so need to parent 2x.
      // however, no need to pop 2x because the "parent" is the same.
      if (current_node_type_ == 3) {
        assert(dynamic_cast<IfEntity*>(current_node_) != nullptr);
        current_node_ = dynamic_cast<Statement*>(current_node_)->GetParentNode();
      }
      */


      if (parent_stack_.empty()) { //back to procedure stmtlist
        current_node_type_ = 0;
        current_node_ = current_procedure;
      } else {
        current_nest = parent_stack_.top();
        if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(current_nest)) {
          current_node_type_ = 1;
        } else if (IfEntity* if_entity = dynamic_cast<IfEntity*>(current_nest)) {
          current_node_type_ = 2;
        } else {
          throw std::invalid_argument("[ERROR] Retrace error. There should not be any other types ");
        }
      }
    }
    return;
  }

  if (!valid) {
    valid_state = false;
    return; //TODO: probably throw an error
  }

  /// TO BE DELETED AFTER ITERATION 1;
  if (tokens[0].GetTokenTag() == TokenTag::kProcedureKeyword && !deliverable_->GetProcList()->empty()) {
    std::cout << "Encountered multiple procedure \n";
    //TODO: Perform exception handling here.
    valid_state = false;
    return;
  }

    Entity* entityObj = entity_factory_.CreateEntities(tokens);

  if (current_node_type_ == -1) { //when current_node_ is null. Only happens when not reading within a procedure.
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
    //TODO: convert multiple-if to get type from EntityFactory
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

}

void PSubsystem::PerformNewProcedureSteps(Procedure* procedure) {
  current_procedure = procedure;
  current_node_ = procedure;
  current_node_type_ = 0;
  //TODO: add proc_var_name;
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
    if_entity->getElseStmtList()->push_back(statement);

    if (if_entity->getElseStmtList()->size() == 1)
      new_else = true;
  } else {
    current_node_->AddStatement(statement);
  }

  if (!parent_stack_.empty()) {
    assert(current_node_type_ == 1 || current_node_type_ == 2 || current_node_type_ == 3);
    statement->SetParentNode(parent_stack_.top());
    deliverable_->AddParentRelationship(reinterpret_cast<Statement*>(parent_stack_.top()), statement);
  }

  /*
  //no need modify follow stack for If and While.
  if (dynamic_cast<IfEntity*>(statement) != nullptr || dynamic_cast<WhileEntity*>(current_node_) != nullptr) {
    return;
  }
  */

  if (current_node_->GetStatementList()->size() == 1 || // 1 because this is newly added in Line curr - 13
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
}

void PSubsystem::HandleElseStmt(ElseEntity* else_entity) {
  IfEntity* if_entity = dynamic_cast<IfEntity*>(current_node_->GetStatementList()->back());
  assert (if_entity != nullptr); //If assertion failed, Else did not follow If
  parent_stack_.push(if_entity);

  if_entity->SetElseEntity(else_entity);
  current_node_type_ = 3;
  current_node_ = if_entity;
}

void PSubsystem::HandleWhileStmt(WhileEntity* while_entity) {
  deliverable_->AddWhileEntity(while_entity);
  parent_stack_.push(while_entity);
  current_node_type_ = 1;
  current_node_ = while_entity;
}

void PSubsystem::HandleAssignStmt(AssignEntity* assign_entity) {
  deliverable_->AddAssignEntity(assign_entity);
  deliverable_->AddModifiesRelationship(assign_entity, assign_entity->getVariable());
  deliverable_->AddModifiesRelationship(current_node_, assign_entity->getVariable());  //container level

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
  deliverable_->AddUsesRelationship(print_entity, print_entity->getVariable());
  deliverable_->AddUsesRelationship(current_node_, print_entity->getVariable());   //container level
}

void PSubsystem::HandleReadStmt(ReadEntity* read_entity) {
  deliverable_->AddReadEntity(read_entity);
  deliverable_->AddModifiesRelationship(read_entity, read_entity->getVariable());
  deliverable_->AddModifiesRelationship(current_node_, read_entity->getVariable());  //container level
}

Deliverable* PSubsystem::GetDeliverables() {
  Program* program = new Program(deliverable_->GetProcList()->front());
  deliverable_->SetProgram(program);
  return deliverable_;
}