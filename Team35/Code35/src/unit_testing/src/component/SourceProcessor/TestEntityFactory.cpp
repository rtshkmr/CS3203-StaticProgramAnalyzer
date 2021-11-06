#include <model/Statement.h>
#include <iostream>
#include "catch.hpp"
#include "component/SourceProcessor/EntityFactory.h"

using std::string;
using std::vector;

TEST_CASE("1.EntityFactory.CreateEntity") {
  std::list<Procedure*> proc_list;
  std::list<Variable*> var_list;
  std::list<Constant*> const_list;
  EntityFactory entity_factory = EntityFactory(& proc_list, & var_list, & const_list);

  SECTION("procedure test") {
    vector<Token> procedure_tokens = {
        Token(string("procedure"), TokenTag::kProcedureKeyword),
        Token(string("proc1"), TokenTag::kName),
        Token(string("{"), TokenTag::kOpenBrace)
    };
    Entity* actual_procedure_entity = entity_factory.CreateEntity(procedure_tokens);
    auto* actual_casted_procedure = dynamic_cast<Procedure*>(actual_procedure_entity);
    auto* actual_procedure_name = const_cast<ProcedureName*>(actual_casted_procedure->GetName());

    // ideally should be able to overload == and compare 2 objects of the same type
    // but object member pointers not pointing to the correct place yet

    CHECK(actual_procedure_name->GetName() == "proc1");

//    Entity* duplicate_proc = entity_factory.CreateEntity(procedure_tokens);
//    CHECK(actual_procedure_entity == duplicate_proc);
  }

  SECTION("read test") {
    vector<Token> read_tokens = {
        Token(string("read"), TokenTag::kReadKeyword),
        Token(string("x"), TokenTag::kName),
        Token(string(";"), TokenTag::kSemicolon),
    };
    //  ReadEntity expected_read_entity = ReadEntity(new Variable(new VariableName("x")));
    Entity* actual_read_entity = entity_factory.CreateEntity(read_tokens);
    auto* actual_casted_read = dynamic_cast<ReadEntity*>(actual_read_entity);
    auto* actual_read_var = actual_casted_read->GetVariable();
    auto* actual_read_variable_name = const_cast<VariableName*>(actual_read_var->GetVariableName());

    CHECK(actual_read_variable_name->GetName() == "x");

    Entity* duplicate_read = entity_factory.CreateEntity(read_tokens);
    auto* dup_casted_read = dynamic_cast<ReadEntity*>(duplicate_read);
    auto* dup_read_var = dup_casted_read->GetVariable();
    CHECK(actual_read_var == dup_read_var);
  }

  SECTION("print test") {
    vector<Token> print_tokens = {
        Token(string("print"), TokenTag::kPrintKeyword),
        Token(string("x"), TokenTag::kName),
        Token(string(";"), TokenTag::kSemicolon),
    };
    //  PrintEntity expected_print_entity = PrintEntity(new Variable(new VariableName("x")));
    Entity* actual_print_entity = entity_factory.CreateEntity(print_tokens);
    auto* actual_casted_print = dynamic_cast<PrintEntity*>(actual_print_entity);
    auto* actual_print_var = actual_casted_print->GetVariable();
    auto* actual_print_variable_name = const_cast<VariableName*>(actual_print_var->GetVariableName());

    CHECK(actual_print_variable_name->GetName() == "x");

    Entity* duplicate_print = entity_factory.CreateEntity(print_tokens);
    auto* dup_casted_print = dynamic_cast<PrintEntity*>(duplicate_print);
    auto* dup_print_var = dup_casted_print->GetVariable();
    CHECK(actual_print_var == dup_print_var);
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
    Entity* actual_call_entity = entity_factory.CreateEntity(call_tokens);
    auto* actual_casted_call = dynamic_cast<CallEntity*>(actual_call_entity);
    auto* actual_call_proc = actual_casted_call->GetProcedure();
    auto* actual_call_proc_name = const_cast<ProcedureName*>(actual_call_proc->GetName());

    CHECK(actual_call_proc_name->GetName() == "anotherProc");

    Entity* duplicate_call = entity_factory.CreateEntity(call_tokens);
    auto* dup_casted_call = dynamic_cast<CallEntity*>(duplicate_call);
    auto* dup_call_proc = dup_casted_call->GetProcedure();
    CHECK(actual_call_proc == dup_call_proc);
  }

  SECTION("ass test") {
    vector<Token> assign_tokens = {
        Token(string("y"), TokenTag::kName),
        Token(string("="), TokenTag::kAssignmentOperator),
        Token(string("8"), TokenTag::kInteger),
        Token(string(";"), TokenTag::kSemicolon),
    };
    Entity* actual_assign_entity = entity_factory.CreateEntity(assign_tokens);
    auto* actual_casted_assign = dynamic_cast<AssignEntity*>(actual_assign_entity);
    auto* actual_ass_var = actual_casted_assign->GetVariableObj();
    auto* actual_assign_variable_name = const_cast<VariableName*>(actual_ass_var->GetVariableName());
    auto actual_assign_expr = actual_casted_assign->GetAssignmentExpr()->GetExpressionString();

    CHECK(actual_assign_variable_name->GetName() == "y");
    CHECK(actual_assign_expr == "8");

    Entity* duplicate_ass = entity_factory.CreateEntity(assign_tokens);
    auto* dup_casted_ass = dynamic_cast<AssignEntity*>(duplicate_ass);
    auto* dup_ass_var = dup_casted_ass->GetVariableObj();
    CHECK(actual_ass_var == dup_ass_var);
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
    Entity* actual_while_entity = entity_factory.CreateEntity(while_tokens);
    auto* actual_casted_while = dynamic_cast<WhileEntity*>(actual_while_entity);
    auto actual_while_expr = actual_casted_while->GetCondExpr()->GetExpressionString();

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
    Entity* actual_if_entity = entity_factory.CreateEntity(if_tokens);
    auto* actual_casted_if = dynamic_cast<IfEntity*>(actual_if_entity);
    auto actual_if_expr = actual_casted_if->GetCondExpr()->GetExpressionString();

    CHECK(actual_if_expr == "yyy!=abc");
  }

  SECTION("else test") {
    vector<Token> else_tokens = {
        Token(string("else"), TokenTag::kElseKeyword),
        Token(string("{"), TokenTag::kOpenBrace),
    };
    CHECK(dynamic_cast<ElseEntity*>(entity_factory.CreateEntity(else_tokens)));
  }

  SECTION("negative test") {
    vector<Token> wrong_keyword_tokens = {
        Token(string("wrongKeyword"), TokenTag::kInvalid),
    };
    CHECK_THROWS_AS(entity_factory.CreateEntity(wrong_keyword_tokens), std::invalid_argument);
  }
}

TEST_CASE("1.EntityFactory.GetExpressionTokens") {
  SECTION("Assignment and semicolon") {
    vector<Token> tokens = {
        Token("x", TokenTag::kName),
        Token("=", TokenTag::kAssignmentOperator),
        Token("d", TokenTag::kName),
        Token("2", TokenTag::kName),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
        Token(";", TokenTag::kSemicolon),
    };
    vector<Token> expected_tokens = {
        Token("d", TokenTag::kName),
        Token("2", TokenTag::kName),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
    };
    vector<Token>
        actual_tokens = EntityFactory::GetExpressionTokens(tokens, TokenTag::kAssignmentOperator, TokenTag::kSemicolon);
    bool equal = true;
    for (int i = 0; i < actual_tokens.size(); ++i) {
      equal = equal && (actual_tokens[i].GetTokenString() == expected_tokens[i].GetTokenString());
    }
    REQUIRE(equal);
    REQUIRE(actual_tokens.size() == expected_tokens.size());
  }

  SECTION("Brackets") {
    vector<Token> tokens = {
        Token("(", TokenTag::kOpenBracket),
        Token("x", TokenTag::kName),
        Token("d", TokenTag::kName),
        Token("2", TokenTag::kName),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
        Token(")", TokenTag::kCloseBracket),
    };
    vector<Token> expected_tokens = {
        Token("x", TokenTag::kName),
        Token("d", TokenTag::kName),
        Token("2", TokenTag::kName),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
    };
    vector<Token>
        actual_tokens = EntityFactory::GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
    bool equal = true;
    for (int i = 0; i < actual_tokens.size(); ++i) {
      equal = equal && (actual_tokens[i].GetTokenString() == expected_tokens[i].GetTokenString());
    }
    REQUIRE(equal);
    REQUIRE(actual_tokens.size() == expected_tokens.size());
  }

  SECTION("Multiple start and end tags") {
    vector<Token> tokens = {
        Token("x", TokenTag::kName),
        Token("(", TokenTag::kOpenBracket),
        Token("(", TokenTag::kOpenBracket),
        Token("x", TokenTag::kName),
        Token("d", TokenTag::kName),
        Token(")", TokenTag::kCloseBracket),
        Token("2", TokenTag::kName),
        Token("(", TokenTag::kOpenBracket),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
        Token(")", TokenTag::kCloseBracket),
        Token(")", TokenTag::kCloseBracket),
        Token("4", TokenTag::kName),
    };
    vector<Token> expected_tokens = {
        Token("(", TokenTag::kOpenBracket),
        Token("x", TokenTag::kName),
        Token("d", TokenTag::kName),
        Token(")", TokenTag::kCloseBracket),
        Token("2", TokenTag::kName),
        Token("(", TokenTag::kOpenBracket),
        Token("4", TokenTag::kName),
        Token("5", TokenTag::kName),
        Token(")", TokenTag::kCloseBracket),
    };
    vector<Token>
        actual_tokens = EntityFactory::GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
    bool equal = true;
    for (int i = 0; i < actual_tokens.size(); ++i) {
      equal = equal && (actual_tokens[i].GetTokenString() == expected_tokens[i].GetTokenString());
    }
    REQUIRE(equal);
    REQUIRE(actual_tokens.size() == expected_tokens.size());
  }

  SECTION("Nothing between start and end tags") {
    vector<Token> tokens = {
        Token("{", TokenTag::kOpenBrace),
        Token("}", TokenTag::kCloseBrace),
        Token("x", TokenTag::kName),
    };
    REQUIRE_THROWS_AS(EntityFactory::GetExpressionTokens(tokens, TokenTag::kOpenBrace, TokenTag::kCloseBrace),
                      std::invalid_argument);
  }

  SECTION("Nothing between start and end tags") {
    vector<Token> tokens = {
        Token("{", TokenTag::kOpenBrace),
        Token("}", TokenTag::kCloseBrace),
        Token("x", TokenTag::kName),
    };
    REQUIRE_THROWS_AS(EntityFactory::GetExpressionTokens(tokens, TokenTag::kSemicolon, TokenTag::kCloseBrace),
                      std::invalid_argument);
  }
}
