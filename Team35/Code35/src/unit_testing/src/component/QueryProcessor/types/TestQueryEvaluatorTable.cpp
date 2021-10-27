#include <model/Statement.h>
#include "catch.hpp"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"

TEST_CASE("3.QueryEvaluatorTable.Target synonym is statement") {
  Synonym *synonym = new Synonym("test", DesignEntity::kRead);
  QueryEvaluatorTable table(synonym);
  REQUIRE(table.GetRowSize() == 0);

  Variable* var_x = new Variable(new VariableName("x"));
  ReadEntity* stmt1 = new ReadEntity(var_x);
  Variable* var_y = new Variable(new VariableName("y"));
  ReadEntity* stmt2 = new ReadEntity(var_y);
  Variable* var_z = new Variable(new VariableName("z"));
  ReadEntity* stmt3 = new ReadEntity(var_z);

  Synonym *assign_syn = new Synonym("a1", DesignEntity::kAssign);
  std::string stmt4_s = "1";
  std::vector<Variable*> stmt4_var_expr;
  std::vector<Constant*> stmt4_cv_expr;
  AssignEntity* stmt4 = new AssignEntity(var_y, stmt4_s, stmt4_var_expr, stmt4_cv_expr);
  std::string stmt5_s = "2";
  std::vector<Variable*> stmt5_var_expr;
  std::vector<Constant*> stmt5_cv_expr;
  AssignEntity* stmt5 = new AssignEntity(var_y, stmt5_s, stmt5_var_expr, stmt5_cv_expr);
  std::string stmt6_s = "3";
  std::vector<Variable*> stmt6_var_expr;
  std::vector<Constant*> stmt6_cv_expr;
  AssignEntity* stmt6 = new AssignEntity(var_y, stmt6_s, stmt6_var_expr, stmt6_cv_expr);

  std::vector<Entity *> synonym_list = {stmt1, stmt2, stmt3};
  bool outcome = table.AddTargetSynonymValues(synonym, synonym_list);
  REQUIRE(outcome);
  REQUIRE(table.GetColumnSize() == 1);
  REQUIRE(table.GetResults()[0][1] == stmt2);

  SECTION("Remove a valid row") {
    bool outcome = table.DeleteRow(1);
    REQUIRE(outcome);
    REQUIRE(table.GetColumnSize() == 1);

    REQUIRE(table.GetResults()[0].size() == 2);
    REQUIRE(table.GetResults()[0][1] == stmt3);
  }

  SECTION("Remove an invalid row") {
    bool outcome = table.DeleteRow(3);
    REQUIRE_FALSE(outcome);
    REQUIRE(table.GetResults()[0].size() == 3);
  }

  SECTION("Add synonym column and values") {
    bool outcome = table.AddColumn(assign_syn);
    REQUIRE(outcome);
    REQUIRE(table.GetColumnSize() == 2);

    SECTION("Added values are valid") {
      outcome = table.AddRow(assign_syn, 0, stmt4);
      REQUIRE(outcome);
      outcome = table.AddRow(assign_syn, 1, stmt5);
      REQUIRE(outcome);
      REQUIRE(table.GetColumn(assign_syn).size() == 2);
    }

    SECTION("Added values are valid but have invalid index positions") {
      outcome = table.AddRow(assign_syn, 0, stmt6);
      REQUIRE(outcome);
      outcome = table.AddRow(assign_syn, 0, stmt5);
      REQUIRE_FALSE(outcome);
    }
  }
}

//TEST_CASE("3.QueryEvaluatorTable.Target synonym is a variable") {
//  std::string target = "v";
//  QueryEvaluatorTable table(target);
//  REQUIRE(table.GetSize() == 0);
//
//  std::list<std::string> synonymList = {"x", "count", "p4p"};
//  bool outcome = table.AddTargetSynonym(synonymList);
//  REQUIRE(outcome);
//  REQUIRE(table.GetSize() == 1);
//  REQUIRE(table.GetResults()[2] == "p4p");
//
//  outcome = table.AddColumn("s1");
//  REQUIRE(outcome);
//  REQUIRE(table.GetSize() == 2);
//
//  SECTION("Add a valid new column") {
//    REQUIRE(table.AddColumn("s2"));
//  }
//
//  SECTION("Add an invalid new column") {
//    REQUIRE_FALSE(table.AddColumn("s1"));
//    REQUIRE_FALSE(table.AddColumn("v"));
//    REQUIRE(table.GetSize() == 2);
//  }
//
//  SECTION("Delete a row with an empty column") {
//    outcome = table.DeleteRow(0);
//    REQUIRE(outcome);
//    REQUIRE(table.GetResults().size() == 2);
//  }
//
//  SECTION("Retrieve an empty column") {
//    REQUIRE(table.GetColumn("s1").empty());
//  }
//
//  SECTION("Empty a valid column") {
//    REQUIRE(table.RemoveColumn("v"));
//    REQUIRE(table.GetResults().empty());
//  }
//
//  SECTION("Empty an invalid column") {
//    REQUIRE_FALSE(table.RemoveColumn("null"));
//  }
//}

//TEST_CASE("3.QueryEvaluatorTable.Add Multiple Rows") {
//  std::string target = "s1";
//  QueryEvaluatorTable table(target);
//
//  std::list<std::string> synonym_list = {"2", "3", "4", "5"};
//  bool outcome = table.AddTargetSynonym(synonym_list);
//  REQUIRE(outcome);
//  table.AddColumn("v1");
//  table.AddRow("v1", 0, "x");
//  bool result = table.AddRow("v1", 2, "x");
//  REQUIRE_FALSE(result);
//  table.AddRow("v1", 1, "y");
//  table.AddRow("v1", 2, "QWERT");
//  table.AddRow("v1", 3, "p2p");
//  table.AddColumn("test");
//  table.AddMultipleRowForAllColumn("test", 0, "a", 0);
//  table.AddMultipleRowForAllColumn("test", 0, "b", 1);
//  table.AddMultipleRowForAllColumn("test", 0, "c", 2);
//  table.AddMultipleRowForAllColumn("test", 0, "d", 3);
//  REQUIRE(table.GetRowSize() == 7);
//  std::vector<std::string> expected = {"2", "2", "2", "2", "3", "4", "5"};
//  REQUIRE(table.GetResults() == expected);
//  table.AddColumn("test2");
//  table.AddRowForAllColumn("test2", 0, "new input");
//  expected = {"2", "2", "2", "2", "2", "3", "4", "5"};
//  REQUIRE(table.GetResults() == expected);
//  REQUIRE(table.GetRowSize() == 8);
//}