#include "catch.hpp"
#include <component/PKB/extractor/NextBipExtractor.h>
#include "../../../../utils/EntityUtils.h"
#include "../../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("2.PKB.NextBipExtractor basics") {
  SECTION("1 procedure") {}
  SECTION("1 call") {}
  SECTION("2 call") {}
  SECTION("call in while") {}
  SECTION("call in if") {}
}

TEST_CASE("2.PKB.NextBipExtractor not basic") {
  SECTION("same call to procedure") {}
  SECTION("back to back call") {}
  SECTION("call ends with while") {}
  SECTION("call ends with if") {}
}
