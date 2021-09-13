#include "catch.hpp"
#include "component/SourceProcessor/Extractors/FollowsTExtractor.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.FollowsTExtractor.Extract FollowsT basic conditions") {
  SECTION("Single level statement list") {}
  SECTION("Statement list with if") {}
  SECTION("Statement list with while") {}
  SECTION("Multiple procedures") {}
  SECTION("Multiple procedures with call") {}
}

TEST_CASE("1.FollowsTExtractor.Extract FollowsT nested containers") {
  SECTION("Nested if x3") {}
  SECTION("Nested while x3") {}
  SECTION("If and while") {}
  SECTION("Multiple nesting") {}
}
