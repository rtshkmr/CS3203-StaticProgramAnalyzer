//
// Created by Max Ng on 7/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"

TEST_CASE("3.QueryEvaluatorTable.Target synonym is statement") {
  std::string target = "s1";
  QueryEvaluatorTable table(target);
  REQUIRE( table.GetSize() == 0);

  std::list<std::string> synonym_list = {"2", "3", "4"};
  bool outcome = table.AddTargetSynonym(synonym_list);
  REQUIRE( outcome );
  REQUIRE( table.GetSize() == 1 );
  REQUIRE( table.GetResults()[1] == "3" );

  SECTION( "Remove a valid row" ) {
    bool outcome = table.DeleteRow(1);
    REQUIRE( outcome );
    REQUIRE( table.GetSize() == 1 );
    REQUIRE( table.GetResults().size() == 2);
    REQUIRE( table.GetResults()[1] == "4" );
  }

  SECTION( "Remove an invalid row" ) {
    bool outcome = table.DeleteRow(3);
    REQUIRE_FALSE( outcome );
    REQUIRE( table.GetResults().size() == 3);
  }

  SECTION( "Add synonym column and values") {
    bool outcome = table.AddColumn("assignment");
    REQUIRE( outcome );
    REQUIRE( table.GetSize() == 2 );

    SECTION("Added values are valid") {
      outcome = table.AddRow("assignment", 0, "100");
      REQUIRE( outcome );
      outcome = table.AddRow("assignment", 1, "101");
      REQUIRE( outcome );
      REQUIRE( table.GetColumn("assignment").size() == 2);
    }

    SECTION("Added values are valid but have invalid index positions") {
      outcome = table.AddRow("assignment", 0, "100");
      REQUIRE( outcome );
      outcome = table.AddRow("assignment", 0, "101");
      REQUIRE_FALSE( outcome );
    }
  }
}

TEST_CASE("3.QueryEvaluatorTable.Target synonym is a variable") {
  std::string target = "v";
  QueryEvaluatorTable table(target);
  REQUIRE( table.GetSize() == 0);

  std::list<std::string> synonymList = {"x", "count", "p4p"};
  bool outcome = table.AddTargetSynonym(synonymList);
  REQUIRE( outcome );
  REQUIRE( table.GetSize() == 1 );
  REQUIRE( table.GetResults()[2] == "p4p" );

  outcome = table.AddColumn("s1");
  REQUIRE( outcome );
  REQUIRE( table.GetSize() == 2 );

  SECTION("Add a valid new column") {
    REQUIRE( table.AddColumn("s2") );
  }

  SECTION("Add an invalid new column") {
    REQUIRE_FALSE( table.AddColumn("s1") );
    REQUIRE_FALSE( table.AddColumn("v") );
    REQUIRE( table.GetSize() == 2);
  }

  SECTION("Delete a row with an empty column") {
    outcome = table.DeleteRow(0);
    REQUIRE( outcome );
    REQUIRE( table.GetResults().size() == 2);
  }

  SECTION("Retrieve an empty column") {
    REQUIRE( table.GetColumn("s1").empty() );
  }

  SECTION("Empty a valid column") {
    REQUIRE( table.RemoveColumn("v") );
    REQUIRE( table.GetResults().empty() );
  }

  SECTION("Empty an invalid column") {
    REQUIRE_FALSE( table.RemoveColumn("null") );
  }
}

TEST_CASE("3.QueryEvaluatorTable.Add Multiple Rows") {
  std::string target = "s1";
  QueryEvaluatorTable table(target);

  std::list<std::string> synonym_list = {"2", "3", "4", "5"};
  bool outcome = table.AddTargetSynonym(synonym_list);
  REQUIRE( outcome );
  table.AddColumn("v1");
  table.AddRow("v1", 0, "x");
  bool result = table.AddRow("v1", 2, "x");
  REQUIRE_FALSE(result);
  table.AddRow("v1", 1, "y");
  table.AddRow("v1", 2, "QWERT");
  table.AddRow("v1", 3, "p2p");
  table.AddColumn("test");
  table.AddMultipleRowForAllColumn("test", 0, "a", 0);
  table.AddMultipleRowForAllColumn("test", 0, "b", 1);
  table.AddMultipleRowForAllColumn("test", 0, "c", 2);
  table.AddMultipleRowForAllColumn("test", 0, "d", 3);
  REQUIRE(table.GetRowSize() == 7);
  std::vector<std::string> expected = {"2", "2", "2", "2", "3", "4", "5"};
  REQUIRE(table.GetResults() == expected);
  table.AddColumn("test2");
  table.AddRowForAllColumn("test2", 0, "new input");
  expected = {"2", "2", "2", "2", "2", "3", "4", "5"};
  REQUIRE(table.GetResults() == expected);
  REQUIRE(table.GetRowSize() == 8);
}