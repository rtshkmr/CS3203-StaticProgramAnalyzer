//
// Created by Max Ng on 8/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryProjector.h"

TEST_CASE( "Given list of string of statement ref" ) {
  std::vector<std::string> inputList = {"2", "3", "5"};
  std::string output = formatQuery(inputList);
  REQUIRE( output == "2, 3, 5");
}

TEST_CASE( "Given list of string of variable names" ) {
  std::vector<std::string> inputList = {"x", "count", "p5"};
  std::string output = formatQuery(inputList);
  REQUIRE( output == "x, count, p5");
}
