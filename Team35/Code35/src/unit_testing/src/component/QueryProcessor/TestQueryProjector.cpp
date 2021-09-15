//
// Created by Max Ng on 8/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryProjector.h"

TEST_CASE( "3.QueryProjector.Given list of string of statement ref" ) {
  std::vector<std::string> input_list = {"2", "3", "5"};
  std::string output = QueryProjector::FormatQuery(input_list);
  REQUIRE( output == "2 3 5");
}

TEST_CASE( "3.QueryProjector.Given list of string of variable names" ) {
  std::vector<std::string> input_list = {"x", "count", "p5"};
  std::string output = QueryProjector::FormatQuery(input_list);
  REQUIRE( output == "x count p5");
}
