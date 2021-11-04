#include <datatype/DataType.h>
#include <model/Entity.h>
#include <model/Statement.h>
#include "catch.hpp"

/// IfEntity (with Statement n Container) + ElseEntity
TEST_CASE("1.Model.Statement.IfEntity.ElseEntity") {

  /// Code
  // if (x == 3) then {
  //    read x;
  // } else {
  //    print x;
  // }

  /// Other object creation
  Variable* var_x = new Variable(new VariableName("x"));
  Constant* cv_3 = new Constant(new ConstantValue("3"));

  std::string condition = "x == 3";
  std::vector<Variable*> expr_var{};
  std::vector<Constant*> expr_constants{cv_3};

  IfEntity* if1 = new IfEntity(condition, expr_var, expr_constants);

  ReadEntity* read_entity = new ReadEntity(var_x);
  PrintEntity* print_entity = new PrintEntity(var_x);
  ElseEntity* else_entity = new ElseEntity();

  if1->AddStatement(read_entity);
  if1->SetElseEntity(else_entity);
  else_entity->AddStatement(print_entity);

  SECTION ("Check Conditional Elements") {
    REQUIRE(if1->GetCondExpr()->GetExpressionString() == ConditionalExpression(condition, expr_var).GetExpressionString());
    REQUIRE(if1->GetControlVariables() == expr_var);
    REQUIRE(if1->GetControlConstants() == expr_constants);
  }

  SECTION ("If Container") {
    Container* container_if = dynamic_cast<Container*>(if1);
    REQUIRE(container_if); //check not null
    REQUIRE(container_if->GetStatementList()->front() == read_entity);

    IfEntity* cast_if = dynamic_cast<IfEntity*>(container_if);
    REQUIRE(if1 == cast_if);
  }

  SECTION ("Else Container") {
    Container* container_else = dynamic_cast<Container*>(if1->GetElseEntity());
    REQUIRE(container_else); //check not null
    REQUIRE(container_else->GetStatementList()->front() == print_entity);

    ElseEntity* cast_else = dynamic_cast<ElseEntity*>(container_else);
    REQUIRE(if1->GetElseEntity() == cast_else);
  }

  SECTION ("Integrated Else") {
    REQUIRE(if1->GetElseStmtList()->front() == print_entity);
  }

  SECTION ("[IF] Casting to Statement and back") {
    Statement* stmt_if = dynamic_cast<Statement*>(if1);
    REQUIRE(stmt_if);

    IfEntity* cast_if = dynamic_cast<IfEntity*>(stmt_if);
    REQUIRE(cast_if);

    REQUIRE(if1 == cast_if);
  }

  SECTION("[IF] Casting to Entity and back") {
    Entity* entity_if = dynamic_cast<Entity*>(if1);
    REQUIRE(entity_if);

    IfEntity* cast_if = dynamic_cast<IfEntity*>(entity_if);
    REQUIRE(cast_if);

    REQUIRE(if1 == cast_if);
  }

  SECTION ("[ELSE] Casting to Statement and back") {
    Statement* stmt_else = dynamic_cast<Statement*>(if1->GetElseEntity());
    REQUIRE(stmt_else);

    ElseEntity* cast_else = dynamic_cast<ElseEntity*>(stmt_else);
    REQUIRE(cast_else);

    REQUIRE(if1->GetElseEntity() == cast_else);
  }

  SECTION("[ELSE] Casting to Entity and back") {
    Entity* entity_else = dynamic_cast<Entity*>(if1->GetElseEntity());
    REQUIRE(entity_else);

    ElseEntity* cast_else = dynamic_cast<ElseEntity*>(entity_else);
    REQUIRE(cast_else);

    REQUIRE(if1->GetElseEntity() == cast_else);
  }

}

/// WhileEntity
TEST_CASE("1.Model.Statement.WhileEntity") {
  /// CODE
  // while (x < 10) {
  //   x = x + 1; 
  // } 

  /// Other object creation
  Variable* var_x = new Variable(new VariableName("x"));
  Constant* cv_1 = new Constant(new ConstantValue("1"));
  Constant* cv_10 = new Constant(new ConstantValue("10"));

  std::string condition = "x < 10";
  std::vector<Variable*> expr_var{var_x};
  std::vector<Constant*> expr_constants{cv_10};

  WhileEntity* while_entity = new WhileEntity(condition, expr_var, expr_constants);

  std::string expression = "x + 1";
  std::vector<Variable*> expr_a_var{var_x};
  std::vector<Constant*> expr_a_constants{cv_1};

  AssignEntity* assign_entity = new AssignEntity(var_x, expression, expr_a_var, expr_a_constants);
  while_entity->AddStatement(assign_entity);

  SECTION ("Check Conditional Elements") {
    REQUIRE(
        while_entity->GetCondExpr()->GetExpressionString() == ConditionalExpression(condition, expr_a_var).GetExpressionString());
    REQUIRE(while_entity->GetControlVariables() == expr_var);
    REQUIRE(while_entity->GetControlConstants() == expr_constants);
  }

  SECTION ("While Container") {
    Container* container_while = dynamic_cast<Container*>(while_entity);
    REQUIRE(container_while); //check not null
    REQUIRE(container_while->GetStatementList()->front() == assign_entity);

    WhileEntity* cast_while = dynamic_cast<WhileEntity*>(container_while);
    REQUIRE(while_entity == cast_while);
  }

  SECTION ("Casting to Statement and back") {
    Statement* stmt_while = dynamic_cast<Statement*>(while_entity);
    REQUIRE(stmt_while);

    WhileEntity* cast_while = dynamic_cast<WhileEntity*>(stmt_while);
    REQUIRE(cast_while);

    REQUIRE(while_entity == cast_while);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_while = dynamic_cast<Entity*>(while_entity);
    REQUIRE(entity_while);

    WhileEntity* cast_while = dynamic_cast<WhileEntity*>(entity_while);
    REQUIRE(cast_while);

    REQUIRE(while_entity == cast_while);
  }

}

/// AssignEntity
TEST_CASE("1.Model.Statement.AssignEntity") {
  /// CODE
  //   x = x + y + 100 + z;

  /// Other object creation
  Variable* var_x = new Variable(new VariableName("x"));
  Variable* var_y = new Variable(new VariableName("y"));
  Variable* var_z = new Variable(new VariableName("z"));
  Constant* cv_100 = new Constant(new ConstantValue("100"));

  std::string expression = "x + y + 100 + z";
  std::vector<Variable*> expr_var{var_x, var_y, var_z};
  std::vector<Constant*> expr_constants{cv_100};

  AssignEntity* assign_entity = new AssignEntity(var_x, expression, expr_var, expr_constants);

  SECTION ("Check Elements") {
    REQUIRE(assign_entity->GetVariableObj() == var_x);
    REQUIRE(assign_entity->GetAssignmentExpr()->GetExpressionString()
                == AssignmentExpression(expression).GetExpressionString());

    REQUIRE(assign_entity->GetAssignmentExpr()->CheckExact(expression));

    REQUIRE(assign_entity->GetControlVariables() == expr_var);
    REQUIRE(assign_entity->GetExpressionConstants() == expr_constants);
  }

  SECTION ("Casting to Statement and back") {
    Statement* stmt_assign = dynamic_cast<Statement*>(assign_entity);
    REQUIRE(stmt_assign);

    AssignEntity* cast_assign = dynamic_cast<AssignEntity*>(stmt_assign);
    REQUIRE(cast_assign);

    REQUIRE(assign_entity == cast_assign);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_assign = dynamic_cast<Entity*>(assign_entity);
    REQUIRE(entity_assign);

    AssignEntity* cast_assign = dynamic_cast<AssignEntity*>(entity_assign);
    REQUIRE(cast_assign);

    REQUIRE(assign_entity == cast_assign);
  }

}

/// CallEntity
TEST_CASE("1.Model.Statement.CallEntity") {
  /// CODE
  // call x;

  /// Other object creation
  Procedure* proc_x = new Procedure(new ProcedureName("x"));
  CallEntity* call_entity = new CallEntity(proc_x);

  SECTION ("Check Elements") {
    REQUIRE(call_entity->GetProcedure() == proc_x);
  }

  SECTION ("Casting to Statement and back") {
    Statement* stmt_call = dynamic_cast<Statement*>(call_entity);
    REQUIRE(stmt_call);

    CallEntity* cast_call = dynamic_cast<CallEntity*>(stmt_call);
    REQUIRE(cast_call);

    REQUIRE(call_entity == cast_call);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_call = dynamic_cast<Entity*>(call_entity);
    REQUIRE(entity_call);

    CallEntity* cast_call = dynamic_cast<CallEntity*>(entity_call);
    REQUIRE(cast_call);

    REQUIRE(call_entity == cast_call);
  }

}

/// PrintEntity
TEST_CASE("1.Model.Statement.PrintEntity") {
  /// CODE
  // print x;

  /// Other object creation
  Variable* var_x = new Variable(new VariableName("x"));
  PrintEntity* print_entity = new PrintEntity(var_x);

  SECTION ("Check Elements") {
    REQUIRE(print_entity->GetVariable() == var_x);
  }

  SECTION ("Casting to Statement and back") {
    Statement* stmt_print = dynamic_cast<Statement*>(print_entity);
    REQUIRE(stmt_print);

    PrintEntity* cast_print = dynamic_cast<PrintEntity*>(stmt_print);
    REQUIRE(cast_print);

    REQUIRE(print_entity == cast_print);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_print = dynamic_cast<Entity*>(print_entity);
    REQUIRE(entity_print);

    PrintEntity* cast_print = dynamic_cast<PrintEntity*>(entity_print);
    REQUIRE(cast_print);

    REQUIRE(print_entity == cast_print);
  }

}

/// ReadEntity
TEST_CASE("1.Model.Statement.ReadEntity") {
  /// CODE
  // read x;

  /// Other object creation
  Variable* var_x = new Variable(new VariableName("x"));
  ReadEntity* read_entity = new ReadEntity(var_x);

  SECTION ("Check Elements") {
    REQUIRE(read_entity->GetVariable() == var_x);
  }

  SECTION ("Casting to Statement and back") {
    Statement* stmt_read = dynamic_cast<Statement*>(read_entity);
    REQUIRE(stmt_read);

    ReadEntity* cast_read = dynamic_cast<ReadEntity*>(stmt_read);
    REQUIRE(cast_read);

    REQUIRE(read_entity == cast_read);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_read = dynamic_cast<Entity*>(read_entity);
    REQUIRE(entity_read);

    ReadEntity* cast_read = dynamic_cast<ReadEntity*>(entity_read);
    REQUIRE(cast_read);

    REQUIRE(read_entity == cast_read);
  }

}

/// Test Statement elements - StatementNumber, ParentNode, BeforeNode
/// Note that this test is to check the getters / setters for Statement. As such, the sample code will be simple.
TEST_CASE("1.Model.Statement") {
  /// Code
  // procedure Statement {
  //   while (x == 1) {
  //     read x;
  //     read y;
  //   }
  //   print x;
  // }

  /// Object Creation
  Procedure* proc = new Procedure(new ProcedureName("Statement"));

  //while (x == 1) {
  Variable* var_x = new Variable(new VariableName("x"));
  Constant* cv_1 = new Constant(new ConstantValue("1"));

  std::string condition = "x == 1";
  std::vector<Variable*> expr_var{var_x};
  std::vector<Constant*> expr_constants{cv_1};

  WhileEntity* while_entity = new WhileEntity(condition, expr_var, expr_constants);
  while_entity->SetStatementNumber(new StatementNumber(1));
  while_entity->SetLineNumber(new LineNumber(1));
  while_entity->SetParentNode(nullptr);
  while_entity->SetBeforeNode(nullptr);

  // read x;
  ReadEntity* read_x = new ReadEntity(var_x);
  read_x->SetStatementNumber(new StatementNumber(2));
  read_x->SetLineNumber(new LineNumber(2));
  read_x->SetParentNode(while_entity);
  read_x->SetBeforeNode(nullptr);

  // read y;
  Variable* var_y = new Variable(new VariableName("y"));
  ReadEntity* read_y = new ReadEntity(var_y);
  read_y->SetStatementNumber(new StatementNumber(3));
  read_y->SetLineNumber(new LineNumber(3));
  read_y->SetParentNode(while_entity);
  read_y->SetBeforeNode(read_x);

  // print z;
  Variable* var_z = new Variable(new VariableName("z"));
  PrintEntity* print_z = new PrintEntity(var_y);
  print_z->SetStatementNumber(new StatementNumber(4));
  print_z->SetLineNumber(new LineNumber(4));
  print_z->SetParentNode(nullptr);
  print_z->SetBeforeNode(while_entity);

  SECTION ("Test StatementNumber") {
    REQUIRE(* while_entity->GetStatementNumber() == StatementNumber(1));
    REQUIRE(* read_x->GetStatementNumber() == StatementNumber(2));
    REQUIRE(* read_y->GetStatementNumber() == StatementNumber(3));
    REQUIRE(* print_z->GetStatementNumber() == StatementNumber(4));
  }

  SECTION ("Test LineNumber") {
    REQUIRE(* while_entity->GetLineNumber() == LineNumber(1));
    REQUIRE(* read_x->GetLineNumber() == LineNumber(2));
    REQUIRE(* read_y->GetLineNumber() == LineNumber(3));
    REQUIRE(* print_z->GetLineNumber() == LineNumber(4));
  }

  SECTION ("Test ParentNode") {
    REQUIRE(while_entity->GetParentNode() == nullptr);
    REQUIRE(read_x->GetParentNode() == while_entity);
    REQUIRE(read_y->GetParentNode() == while_entity);
    REQUIRE(print_z->GetParentNode() == nullptr);
  }

  SECTION ("Test BeforeNode") {
    REQUIRE(while_entity->GetBeforeNode() == nullptr);
    REQUIRE(read_x->GetBeforeNode() == nullptr);
    REQUIRE(read_y->GetBeforeNode() == read_x);
    REQUIRE(print_z->GetBeforeNode() == while_entity);
  }

}