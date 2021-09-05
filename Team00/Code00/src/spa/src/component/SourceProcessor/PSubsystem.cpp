#include <stdexcept>
#include <cassert>
#include "PSubsystem.h"
#include "Tokenizer.h"
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

  if (tokens[0].token_tag_ == TokenTag::kCloseBrace) {
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

      //why? Else is wrap within If. So it is 2 steps in AST, so need to parent 2x.
      // however, no need to pop 2x because the "parent" is the same.
      if (current_node_type_ == 3) {
        assert(dynamic_cast<IfEntity*>(current_node_) != nullptr);
        current_node_ = dynamic_cast<Statement*>(current_node_)->GetParentNode();
      }

      if (parent_stack_.empty()) { //back to procedure stmtlist
        current_node_type_ = 0;
        //TODO: check if uninitialised parent_node == NULL.
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

  Entity* entityObj = EntityFactory::CreateEntities(tokens);

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
  deliverable_.AddProc(procedure);
  current_node_ = procedure;
  current_node_type_ = 0;
  //TODO: add proc_var_name;
}

void PSubsystem::SetStatementObject(Statement* statement) {
  program_counter_++;
  StatementNumber statement_number = StatementNumber(program_counter_);
  statement->SetStatementNumber(&statement_number);

  if (current_node_->GetStatementList()->empty()) {
    //just entered a stack, follow nothing.
    follow_stack_.push(statement);
  } else {
    statement->SetBeforeNode(follow_stack_.top());
    deliverable_.AddFollowRelationship(follow_stack_.top(), statement);
    follow_stack_.pop();
    follow_stack_.push(statement);
  }
  current_node_->AddStatement(statement);

  if (!parent_stack_.empty()) {
    assert(current_node_type_ == 1 || current_node_type_ == 2);
    statement->SetParentNode(parent_stack_.top());
    deliverable_.AddParentRelationship(reinterpret_cast<Statement*>(parent_stack_.top()), statement);
  }
}

void PSubsystem::HandleIfStmt(IfEntity* if_entity) {
  parent_stack_.push(if_entity);
  current_node_type_ = 2;
  current_node_ = if_entity;
  deliverable_.AddVariableVector(if_entity->GetExpressionVariables());
  deliverable_.AddConstantVector(if_entity->GetExpressionConstants());
}

void PSubsystem::HandleElseStmt(ElseEntity* else_entity) {
  current_node_type_ = 3;
  current_node_ = else_entity;
}

void PSubsystem::HandleWhileStmt(WhileEntity* while_entity) {
  parent_stack_.push(while_entity);
  current_node_type_ = 1;
  current_node_ = while_entity;
  deliverable_.AddVariableVector(while_entity->GetExpressionVariables());
  deliverable_.AddConstantVector(while_entity->GetExpressionConstants());
}

void PSubsystem::HandleAssignStmt(AssignEntity* assign_entity) {
  deliverable_.AddAssignEntity(assign_entity);
  deliverable_.AddVariable(assign_entity->getVariable());
  deliverable_.AddVariableVector(assign_entity->GetExpressionVariables());
  deliverable_.AddConstantVector(assign_entity->GetExpressionConstants());
}

void PSubsystem::HandleCallStmt(CallEntity* call_entity) {
  deliverable_.AddCallEntity(call_entity);
}

void PSubsystem::HandlePrintStmt(PrintEntity* print_entity) {
  deliverable_.AddPrintEntity(print_entity);
  deliverable_.AddVariable(print_entity->getVariable());
}

void PSubsystem::HandleReadStmt(ReadEntity* read_entity) {
  deliverable_.AddReadEntity(read_entity);
  deliverable_.AddVariable(read_entity->getVariable());
}

Deliverable PSubsystem::GetDeliverables() {
  return this->deliverable_;
}
