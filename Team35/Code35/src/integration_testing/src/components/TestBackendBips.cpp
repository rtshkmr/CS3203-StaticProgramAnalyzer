#include "catch.hpp"
#include <component/PKB/extractor/NextBipExtractor.h>
#include <component/PKB/extractor/NextBipTExtractor.h>
#include <component/PKB/RuntimeExtractor.h>
#include "component/SourceProcessor/SourceProcessor.h"
#include "../../util/TestUtil.h"

using namespace test_util;

TEST_CASE("NextBipExtractor basic integration") {
  SECTION("no call no nest") {
    PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_nextbip_lists = {
        {basic_source_tuples[2]},//1
        {basic_source_tuples[3]},//2
        {basic_source_tuples[4]},//3
        {basic_source_tuples[5]},//4
        {},//5
    };

    NextBipExtractor next_bip_extractor = NextBipExtractor(pkb);
    std::vector<Entity*> entities1 = next_bip_extractor.GetRelationship(RelDirection::kForward, 1);
    CHECK(AreEntityListsEqual(expected_nextbip_lists[0], entities1));
    std::vector<Entity*> entities3 = next_bip_extractor.GetRelationship(RelDirection::kForward, 3);
    CHECK(AreEntityListsEqual(expected_nextbip_lists[2], entities3));
    std::vector<Entity*> entities5 = next_bip_extractor.GetRelationship(RelDirection::kForward, 5);
    CHECK(entities5.empty());
  }

  SECTION("no call yes nest") {
    PKB* pkb =
        sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops2_source.txt");
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_next_lists = {
        {ml2_source_tuples[2]},//1
        {ml2_source_tuples[3]},//2
        {ml2_source_tuples[4], ml2_source_tuples[6]},//3
        {ml2_source_tuples[5]},//4
        {ml2_source_tuples[19]},//5
        {ml2_source_tuples[7], ml2_source_tuples[19]},//6
        {ml2_source_tuples[6], ml2_source_tuples[8]},//7
        {ml2_source_tuples[9]},//8
        {ml2_source_tuples[10], ml2_source_tuples[18]},//9
        {ml2_source_tuples[11]},//10
        {ml2_source_tuples[12]},//11
        {ml2_source_tuples[13], ml2_source_tuples[16]},//12
        {ml2_source_tuples[14]},//13
        {ml2_source_tuples[15]},//14
        {ml2_source_tuples[17]},//15
        {ml2_source_tuples[17]},//16
        {ml2_source_tuples[7]},//17
        {ml2_source_tuples[7]},//18
        {ml2_source_tuples[20], ml2_source_tuples[21]},//19
        {ml2_source_tuples[22]},//20
        {ml2_source_tuples[22]},//21
        {ml2_source_tuples[23]},//22
        {},//23
    };
    RuntimeExtractor rte = RuntimeExtractor(pkb);
    for (int i = 0; i < expected_next_lists.size(); ++i) {
      std::vector<Entity*> entities = rte.GetRelationship(PKBRelRefs::kNextBip, std::to_string(i + 1));
      CHECK(AreEntityListsEqual(expected_next_lists[i], entities));
    }
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
      std::vector<Entity*> entities = next_bip_extractor.GetRelationship(RelDirection::kForward, i + 1);
      CHECK(AreEntityListsEqual(expected_next_bip_lists[i], entities));
    }
  }
}

TEST_CASE("NextBipTExtractor basic integration") {
  SECTION("no call no nest") {
    PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
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

  SECTION("no call yes nest") {
    PKB* pkb =
        sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops2_source.txt");
    std::vector<std::vector<int>> expected_tuple_indices = {
        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {5, 19, 20, 21, 22, 23},
        {19, 20, 21, 22, 23},
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //6
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //7
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //8
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //9
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //10
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //11
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //12
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //13
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //14
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //15
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //16
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //17
        {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}, //18
        {20, 21, 22, 23},
        {22, 23},
        {22, 23},
        {23},
        {}
    };
    RuntimeExtractor rte = RuntimeExtractor(pkb);
    for (int i = 0; i < 23; ++i) {
      std::vector<Entity*> next = rte.GetRelationship(PKBRelRefs::kNextBipT, std::to_string(i+1));
      CHECK(next.size() == expected_tuple_indices[i].size());
      std::list<std::tuple<EntityEnum, std::string>> expected_list;
      for (int j : expected_tuple_indices[i]) {
        expected_list.push_back(ml2_source_tuples[j]);
      }
      CHECK(AreEntityListsEqual(expected_list, next));
    }
  }
}

TEST_CASE("NextBipTExtractor more procedures") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/three_procs_source.txt");

  SECTION("call") {
    std::list<std::tuple<EntityEnum, std::string>> expected_lists;
    for (int i = 0; i < 19; ++i) {
      expected_lists.push_back(tp_source_tuples[i + 1]);
    }
    RuntimeExtractor rte = RuntimeExtractor(pkb);
    for (int i = 0; i < 19; ++i) {
      std::string stmt_num = std::to_string(i + 1);
      std::vector<Entity*> entities = rte.GetRelationship(PKBRelRefs::kNextBipT, stmt_num);
      CHECK(AreEntityListsEqual(expected_lists, entities));
    }
  }
}
