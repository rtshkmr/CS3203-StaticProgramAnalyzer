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
  }
}
