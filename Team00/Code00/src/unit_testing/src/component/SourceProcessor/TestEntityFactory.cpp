#include <model/Statement.h>
#include <iostream>
#include "catch.hpp"
#include "component/SourceProcessor/EntityFactory.h"

TEST_CASE("CreateEntities") {
  // procedure test
  vector<Token> procedure_tokens = {
      Token(string("procedure"), TokenTag::kKeyword),
      Token(string("proc1"), TokenTag::kName),
      Token(string("{"), TokenTag::kName)
  };
//  Procedure expected_procedure_entity = Procedure(new ProcedureName("proc1"));
  Entity* actual_procedure_entity = EntityFactory::CreateEntities(procedure_tokens);
  auto* actual_casted_procedure = dynamic_cast<Procedure*>(actual_procedure_entity);
  auto* actual_procedure_name = const_cast<ProcedureName*>(actual_casted_procedure->getName());

  // ideally should be able to overload == and compare 2 objects of the same type
  // but object member pointers not pointing to the correct place yet

  CHECK(actual_procedure_name->getName() == "proc1");

  // read test
  vector<Token> read_tokens = {
      Token(string("read"), TokenTag::kKeyword),
      Token(string("x"), TokenTag::kName),
      Token(string(";"), TokenTag::kName),
  };
//  ReadEntity expected_read_entity = ReadEntity(new Variable(new VariableName("x")));
  Entity* actual_read_entity = EntityFactory::CreateEntities(read_tokens);
  auto* actual_casted_read = dynamic_cast<ReadEntity*>(actual_read_entity);
  auto* actual_read_variable_name = const_cast<VariableName*>(actual_casted_read->getVariable()->getName());

  CHECK(actual_read_variable_name->getName() == "x");

  // print test
  vector<Token> print_tokens = {
      Token(string("print"), TokenTag::kKeyword),
      Token(string("x"), TokenTag::kName),
      Token(string(";"), TokenTag::kName),
  };
//  PrintEntity expected_print_entity = PrintEntity(new Variable(new VariableName("x")));
  Entity* actual_print_entity = EntityFactory::CreateEntities(print_tokens);
  auto* actual_casted_print = dynamic_cast<PrintEntity*>(actual_print_entity);
  auto* actual_print_variable_name = const_cast<VariableName*>(actual_casted_print->getVariable()->getName());

  CHECK(actual_print_variable_name->getName() == "x");

  // call test
  vector<Token> call_tokens = {
      Token(string("call"), TokenTag::kKeyword),
      Token(string("anotherProc"), TokenTag::kName),
      Token(string(";"), TokenTag::kName),
  };
//  CallEntity expected_call_entity =
//      CallEntity(
//      new Procedure(
//          new ProcedureName("anotherProc")));
  Entity* actual_call_entity = EntityFactory::CreateEntities(call_tokens);
  auto* actual_casted_call = dynamic_cast<CallEntity*>(actual_call_entity);
  auto* actual_call_proc_name = const_cast<ProcedureName*>(actual_casted_call->getProcedure()->getName());

  CHECK(actual_call_proc_name->getName() == "anotherProc");

  // ass test
  vector<Token> assign_tokens = {
      Token(string("y"), TokenTag::kName),
      Token(string("="), TokenTag::kName),
      Token(string("8"), TokenTag::kName),
      Token(string(";"), TokenTag::kName),
  };
//  AssignEntity expected_assignment_entity =
//      AssignEntity(
//          new Variable(
//              new VariableName("y")), "8");
  Entity* actual_assign_entity = EntityFactory::CreateEntities(assign_tokens);
  auto* actual_casted_assign = dynamic_cast<AssignEntity*>(actual_assign_entity);
  auto* actual_assign_variable_name = const_cast<VariableName*>(actual_casted_assign->getVariable()->getName());

  CHECK(actual_assign_variable_name->getName() == "y");

  /*
  // while test
  vector<Token> while_tokens = {
      Token(new string("while"), TokenTag()),
      Token(new string("("), TokenTag()),
      Token(new string("x"), TokenTag()),
      Token(new string("=="), TokenTag()),
      Token(new string("1"), TokenTag()),
      Token(new string(")"), TokenTag()),
      Token(new string("{"), TokenTag()),
  };
  WhileEntity expected_while_entity = WhileEntity("x==1");

  // if test
  vector<Token> if_tokens = {
      Token(new string("if"), TokenTag()),
      Token(new string("("), TokenTag()),
      Token(new string("yyy"), TokenTag()),
      Token(new string("!="), TokenTag()),
      Token(new string("abc"), TokenTag()),
      Token(new string(")"), TokenTag()),
      Token(new string("then"), TokenTag()),
  };
  IfEntity expected_if_entity = IfEntity("yyy!=abc");

  // else test
  vector<Token> else_tokens = {
      Token(new string("else"), TokenTag()),
      Token(new string("{"), TokenTag()),
  };
  ElseEntity expected_else_entity = ElseEntity();
*/

  // negative test
  vector<Token> wrong_keyword_tokens = {
      Token(string("wrongKeyword"), TokenTag::kKeyword),
  };
  CHECK_THROWS_AS(EntityFactory::CreateEntities(wrong_keyword_tokens), std::invalid_argument);
}
