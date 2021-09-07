#include <model/Statement.h>
#include <iostream>
#include "catch.hpp"
#include "component/SourceProcessor/EntityFactory.h"

TEST_CASE("1.EntityFactory.CreateEntities") {
  SECTION("procedure test") {
    vector<Token> procedure_tokens = {
        Token(string("procedure"), TokenTag::kProcedureKeyword),
        Token(string("proc1"), TokenTag::kName),
        Token(string("{"), TokenTag::kOpenBrace)
    };
    //  Procedure expected_procedure_entity = Procedure(new ProcedureName("proc1"));
    Entity* actual_procedure_entity = EntityFactory::CreateEntities(procedure_tokens);
    auto* actual_casted_procedure = dynamic_cast<Procedure*>(actual_procedure_entity);
    auto* actual_procedure_name = const_cast<ProcedureName*>(actual_casted_procedure->getName());

    // ideally should be able to overload == and compare 2 objects of the same type
    // but object member pointers not pointing to the correct place yet

    CHECK(actual_procedure_name->getName() == "proc1");
  }

  SECTION("read test") {
    vector<Token> read_tokens = {
        Token(string("read"), TokenTag::kReadKeyword),
        Token(string("x"), TokenTag::kName),
        Token(string(";"), TokenTag::kSemicolon),
    };
    //  ReadEntity expected_read_entity = ReadEntity(new Variable(new VariableName("x")));
    Entity* actual_read_entity = EntityFactory::CreateEntities(read_tokens);
    auto* actual_casted_read = dynamic_cast<ReadEntity*>(actual_read_entity);
    auto* actual_read_variable_name = const_cast<VariableName*>(actual_casted_read->getVariable()->getName());

    CHECK(actual_read_variable_name->getName() == "x");
  }

  SECTION("print test") {
    vector<Token> print_tokens = {
        Token(string("print"), TokenTag::kPrintKeyword),
        Token(string("x"), TokenTag::kName),
        Token(string(";"), TokenTag::kSemicolon),
    };
    //  PrintEntity expected_print_entity = PrintEntity(new Variable(new VariableName("x")));
    Entity* actual_print_entity = EntityFactory::CreateEntities(print_tokens);
    auto* actual_casted_print = dynamic_cast<PrintEntity*>(actual_print_entity);
    auto* actual_print_variable_name = const_cast<VariableName*>(actual_casted_print->getVariable()->getName());

    CHECK(actual_print_variable_name->getName() == "x");
  }

  SECTION("call test") {
    vector<Token> call_tokens = {
        Token(string("call"), TokenTag::kCallKeyword),
        Token(string("anotherProc"), TokenTag::kName),
        Token(string(";"), TokenTag::kSemicolon),
    };
    //  CallEntity expected_call_entity =
    //      CallEntity(
    //      new Procedure(
    //          new ProcedureName("anotherProc")));
    Entity* actual_call_entity = EntityFactory::CreateEntities(call_tokens);
    auto* actual_casted_call = dynamic_cast<CallEntity*>(actual_call_entity);
    auto* actual_call_proc_name = const_cast<ProcedureName*>(actual_casted_call->getProcedure()->getName());

    CHECK(actual_call_proc_name->getName() == "anotherProc");
  }

  SECTION("ass test") {
    vector<Token> assign_tokens = {
        Token(string("y"), TokenTag::kName),
        Token(string("="), TokenTag::kAssignmentOperator),
        Token(string("8"), TokenTag::kInteger),
        Token(string(";"), TokenTag::kSemicolon),
    };
    Entity* actual_assign_entity = EntityFactory::CreateEntities(assign_tokens);
    auto* actual_casted_assign = dynamic_cast<AssignEntity*>(actual_assign_entity);
    auto* actual_assign_variable_name = const_cast<VariableName*>(actual_casted_assign->getVariable()->getName());
    auto actual_assign_expr = actual_casted_assign->getAssignmentExpr()->GetExpressionString();

    CHECK(actual_assign_variable_name->getName() == "y");
    CHECK(actual_assign_expr == "8");
  }

  SECTION("while test") {
    vector<Token> while_tokens = {
        Token(string("while"), TokenTag::kWhileKeyword),
        Token(string("("), TokenTag::kOpenBracket),
        Token(string("x"), TokenTag::kName),
        Token(string("=="), TokenTag::kBinaryComparisonOperator),
        Token(string("1"), TokenTag::kInteger),
        Token(string(")"), TokenTag::kCloseBracket),
        Token(string("{"), TokenTag::kOpenBrace),
    };
    Entity* actual_while_entity = EntityFactory::CreateEntities(while_tokens);
    auto* actual_casted_while = dynamic_cast<WhileEntity*>(actual_while_entity);
    auto actual_while_expr = actual_casted_while->getCondExpr()->GetExpressionString();

    CHECK(actual_while_expr == "x==1");
  }

  SECTION("if test") {
    vector<Token> if_tokens = {
        Token(string("if"), TokenTag::kIfKeyword),
        Token(string("("), TokenTag::kOpenBracket),
        Token(string("yyy"), TokenTag::kName),
        Token(string("!="), TokenTag::kBinaryComparisonOperator),
        Token(string("abc"), TokenTag::kName),
        Token(string(")"), TokenTag::kCloseBracket),
        Token(string("then"), TokenTag::kThenKeyword),
    };
    Entity* actual_if_entity = EntityFactory::CreateEntities(if_tokens);
    auto* actual_casted_if = dynamic_cast<IfEntity*>(actual_if_entity);
    auto actual_if_expr = actual_casted_if->getCondExpr()->GetExpressionString();

    CHECK(actual_if_expr == "yyy!=abc");
  }

  SECTION("else test") {
    vector<Token> else_tokens = {
        Token(string("else"), TokenTag::kElseKeyword),
        Token(string("{"), TokenTag::kOpenBrace),
    };
    ElseEntity expected_else_entity = ElseEntity();
  }

  SECTION("negative test") {
    vector<Token> wrong_keyword_tokens = {
        Token(string("wrongKeyword"), TokenTag::kInvalid),
    };
    CHECK_THROWS_AS(EntityFactory::CreateEntities(wrong_keyword_tokens), std::invalid_argument);
  }
}
