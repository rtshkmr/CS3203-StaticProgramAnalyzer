//
// Created by Max Ng on 7/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"

TEST_CASE("Target synonym is statement") {
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

TEST_CASE("Target synonym is a variable") {
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