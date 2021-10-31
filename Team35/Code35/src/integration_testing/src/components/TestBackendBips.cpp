#include "catch.hpp"
#include <component/PKB/extractor/NextBipExtractor.h>
#include "component/SourceProcessor/SourceProcessor.h"
#include "../../util/TestUtil.h"

using namespace test_util;

TEST_CASE("NextBipExtractor basic integration") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");

  SECTION("no call") {
    std::vector<std::vector<std::tuple<EntityEnum, std::string>>> expected_next_lists = {
        {basic_source_tuples[2]},//1
        {basic_source_tuples[3]},//2
        {basic_source_tuples[4]},//3
        {basic_source_tuples[5]},//4
        {},//5
    };
    
    NextBipExtractor next_bip_extractor = NextBipExtractor(pkb);
    std::vector<Entity*> entities1 = next_bip_extractor.GetNextBip(1);
    std::vector<int> expected_int_1 = {2, 3, 4, 5};
    std::vector<int> actual_int_1 = EntitiesToStmtNums(entities1);
    CHECK(expected_int_1 == actual_int_1);
    std::vector<Entity*> entities3 = next_bip_extractor.GetNextBip(3);
    std::vector<int> expected_int_3 = {2, 3, 4, 5};
    std::vector<int> actual_int_3 = EntitiesToStmtNums(entities3);
    CHECK(expected_int_3 == actual_int_3);
    std::vector<Entity*> entities5 = next_bip_extractor.GetNextBip(5);
    CHECK(entities5.empty());
  }
}
