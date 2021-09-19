#include "catch.hpp"
#include <component/QueryProcessor/QuerySystemController.h>
#include <component/PKB/PKB.h>
#include "datatype/Deliverable.h"

/* procedure Week4 {
 * read x;
 * print x;
 * y = 1;
 * z = 3;
 * z = x + y + 3;
 * }
 */

Deliverable* SetUpDeliverable_Week4() {
  /// Using ./tests/basic_source.txt as input

  // procedure
  Deliverable* deliverable = new Deliverable();
  Procedure* proc = new Procedure(new ProcedureName("Week4"));
  Program* program = new Program(proc);
  deliverable->SetProgram(program);
  std::list<Procedure*>* proc_list = deliverable->GetProcList();
  proc_list->push_back(proc);

  std::list<Variable*>* var_list = deliverable->GetVariableList();
  std::list<ConstantValue*>* const_list = deliverable->GetConstantValueList();

  // read x
  Variable* var_x = new Variable(new VariableName("x"));
  var_list->push_back(var_x);

  ReadEntity* stmt1 = new ReadEntity(var_x);
  stmt1->SetStatementNumber(new StatementNumber(1));
  proc->AddStatement(stmt1);
  deliverable->AddStatement(stmt1);
  deliverable->AddReadEntity(stmt1);
  deliverable->AddModifiesRelationship(stmt1, stmt1->GetVariable());

  // print x
  PrintEntity* stmt2 = new PrintEntity(var_x);
  stmt2->SetStatementNumber(new StatementNumber(2));
  stmt2->SetBeforeNode(stmt1);
  proc->AddStatement(stmt2);
  deliverable->AddStatement(stmt2);
  deliverable->AddPrintEntity(stmt2);
  deliverable->AddUsesRelationship(stmt2, stmt2->GetVariable());

  // y = 1
  ConstantValue* cv1 = new ConstantValue("1");
  const_list->push_back(cv1);
  Variable* var_y = new Variable(new VariableName("y"));
  var_list->push_back(var_y);

  std::string stmt3_s = "1";
  std::vector<Variable*> stmt3_var_expr;
  std::vector<ConstantValue*> stmt3_cv_expr;
  stmt3_cv_expr.push_back(cv1);

  AssignEntity* stmt3 = new AssignEntity(var_y, stmt3_s, stmt3_var_expr, stmt3_cv_expr);
  stmt3->SetStatementNumber(new StatementNumber(3));
  stmt3->SetBeforeNode(stmt2);
  proc->AddStatement(stmt3);
  deliverable->AddStatement(stmt3);
  deliverable->AddAssignEntity(stmt3);
  deliverable->AddModifiesRelationship(stmt3, stmt3->GetVariable());

  for (Variable* v: stmt3->GetExpressionVariables()) {
    deliverable->AddUsesRelationship(stmt3, v);
  }

  // z = 3
  ConstantValue* cv2 = new ConstantValue("3");
  const_list->push_back(cv2);
  Variable* var_z = new Variable(new VariableName("z"));
  var_list->push_back(var_z);

  std::string stmt4_s = "3";
  std::vector<Variable*> stmt4_var_expr;
  std::vector<ConstantValue*> stmt4_cv_expr;
  stmt4_cv_expr.push_back(cv2);

  AssignEntity* stmt4 = new AssignEntity(var_z, stmt4_s, stmt4_var_expr, stmt4_cv_expr);
  stmt4->SetStatementNumber(new StatementNumber(4));
  stmt4->SetBeforeNode(stmt3);
  proc->AddStatement(stmt4);
  deliverable->AddStatement(stmt4);
  deliverable->AddAssignEntity(stmt4);
  deliverable->AddModifiesRelationship(stmt4, stmt4->GetVariable());

  for (Variable* v: stmt4->GetExpressionVariables()) {
    deliverable->AddUsesRelationship(stmt4, v);
  }

  // z = x + y + 3;
  ConstantValue* cv3 = new ConstantValue("3"); //note that ConstantValue is not unique.
  const_list->push_back(cv3);

  std::string stmt5_s = "x + y + 3";
  std::vector<Variable*> stmt5_var_expr;
  stmt5_var_expr.push_back(var_x);
  stmt5_var_expr.push_back(var_y);
  std::vector<ConstantValue*> stmt5_cv_expr;
  stmt5_cv_expr.push_back(cv3);

  AssignEntity* stmt5 = new AssignEntity(var_z, stmt5_s, stmt5_var_expr, stmt5_cv_expr);
  stmt5->SetStatementNumber(new StatementNumber(5));
  stmt5->SetBeforeNode(stmt4);
  proc->AddStatement(stmt5);
  deliverable->AddStatement(stmt5);
  deliverable->AddAssignEntity(stmt5);
  deliverable->AddModifiesRelationship(stmt5, stmt5->GetVariable());

  for (Variable* v: stmt5->GetExpressionVariables()) {
    deliverable->AddUsesRelationship(stmt5, v);
  }

  deliverable->AddFollowRelationship(stmt1, stmt2);
  deliverable->AddFollowRelationship(stmt2, stmt3);
  deliverable->AddFollowRelationship(stmt3, stmt4);
  deliverable->AddFollowRelationship(stmt4, stmt5);

  return deliverable;
}

TEST_CASE("5. Miscellaneous entity and relationship retrievals") {

  /// Using ./tests/basic_source.txt as input
  Deliverable* deliverable = SetUpDeliverable_Week4();
  PKB pkb = PKB();
  pkb.PopulateDataStructures(* deliverable);

  /*
   * todo:
   * 1. Uses and Modifies have their arguments swapped (Uses(v,a) works when it should be Uses(a,v)
   * 2. The PQL grammar does not allow for ';' at the end of select clauses -> should throw exception and exit gracefully
   * 3. Follows is not working as expected (refer to the commented test-cases below)
   * 4. _ and "x" need to be allowed for as wildcards and variable names specifically.
   * 5.
   */

  std::vector<std::string> query_list = {
      "variable v; Select v",
      "assign a; Select a",
      "read r; Select r",
      "procedure p; Select p",
      "assign a, b; Select a",
      "read r; print p; Select r such that Follows(r, p)",
      "assign procedure; Select procedure",
      "read r; print p; Select p such that Follows(r, p)",
      "assign a; assign b; Select a such that Follows(a, b)",
      "variable v; read r; print p; Select v such that Follows(r, p)",
      "variable v; assign a; print p; Select v such that Follows(a, p)",

//            "variable v; assign a; Select v such that Uses(a, v)", // this does not work while Uses(v, a) works -> params are mixed up
//            "variable v; assign a; Select v such that Modifies(a, v)", // same as above

//            "assign a; Select v such that Uses(a, \"x\")", // PQL should be able to handle variables in inverted commas
//            "assign a; Select v such that Uses(a, _)", // QueryProcessor should be able to handle underscore wildcard

//            The following are not in the scope of iter 1.
//            "read r; print p; assign a; Select p such that Follows(r, p) such that Follows(p, a)", // should get "2", get "" instead
//            "read r; print p; assign a; Select p such that Follows(p, r) such that Follows(p, a)", // should get "", get "2 instead
  };

  std::vector<std::list<std::string>> answer_list = {
      {"x", "y", "z"},
      {"3", "4", "5"},
      {"1"},
      {"Week4"},
      {"3", "4", "5"},
      {"1"},
      {"3", "4", "5"},
      {"2"},
      {"3", "4"},
      {"x", "y", "z"},
      {},
      {"dummy"}
  };
  for (int i = 0; i < query_list.size(); i++) {
    std::string* query = & query_list.at(i);
    std::list<std::string> answer = QuerySystemController::Evaluate(query, & pkb);
    REQUIRE(answer == answer_list[i]);
  }
}

