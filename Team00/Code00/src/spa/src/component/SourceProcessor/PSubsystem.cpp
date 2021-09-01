#include <stdexcept>
#include <cassert>
#include "PSubsystem.h"
#include "Tokenizer.h"
#include "ConcreteSyntaxValidator.h"
#include "EntityFactory.h"

using namespace psub;

void PSubsystem::InitDataStructures() {
  deliverable_ = Deliverable();
  syntax_validator_ = ConcreteSyntaxValidator();
}

void PSubsystem::ProcessStatement(std::string statement) {
  if (!valid_state) {
    return; //syntax error had occured. No processing required; TODO: probably throw an error
  }

  std::vector<Token> tokens = Tokenizer::CreateTokens(statement);
  bool valid = syntax_validator_.ValidateConcreteSyntax(tokens);

  if (!valid) {
    valid_state = false;
    return; //TODO: probably throw an error
  }

  Entity entityObj = EntityFactory::CreateEntities(tokens);

  if (current_node_type_ == -1) { //when current_node_ is null. Only happens when not reading within a procedure.
    if (Procedure* procedure = dynamic_cast<Procedure*>(&entityObj)) {
      PerformNewProcedureSteps(procedure);
      return;
    } else {
      throw std::invalid_argument("[ERROR] Expected a procedure entity.");
    }
  }

  //From here onwards, Entity must be a Statement type;

  if (Statement* stmt = dynamic_cast<Statement*>(&entityObj)) {
    SetStatementObject(stmt);

    //evaluate what type of stmt it is:
    //if, else, while, assign, call, print, read
    //TODO: convert multiple-if to get type from EntityFactory
    if (AssignEntity* assign_entity = dynamic_cast<AssignEntity*>(stmt)) {
      HandleAssignStmt(assign_entity);
    } else if (CallEntity* call_entity = dynamic_cast<CallEntity*>(stmt)) {
      HandleCallStmt(call_entity);
    } else if (PrintEntity* print_entity = dynamic_cast<PrintEntity*>(stmt)) {
      HandlePrintStmt(print_entity);
    } else if (ReadEntity* read_entity = dynamic_cast<ReadEntity*>(stmt)) {
      HandleReadStmt(read_entity);
    } else {
      throw std::logic_error("If, Else, While entity are not available now");
    }
  } else {
    throw std::invalid_argument("[ERROR] current_node_ is not null and Entity is not a Statement type.");
  }

}

void PSubsystem::PerformNewProcedureSteps(Procedure* procedure) {
  deliverable_.AddProc(procedure);
  current_node_ = procedure;
  current_node_type_ = 0;
  //TODO: add proc_var_name;
}

void PSubsystem::SetStatementObject(Statement* statement) {
  program_counter_++;
  StatementNumber statement_number = StatementNumber(program_counter_);
  statement->SetStatementNumber(&statement_number);
  current_node_->AddStatement(statement);

  if (!follow_stack_.empty()) {
    statement->SetBeforeNode(follow_stack_.top());
    deliverable_.AddFollowRelationship(follow_stack_.top(), statement);
    follow_stack_.pop();
  } else {
    assert(current_node_type_ == 0); //if there isnt a follow stack, this must be the first stmt in the procedure
    assert(current_node_->GetStatementList()->empty());
  }
  follow_stack_.push(statement);

  if (!parent_stack_.empty()) {
    assert(current_node_type_ == 1 || current_node_type_ == 2);
    statement->SetParentNode(parent_stack_.top());
    deliverable_.AddParentRelationship(parent_stack_.top(), statement);
  }
}

void PSubsystem::HandleAssignStmt(AssignEntity* assign_entity) {
  deliverable_.AddAssignEntity(assign_entity);
}

void PSubsystem::HandleCallStmt(CallEntity* call_entity) {
  deliverable_.AddCallEntity(call_entity);
}

void PSubsystem::HandlePrintStmt(PrintEntity* print_entity) {
  deliverable_.AddPrintEntity(print_entity);
}

void PSubsystem::HandleReadStmt(ReadEntity* read_entity) {
  deliverable_.AddReadEntity(read_entity);
}

Deliverable PSubsystem::GetDeliverables() {
  return this->deliverable_;
}
