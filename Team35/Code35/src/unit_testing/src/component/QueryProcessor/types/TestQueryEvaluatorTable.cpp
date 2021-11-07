#include <model/Statement.h>
#include "catch.hpp"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("3.QueryEvaluatorTable.Target synonym is statement") {
  auto *read_synonym = new Synonym("test", DesignEntity::kRead);
  auto *read_synonym_2 = new Synonym("test2", DesignEntity::kRead);
  QueryEvaluatorTable table(read_synonym);
  REQUIRE(table.GetRowSize() == 0);

  ReadEntity* stmt1 = GetReadX();
  ReadEntity* stmt2 = GetReadY();
  ReadEntity* stmt3 = GetReadZ();

  auto *assign_syn = new Synonym("a1", DesignEntity::kAssign);
  AssignEntity* stmt4 = GetAssign1();
  AssignEntity* stmt5 = GetAssign2();
  AssignEntity* stmt6 = GetAssign3();

  std::vector<Entity *> synonym_list = {stmt1, stmt2, stmt3};
  bool outcome = table.AddTargetSynonymValues(read_synonym, synonym_list);
  REQUIRE(outcome);
  REQUIRE(table.GetColumnSize() == 1);
  REQUIRE(table.GetResults()[0][1] == stmt2);

  SECTION("Remove a valid row") {
    outcome = table.DeleteRow(1);
    REQUIRE(outcome);
    REQUIRE(table.GetColumnSize() == 1);

    REQUIRE(table.GetResults()[0].size() == 2);
    REQUIRE(table.GetResults()[0][1] == stmt3);
  }

  SECTION("Remove an invalid row") {
    outcome = table.DeleteRow(3);
    REQUIRE_FALSE(outcome);
    REQUIRE(table.GetResults()[0].size() == 3);
  }

  SECTION("Add synonym column and values") {
    outcome = table.AddColumn(assign_syn);
    REQUIRE(outcome);
    REQUIRE(table.GetColumnSize() == 2);

    SECTION("Added values are valid") {
      outcome = table.AddRow(assign_syn, 3, stmt4);
      REQUIRE(outcome);
      outcome = table.AddRow(assign_syn, 4, stmt5);
      REQUIRE(outcome);
      REQUIRE(table.GetColumn(assign_syn).size() == 5);
    }

    SECTION("Added values are valid but have invalid index positions") {
      outcome = table.AddRow(assign_syn, 3, stmt6);
      REQUIRE(outcome);
      outcome = table.AddRow(assign_syn, 0, stmt5);
      REQUIRE_FALSE(outcome);
    }

    SECTION("Check columns present") {
      REQUIRE(table.ContainsColumn(read_synonym));
      REQUIRE(table.ContainsColumn(assign_syn));
      REQUIRE_FALSE(table.ContainsColumn(read_synonym_2));
    }
  }
}

TEST_CASE("3.QueryEvaluatorTable.Multiple Target Synonyms") {
  auto *variable_synonym = new Synonym("v1", DesignEntity::kRead);
  auto *assign_synonym = new Synonym("a1", DesignEntity::kAssign);
  auto *prog_line_synonym = new Synonym("pl", DesignEntity::kProgLine);
  auto *constant_synonym = new Synonym("const1", DesignEntity::kConstant);
  std::vector<Synonym *> target_synonyms = {variable_synonym, assign_synonym};
  QueryEvaluatorTable table(target_synonyms);
  REQUIRE(table.GetColumnSize() == 0);
  REQUIRE(table.GetRowSize() == 0);

  Variable* var_x = GetVarX();
  Variable* var_z = GetVarZ();
  Variable* var_i = GetVarI();
  std::vector<Entity *> variable_values = {var_x, var_z, var_i};
  AssignEntity *assign_1 = GetAssign1();
  AssignEntity *assign_2 = GetAssign2();
  AssignEntity *assign_3 = GetAssign3();
  std::vector<Entity *> assign_values = {assign_1, assign_2, assign_3};

  table.AddTargetSynonymValues(variable_synonym, variable_values);    // Init, and is first column

  REQUIRE(table.GetColumnSize() == 1);
  REQUIRE(table.GetResults()[0][2] == var_i);

  SECTION("Add an invalid new column") {
    REQUIRE_FALSE(table.AddColumn(variable_synonym));
  }

  SECTION("Add multiple rows") {
    table.AddColumn(assign_synonym);
    table.AddMultipleRowForAllColumn(assign_synonym, 0, assign_1, 0);
    table.AddMultipleRowForAllColumn(assign_synonym, 0, assign_2, 1);
    table.AddMultipleRowForAllColumn(assign_synonym, 1, assign_2, 0);
    table.AddMultipleRowForAllColumn(assign_synonym, 2, assign_1, 0);
    table.AddMultipleRowForAllColumn(assign_synonym, 2, assign_2, 1);
    table.AddMultipleRowForAllColumn(assign_synonym, 2, assign_3, 2);
    REQUIRE(table.GetRowSize() == 6);
    std::vector<std::vector<Entity*>> results = table.GetResults();
    REQUIRE(results[0][1] == var_z);
    REQUIRE(results[0][2] == var_i);
    REQUIRE(results[0][3] == var_x);
    REQUIRE(results[1][2] == assign_1);
    REQUIRE(results[1][3] == assign_2);
    REQUIRE(results[1][5] == assign_3);

    SECTION("Delete a valid row with no empty column") {
      table.DeleteRow(0);
      std::vector<std::vector<Entity*>> curr_results = table.GetResults();
      bool output = curr_results[0][0] == var_z;
      REQUIRE(output);
      REQUIRE(curr_results[1][0] == assign_2);
    }
  }

  table.AddColumn(assign_synonym);
  table.AddMultipleRowForAllColumn(assign_synonym, 0, assign_1, 0);
  table.AddMultipleRowForAllColumn(assign_synonym, 0, assign_2, 1);
  table.AddMultipleRowForAllColumn(assign_synonym, 2, assign_2, 0);
  table.AddMultipleRowForAllColumn(assign_synonym, 3, assign_1, 0);
  table.AddMultipleRowForAllColumn(assign_synonym, 3, assign_2, 1);
  table.AddMultipleRowForAllColumn(assign_synonym, 3, assign_3, 2);

  SECTION("Cross Product column") {
    std::vector<Synonym *> synonyms = {constant_synonym};
    Entity *first_const = GetConst0();
    Entity *second_const = GetConst1();
    Entity *third_const = GetConst3();
    std::vector<Entity *> constant_values = {first_const, second_const, third_const};
    std::vector<std::vector<Entity *>> values = {constant_values};
    table.CrossProductColumns(synonyms, values);
    REQUIRE(table.GetColumnSize() == 3);
    REQUIRE(table.GetRowSize() == 18);
  }

  table.AddColumn(prog_line_synonym);

  SECTION("Delete a row with an empty column") {
    bool outcome = table.DeleteRow(5);
    REQUIRE(outcome);
    REQUIRE(table.GetRowSize() == 5);
    REQUIRE(table.GetColumnSize() == 3);
  }

  SECTION("Retrieve a column") {
    REQUIRE(table.GetColumn(assign_synonym)[0] == assign_1);
  }

  SECTION("Add a valid column") {
    REQUIRE(table.AddColumn(constant_synonym));
  }

  SECTION("Get target synonyms") {
    std::vector<Synonym *> target_synonym_in_table = table.GetTargetSynonymList();
    REQUIRE(target_synonym_in_table[0] == variable_synonym);
    REQUIRE(target_synonym_in_table[1] == assign_synonym);
  }
}
