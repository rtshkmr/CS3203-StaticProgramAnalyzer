#include "catch.hpp"
#include <component/PKB/extractor/NextBipExtractor.h>
#include <component/PKB/extractor/NextBipTExtractor.h>
#include <component/PKB/RuntimeExtractor.h>
#include "component/SourceProcessor/SourceProcessor.h"
#include "../../util/TestUtil.h"

using namespace test_util;

TEST_CASE("NextBipExtractor basic integration") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");

  SECTION("no call") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_nextbip_lists = {
        {basic_source_tuples[2]},//1
        {basic_source_tuples[3]},//2
        {basic_source_tuples[4]},//3
        {basic_source_tuples[5]},//4
        {},//5
    };
    
    NextBipExtractor next_bip_extractor = NextBipExtractor(pkb);
    std::vector<Entity*> entities1 = next_bip_extractor.GetRelationship(RelDirection::kForward, "1");
    CHECK(AreEntityListsEqual(expected_nextbip_lists[0], entities1));
    std::vector<Entity*> entities3 = next_bip_extractor.GetRelationship(RelDirection::kForward, "3");
    CHECK(AreEntityListsEqual(expected_nextbip_lists[2], entities3));
    std::vector<Entity*> entities5 = next_bip_extractor.GetRelationship(RelDirection::kForward, "5");
    CHECK(entities5.empty());
  }
}

TEST_CASE("NextBipExtractor more procedures") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/three_procs_source.txt");

  SECTION("call") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_next_bip_lists = {
        {tp_source_tuples[2]},//1
        {tp_source_tuples[3], tp_source_tuples[5]},//2
        {tp_source_tuples[4]},//3
        {tp_source_tuples[7]},//4
        {tp_source_tuples[6]},//5
        {tp_source_tuples[17]},//6
        {tp_source_tuples[8], tp_source_tuples[11]},//7
        {tp_source_tuples[9], tp_source_tuples[6]},//8
        {tp_source_tuples[10]},//9
        {tp_source_tuples[8]},//10
        {tp_source_tuples[12]},//11
        {tp_source_tuples[13], tp_source_tuples[15]},//12
        {tp_source_tuples[14]},//13
        {tp_source_tuples[17]},//14
        {tp_source_tuples[16]},//15
        {tp_source_tuples[17]},//16
        {tp_source_tuples[18]},//17
        {tp_source_tuples[19], tp_source_tuples[6], tp_source_tuples[1]},//18
        {tp_source_tuples[18]},//19
        {},//20
    };

    NextBipExtractor next_bip_extractor = NextBipExtractor(pkb);
    for (int i = 0; i < expected_next_bip_lists.size(); ++i) {
      std::string stmt_num = std::to_string(i + 1);
      std::vector<Entity*> entities = next_bip_extractor.GetRelationship(RelDirection::kForward, stmt_num);
      CHECK(AreEntityListsEqual(expected_next_bip_lists[i], entities));
    }
  }
}

TEST_CASE("NextBipTExtractor basic integration") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");

  SECTION("no call") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_nextbipt_lists = {
        {basic_source_tuples[2], basic_source_tuples[3], basic_source_tuples[4], basic_source_tuples[5]},//1
        {basic_source_tuples[3], basic_source_tuples[4], basic_source_tuples[5]},//2
        {basic_source_tuples[4], basic_source_tuples[5]},//3
        {basic_source_tuples[5]},//4
        {},//5
    };

    RuntimeExtractor rte = RuntimeExtractor(pkb);
    for (int i = 0; i < expected_nextbipt_lists.size(); ++i) {
      std::string stmt_num = std::to_string(i + 1);
      std::vector<Entity*> entities = rte.GetRelationship(PKBRelRefs::kNextBipT, stmt_num);
      CHECK(AreEntityListsEqual(expected_nextbipt_lists[i], entities));
    }
  }
}

TEST_CASE("NextBipTExtractor more procedures") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/three_procs_source.txt");

  SECTION("call") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_next_bipt_lists = {
        {tp_source_tuples[2]},//1
        {tp_source_tuples[3], tp_source_tuples[5]},//2
        {tp_source_tuples[4]},//3
        {tp_source_tuples[7]},//4
        {tp_source_tuples[6]},//5
        {tp_source_tuples[17]},//6
        {tp_source_tuples[8], tp_source_tuples[11]},//7
        {tp_source_tuples[9], tp_source_tuples[6]},//8
        {tp_source_tuples[10]},//9
        {tp_source_tuples[8]},//10
        {tp_source_tuples[12]},//11
        {tp_source_tuples[13], tp_source_tuples[15]},//12
        {tp_source_tuples[14]},//13
        {tp_source_tuples[17]},//14
        {tp_source_tuples[16]},//15
        {tp_source_tuples[17]},//16
        {tp_source_tuples[18]},//17
        {tp_source_tuples[19], tp_source_tuples[6], tp_source_tuples[1]},//18
        {tp_source_tuples[18]},//19
        {},//20
    };
    std::list<std::tuple<EntityEnum, std::string>> expected_lists;
    for (int i = 0; i < 19; ++i) {
      expected_lists.push_back(tp_source_tuples[i + 1]);
    }
    RuntimeExtractor rte = RuntimeExtractor(pkb);
    for (int i = 0; i < expected_next_bipt_lists.size(); ++i) {
      std::string stmt_num = std::to_string(i + 1);
      std::vector<Entity*> entities = rte.GetRelationship(PKBRelRefs::kNextBipT, stmt_num);
      CHECK(AreEntityListsEqual(expected_lists, entities));
    }
  }
}
