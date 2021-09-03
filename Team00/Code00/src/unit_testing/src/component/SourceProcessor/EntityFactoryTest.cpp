#include <model/Statement.h>
#include <iostream>
#include "catch.hpp"
#include "component/SourceProcessor/EntityFactory.h"

TEST_CASE("CreateEntities") {
  std::string procedure_name_1 = "proc1";
  cout << "cant run?";
  // procedure test
  vector<Token> procedure_tokens = {
      Token(new string("procedure"), TokenTag()),
      Token(new string("proc1"), TokenTag()),
      Token(new string("{"), TokenTag())
  };
  Procedure expected_procedure_entity = Procedure(new ProcedureName(procedure_name_1));
  Entity actual_procedure_entity = EntityFactory::CreateEntities(procedure_tokens);
  REQUIRE(dynamic_cast<Procedure*>(&actual_procedure_entity) == &expected_procedure_entity);

  /*
  // read test
  vector<Token> read_tokens = {
      Token(new string("read"), TokenTag()),
      Token(new string("x"), TokenTag()),
      Token(new string(";"), TokenTag()),
  };
  ReadEntity expected_read_entity = ReadEntity(new Variable(new VariableName("x")));

  // print test
  vector<Token> print_tokens = {
      Token(new string("print"), TokenTag()),
      Token(new string("x"), TokenTag()),
      Token(new string(";"), TokenTag()),
  };
  PrintEntity expected_print_entity = PrintEntity(new Variable(new VariableName("x")));

  // call test
  vector<Token> call_tokens = {
      Token(new string("call"), TokenTag()),
      Token(new string("anotherProc"), TokenTag()),
      Token(new string(";"), TokenTag()),
  };
  CallEntity expected_call_entity =
      CallEntity(
      new Procedure(
          new ProcedureName("anotherProc")));

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

  // ass test
  vector<Token> assignment_tokens = {
      Token(new string("y"), TokenTag()),
      Token(new string("="), TokenTag()),
      Token(new string("8"), TokenTag()),
      Token(new string("}"), TokenTag()),
  };
  AssignEntity expected_assignment_entity =
      AssignEntity(
          new Variable(
              new VariableName("y")), "8");
*/
}
