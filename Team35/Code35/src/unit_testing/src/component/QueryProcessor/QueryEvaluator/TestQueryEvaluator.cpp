#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include "../../../../utils/PKBStub.h"
#include "../../../../utils/EntityUtils.h"

static std::vector<int> GetStmtIntValues(std::vector<Entity*> input) {
  std::vector<int> output;
  for (auto entity : input) {
    output.push_back(dynamic_cast<Statement*>(entity)->GetStatementNumber());
  }
  return output;
}

TEST_CASE("3.QueryProcessor.Query select only") {
  Deliverable* deliverable = SetUpDeliverable_Week4();

  PKB *pkb = new PKB();

  pkb->PopulateDataStructures(*deliverable);
  auto* dbm = new DBManager(pkb);
  QueryEvaluator evaluator(dbm);
  SECTION("Basic select query") {
    std::vector<Synonym> synonymList = {Synonym("s2", DesignEntity::kStmt),
                                      Synonym("a1", DesignEntity::kAssign),
                                      Synonym("call", DesignEntity::kCall)};
    std::vector<Group*> groupList;

    SECTION("Basic statement select query") {
      Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);

      UnformattedQueryResult output = evaluator.EvaluateQuery(groupList);
      REQUIRE(output.GetBooleanResult());
      REQUIRE(output.GetTables().empty());


//      std::vector<Entity*> entity_list = output.GetTables()[0]->GetResults()[0];
//
//      std::vector<std::string> actual_answer = GetStmtStringValues(entity_list);
//      std::vector<std::string> expected_answer = {"1", "2", "3", "4"};
//      REQUIRE(actual_answer == expected_answer);
    }
  }

  SECTION("Single SuchThat query") {
    Synonym* stmt = new Synonym("s2", DesignEntity::kStmt);
    Synonym* assign = new Synonym("a1", DesignEntity::kAssign);
    Synonym* call = new Synonym("call", DesignEntity::kCall);
    struct SuchThat st;
    st.rel_ref = RelRef::kFollows;

    SECTION("With double synonyms") {
      st.left_is_synonym = true;
      st.right_is_synonym = true;
      st.left_hand_side = "s2";
      st.right_hand_side = "a1";
      st.first_synonym = stmt;
      st.second_synonym = assign;
      std::vector<Clause*> clause_list = {& st};
      std::vector<Synonym *> target_synonym = {stmt};
      Group firstGroup(clause_list, true, target_synonym);
      std::vector<Group*> group_list = {& firstGroup};

      UnformattedQueryResult output = evaluator.EvaluateQuery(group_list);
      bool boolean_output = output.GetBooleanResult();
      REQUIRE(boolean_output);
      std::vector<Entity *> entity_list = output.GetTables()[0]->GetResults()[0];
      std::vector<int> actual_answer = GetStmtIntValues(entity_list);
      std::vector<int> answer = {2, 3, 4};
      REQUIRE(actual_answer == answer);
      REQUIRE(true);
    }

    SECTION("With single Synonym") {
      st.left_is_synonym = true;
      st.right_is_synonym = false;
      st.left_hand_side = "s2";
      st.right_hand_side = "3";
      st.first_synonym = stmt;

      std::vector<Clause*> clause_list = {& st};
      std::vector<Synonym *> target_synonym = {stmt};
      Group firstGroup(clause_list, true, target_synonym);
      std::vector<Group*> group_list = {& firstGroup};

      UnformattedQueryResult output = evaluator.EvaluateQuery(group_list);
      bool boolean_output = output.GetBooleanResult();
      REQUIRE(boolean_output);
      std::vector<Entity *> entity_list = output.GetTables()[0]->GetResults()[0];
      std::vector<int> actual_answer = GetStmtIntValues(entity_list);
      std::vector<int> answer = {2};
      REQUIRE(actual_answer == answer);
    }

    SECTION("With no synonyms and both references with correct result") {
      st.right_is_synonym = false;
      st.left_is_synonym = false;
      st.right_hand_side = "3";
      st.left_hand_side = "2";

      std::vector<Clause *> empty_clause_list = {};
      std::vector<Clause*> clauseList = {& st};
      std::vector<Synonym *> target_synonym = {stmt};
      std::vector<Synonym *> empty_target_synonym = {};
      Group first_group(empty_clause_list, true, target_synonym);
      Group second_group(clauseList, false, empty_target_synonym);
      std::vector<Group*> group_list = {& first_group, &second_group};

      UnformattedQueryResult output = evaluator.EvaluateQuery(group_list);
      bool boolean_output = output.GetBooleanResult();
      REQUIRE(boolean_output);
      std::vector<Entity *> entity_list = output.GetTables()[0]->GetResults()[0];
      std::vector<int> actual_answer = GetStmtIntValues(entity_list);
      std::vector<int> answer = {1, 2, 3, 4, 5};
      REQUIRE(actual_answer == answer);
    }
//
//    SECTION("With no synonyms and both references but wrong") {
//      st.right_is_synonym = false;
//      st.left_is_synonym = false;
//      st.right_hand_side = "1";
//      st.left_hand_side = "2";
//
//      std::vector<Clause*> clauseList = {& st};
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//
//      Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("With no synonyms and left side is _ with correct result") {
//      st.right_is_synonym = false;
//      st.left_is_synonym = false;
//      st.right_hand_side = "3";
//      st.left_hand_side = "_";
//
//      std::vector<Clause*> clauseList = {& st};
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//
//      Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1", "2", "3", "4"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("With no synonyms and left side is _ with incorrect result") {
//      st.right_is_synonym = false;
//      st.left_is_synonym = false;
//      st.right_hand_side = "1";
//      st.left_hand_side = "_";
//
//      std::vector<Clause*> clauseList = {& st};
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//
//      Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {};
//      REQUIRE(output == answer);
//    }
  }
}

//TEST_CASE("3.QueryEvaluator.Evaluate single Follow and Parent clauses") {
//  Deliverable* deliverable = SetUpDeliverable_Week4();
//  PKB pkb = PKB();
//  pkb.PopulateDataStructures(* deliverable);
//  std::list<Synonym> synonymList = {Synonym("s1", DesignEntity::kStmt),
//                                    Synonym("a1", DesignEntity::kAssign),
//                                    Synonym("a2", DesignEntity::kAssign),
//                                    Synonym("p1", DesignEntity::kPrint),
//                                    Synonym("v1", DesignEntity::kVariable),
//                                    Synonym("r1", DesignEntity::kRead)};
//
//  struct SuchThat st;
//  st.rel_ref = RelRef::kFollows;
//
//  SECTION("Both Synonyms with correct results") {
//    st.left_is_synonym = true;
//    st.left_hand_side = "p1";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "a1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"2"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select neither synonym, with correct boolean result") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3", "4", "5"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("Both Synonyms with incorrect results") {
//    st.left_is_synonym = true;
//    st.left_hand_side = "p1";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "r1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("r1", DesignEntity::kRead);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select neither synonym, with incorrect boolean result") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("2", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("First param is synonym, second is value with correct results") {
//    st.left_is_synonym = true;
//    st.left_hand_side = "a1";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "4";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select synonym not in clause") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("r1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("First param is synonym, second is _ with correct results") {
//    st.left_is_synonym = true;
//    st.left_hand_side = "a1";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "_";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3", "4"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select synonym not in clause") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("r1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("Second param is synonym, first is value with correct results") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "4";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "a1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"5"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select synonym not in clause") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3", "4", "5"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("Second param is synonym, first is _ with correct results") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "_";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "a1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3", "4", "5"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select synonym not in clause") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> groupList = {& firstGroup};
//      Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"3", "4", "5"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("None is synonym, both values, correct answer") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "1";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "2";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    Group firstGroup(clauseList, false);
//    std::list<Group*> groupList = {& firstGroup};
//    Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//    QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//    std::vector<std::string> output = evaluator.EvaluateQuery();
//    std::vector<std::string> answer = {"3", "4", "5"};
//    REQUIRE(output == answer);
//  }
//
//  SECTION("None is synonym, both values, incorrect answer") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "1";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "3";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    Group firstGroup(clauseList, false);
//    std::list<Group*> groupList = {& firstGroup};
//    Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
//    QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//    std::vector<std::string> output = evaluator.EvaluateQuery();
//    std::vector<std::string> answer = {};
//    REQUIRE(output == answer);
//  }
//
//  SECTION("None is synonym, single wildcard, incorrect answer") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "_";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    Group firstGroup(clauseList, false);
//    std::list<Group*> groupList = {& firstGroup};
//    Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//    QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//    std::vector<std::string> output = evaluator.EvaluateQuery();
//    std::vector<std::string> answer = {};
//    REQUIRE(output == answer);
//  }
//
//  SECTION("None is synonym, single wildcard, incorrect answer") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "_";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    Group firstGroup(clauseList, false);
//    std::list<Group*> groupList = {& firstGroup};
//    Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//    QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//    std::vector<std::string> output = evaluator.EvaluateQuery();
//    std::vector<std::string> answer = {};
//    REQUIRE(output == answer);
//  }
//
//  SECTION("None is synonym, doublle wildcard, correct answer") {
//    st.left_is_synonym = false;
//    st.left_hand_side = "_";
//
//    st.right_is_synonym = false;
//    st.right_hand_side = "_";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    Group firstGroup(clauseList, false);
//    std::list<Group*> groupList = {& firstGroup};
//    Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
//    QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);
//
//    std::vector<std::string> output = evaluator.EvaluateQuery();
//    std::vector<std::string> answer = {"3", "4", "5"};
//    REQUIRE(output == answer);
//  }
//}
//
///* procedure Week4 {
// * read x;
// * print x;
// * y = 1;
// * z = 3;
// * z = x + y + 3;
// * }
// */
//TEST_CASE("3.QueryEvaluator.Evaluate single Uses and Modifies clauses") {
//  Deliverable* deliverable = SetUpDeliverable_Week4();
//  PKB pkb = PKB();
//  pkb.PopulateDataStructures(* deliverable);
//  std::list<Synonym> synonymList = {Synonym("s1", DesignEntity::kStmt),
//                                    Synonym("a1", DesignEntity::kAssign),
//                                    Synonym("v2", DesignEntity::kVariable),
//                                    Synonym("p1", DesignEntity::kPrint),
//                                    Synonym("v1", DesignEntity::kVariable),
//                                    Synonym("r1", DesignEntity::kRead)};
//
//  SECTION("Both Synonyms with correct results") {
//    struct SuchThat st;
//    st.rel_ref = RelRef::kModifiesS;
//
//    st.left_is_synonym = true;
//    st.left_hand_side = "r1";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "v1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("r1", DesignEntity::kRead);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("v1", DesignEntity::kVariable);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"x"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select neither synonym, with correct boolean result") {
//      Group firstGroup(clauseList, false);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("v2", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"x", "y", "z"};
//      REQUIRE(output == answer);
//    }
//
//  }
//}
//
///* procedure Transitive {
//    y = 2 * z;        //1
//    if (q != 1) then    {    //2
//        z = x + 24;    //3
//    } else {
//        while (z > 0) {    //4
//            y = x * q - 5;    //5
//        }
//        y = x + y * z + p * q;    //6
//    }
// */
//TEST_CASE("3.QueryEvaluator.Evaluate single ParentT and FollowsT") {
//  Deliverable* deliverable = SetUpDeliverable_Transitive();
//  PKB pkb = PKB();
//  pkb.PopulateDataStructures(* deliverable);
//  std::list<Synonym> synonymList = {Synonym("s1", DesignEntity::kStmt),
//                                    Synonym("a1", DesignEntity::kAssign),
//                                    Synonym("v2", DesignEntity::kVariable),
//                                    Synonym("p1", DesignEntity::kPrint),
//                                    Synonym("v1", DesignEntity::kVariable),
//                                    Synonym("r1", DesignEntity::kRead)};
//
//  SECTION("Both Synonyms with correct results") {
//    struct SuchThat st;
//    st.rel_ref = RelRef::kFollowsT;
//
//    st.left_is_synonym = true;
//    st.left_hand_side = "a1";
//
//    st.right_is_synonym = true;
//    st.right_hand_side = "s1";
//
//    std::vector<Clause*> clauseList = {& st};
//
//    SECTION("Select first synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("Select second synonym") {
//      Group firstGroup(clauseList, true);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("s1", DesignEntity::kStmt);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"2"};
//      REQUIRE(output == answer);
//    }
//  }
//
//  SECTION("Both wildcards") {
//    struct SuchThat st;
//
//    st.left_is_synonym = false;
//    st.left_hand_side = "_";
//    st.right_is_synonym = false;
//    st.right_hand_side = "_";
//
//    SECTION("For ParentT") {
//      std::vector<Clause*> clauseList = {& st};
//      st.rel_ref = RelRef::kParentT;
//
//      Group firstGroup(clauseList, false);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1", "3", "5", "6"};
//      REQUIRE(output == answer);
//    }
//
//    SECTION("For FollowsT with true boolean result") {
//      std::vector<Clause*> clauseList = {& st};
//      st.rel_ref = RelRef::kFollowsT;
//
//      Group firstGroup(clauseList, false);
//      std::list<Group*> group_list = {& firstGroup};
//      Synonym target_synonym = Synonym("a1", DesignEntity::kAssign);
//      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);
//
//      std::vector<std::string> output = evaluator.EvaluateQuery();
//      std::vector<std::string> answer = {"1", "3", "5", "6"};
//      REQUIRE(output == answer);
//    }
//  }
//}
