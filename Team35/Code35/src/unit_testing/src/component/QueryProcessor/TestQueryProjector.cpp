#include "catch.hpp"
#include "component/QueryProcessor/QueryProjector.h"

TEST_CASE("3.QueryProjector.Given list of string of statement ref") {
  std::vector<std::string> input_list = {"2", "3", "5"};
  std::list<std::string> output = QueryProjector::FormatQuery(input_list);
  REQUIRE(output == std::list<std::string>{"2", "3", "5"});
}

TEST_CASE("3.QueryProjector.Given list of string of variable names") {
  std::vector<std::string> input_list = {"x", "count", "p5"};
  std::list<std::string> output = QueryProjector::FormatQuery(input_list);
  REQUIRE(output == std::list<std::string>{ "x", "count", "p5" });
}
