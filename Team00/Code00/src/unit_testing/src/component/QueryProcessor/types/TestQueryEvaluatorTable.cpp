//
// Created by Max Ng on 7/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"

TEST_CASE("Target synonym is statement") {
  std::string target = "s1";
  QueryEvaluatorTable table(target);
  REQUIRE( table.getSize() == 0);

  std::list<std::string> synonymList = {"2", "3", "4"};
  bool outcome = table.addTargetSynonym(synonymList);
  REQUIRE( outcome );
  REQUIRE( table.getSize() == 1 );
  REQUIRE( table.getResults()[1] == "3" );

  SECTION( "Remove a valid row" ) {
    bool outcome = table.deleteRow(1);
    REQUIRE( outcome );
    REQUIRE( table.getSize() == 1 );
    REQUIRE( table.getResults().size() == 2);
    REQUIRE( table.getResults()[1] == "4" );
  }

  SECTION( "Remove an invalid row" ) {
    bool outcome = table.deleteRow(3);
    REQUIRE_FALSE( outcome );
    REQUIRE( table.getResults().size() == 3);
  }

  SECTION( "Add synonym column and values") {
    bool outcome = table.addColumn("assignment");
    REQUIRE( outcome );
    REQUIRE( table.getSize() == 2 );

    SECTION("Added values are valid") {
      outcome = table.addRow("assignment", 0, "100");
      REQUIRE( outcome );
      outcome = table.addRow("assignment", 1, "101");
      REQUIRE( outcome );
      REQUIRE( table.getColumn("assignment").size() == 2);
    }

    SECTION("Added values are valid but have invalid index positions") {
      outcome = table.addRow("assignment", 0, "100");
      REQUIRE( outcome );
      outcome = table.addRow("assignment", 0, "101");
      REQUIRE_FALSE( outcome );
    }
  }
}

TEST_CASE("Target synonym is a variable") {
  std::string target = "v";
  QueryEvaluatorTable table(target);
  REQUIRE( table.getSize() == 0);

  std::list<std::string> synonymList = {"x", "count", "p4p"};
  bool outcome = table.addTargetSynonym(synonymList);
  REQUIRE( outcome );
  REQUIRE( table.getSize() == 1 );
  REQUIRE( table.getResults()[2] == "p4p" );

  outcome = table.addColumn("s1");
  REQUIRE( outcome );
  REQUIRE( table.getSize() == 2 );

  SECTION("Add a valid new column") {
    REQUIRE( table.addColumn("s2") );
  }

  SECTION("Add an invalid new column") {
    REQUIRE_FALSE( table.addColumn("s1") );
    REQUIRE_FALSE( table.addColumn("v") );
    REQUIRE( table.getSize() == 2);
  }

  SECTION("Delete a row with an empty column") {
    outcome = table.deleteRow(0);
    REQUIRE( outcome );
    REQUIRE( table.getResults().size() == 2);
  }

  SECTION("Retrieve an empty column") {
    REQUIRE( table.getColumn("s1").empty() );
  }

  SECTION("Empty a valid column") {
    REQUIRE( table.removeColumn("v") );
    REQUIRE( table.getResults().empty() );
  }

  SECTION("Empty an invalid column") {
    REQUIRE_FALSE( table.removeColumn("null") );
  }
}