#include "catch.hpp"
#include "component/SourceProcessor/SourceProcessor.h"
#include "component/SourceProcessor/Parser.h"
#include "../../util/TestUtil.h"

using namespace test_util;

TEST_CASE("SP to PKB basic retrieval") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  SECTION("Procedure") {
    std::vector<Entity*> proc_list = pkb->GetDesignEntities(DesignEntity::kProcedure);
    REQUIRE(proc_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(proc_list.at(0)) == "Week4");
  }

  SECTION("Variable") {
    std::vector<Entity*> var_list = pkb->GetDesignEntities(DesignEntity::kVariable);
    REQUIRE(var_list.size() == 3);
    REQUIRE(pkb->GetNameFromEntity(var_list.at(0)) == "x");
    REQUIRE(pkb->GetNameFromEntity(var_list.at(1)) == "y");
    REQUIRE(pkb->GetNameFromEntity(var_list.at(2)) == "z");
  }

  SECTION("Constant") {
    std::vector<Entity*> const_list = pkb->GetDesignEntities(DesignEntity::kConstant);
    REQUIRE(const_list.size() == 3);
    REQUIRE(pkb->GetNameFromEntity(const_list.at(0)) == "1");
    REQUIRE(pkb->GetNameFromEntity(const_list.at(1)) == "3");
    REQUIRE(pkb->GetNameFromEntity(const_list.at(2)) == "3");
  }

  SECTION("Statement") {
    std::vector<Entity*> stmt_list = pkb->GetDesignEntities(DesignEntity::kStmt);
    REQUIRE(stmt_list.size() == 5);
    REQUIRE(pkb->GetNameFromEntity(stmt_list.at(0)) == "1");
    REQUIRE(pkb->GetNameFromEntity(stmt_list.at(1)) == "2");
    REQUIRE(pkb->GetNameFromEntity(stmt_list.at(2)) == "3");
    REQUIRE(pkb->GetNameFromEntity(stmt_list.at(3)) == "4");
    REQUIRE(pkb->GetNameFromEntity(stmt_list.at(4)) == "5");
  }

  SECTION("If") {
    REQUIRE(pkb->GetDesignEntities(DesignEntity::kIf).size() == 0);
  }

  SECTION("While") {
    REQUIRE(pkb->GetDesignEntities(DesignEntity::kWhile).size() == 0);
  }

  SECTION("Assign") {
    std::vector<Entity*> assign_list = pkb->GetDesignEntities(DesignEntity::kAssign);
    REQUIRE(assign_list.size() == 3);
    REQUIRE(pkb->GetNameFromEntity(assign_list.at(0)) == "3");
    REQUIRE(pkb->GetNameFromEntity(assign_list.at(1)) == "4");
    REQUIRE(pkb->GetNameFromEntity(assign_list.at(2)) == "5");
  }

  SECTION("Call") {
    REQUIRE(pkb->GetDesignEntities(DesignEntity::kCall).size() == 0);
  }

  SECTION("Print") {
    std::vector<Entity*> print_list = pkb->GetDesignEntities(DesignEntity::kPrint);
    REQUIRE(print_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(print_list.at(0)) == "2");
  }

  SECTION("Read") {
    std::vector<Entity*> read_list = pkb->GetDesignEntities(DesignEntity::kRead);
    REQUIRE(read_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(read_list.at(0)) == "1");
  }

  SECTION("Follows") {
    std::vector<Entity*> follows_list = pkb->GetRelationship(PKBRelRefs::kFollows, "1");
    REQUIRE(follows_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(follows_list.at(0)) == "2");
    REQUIRE(dynamic_cast<PrintEntity*>(follows_list.at(0)) != nullptr);

    follows_list = pkb->GetRelationship(PKBRelRefs::kFollows, "2");
    REQUIRE(follows_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(follows_list.at(0)) == "3");
    REQUIRE(dynamic_cast<AssignEntity*>(follows_list.at(0)) != nullptr);

    follows_list = pkb->GetRelationship(PKBRelRefs::kFollows, "3");
    REQUIRE(follows_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(follows_list.at(0)) == "4");
    REQUIRE(dynamic_cast<AssignEntity*>(follows_list.at(0)) != nullptr);

    follows_list = pkb->GetRelationship(PKBRelRefs::kFollows, "4");
    REQUIRE(follows_list.size() == 1);
    REQUIRE(pkb->GetNameFromEntity(follows_list.at(0)) == "5");
    REQUIRE(dynamic_cast<AssignEntity*>(follows_list.at(0)) != nullptr);

    follows_list = pkb->GetRelationship(PKBRelRefs::kFollows, "5");
    REQUIRE(follows_list.size() == 0);
  }
}

TEST_CASE("SP to PKB relationships tests") {

  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops_source.txt");

  SECTION("Follows") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_get_follows_lists = {
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[2]},    //1
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[3]},      //2
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[19]},     //3
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[5]},  //4
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[6]},   //5
        std::list<std::tuple<EntityEnum, std::string>>(),                                             //6
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[8]},      //7
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //8
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[10]}, //9
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[11]},     //10
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[16]}, //11
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[13]},  //12
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[14]}, //13
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //14
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //15
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //16
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //17
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //18
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[22]},  //19
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //20
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //21
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[23]}, //22
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //23
        std::list<std::tuple<EntityEnum, std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<EntityEnum, std::string>> expected_get_follows_list = expected_get_follows_lists[i - 1];
      std::vector<Entity*> actual_list = pkb->GetRelationship(PKBRelRefs::kFollows, stmt_no_str);
      int j = 0;
      for (auto expected_follows : expected_get_follows_list) {
        Entity* actual_follows_entity = actual_list[j];
        CHECK(pkb->GetNameFromEntity(actual_follows_entity) == std::get<1>(expected_follows));
        CHECK(actual_follows_entity->GetEntityEnum() == std::get<0>(expected_follows));
        j += 1;
      }
    }
  }

  SECTION("Follows*") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_get_followsT_lists = {
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[2],
            ml_source_tuples[3],
            ml_source_tuples[19],
            ml_source_tuples[22],
            ml_source_tuples[23],},    //1
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[3],
            ml_source_tuples[19],
            ml_source_tuples[22],
            ml_source_tuples[23],},      //2
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[19],
            ml_source_tuples[22],
            ml_source_tuples[23],},     //3
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[5],
            ml_source_tuples[6],},  //4
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[6]},   //5
        std::list<std::tuple<EntityEnum, std::string>>(),                                             //6
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[8]},      //7
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //8
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[10],
            ml_source_tuples[11],
            ml_source_tuples[16],}, //9
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[11],
            ml_source_tuples[16],},     //10
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[16]}, //11
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[13],
            ml_source_tuples[14],},  //12
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[14]}, //13
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //14
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //15
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //16
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //17
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //18
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[22],
            ml_source_tuples[23],},  //19
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //20
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //21
        std::list<std::tuple<EntityEnum, std::string>>{ml_source_tuples[23]}, //22
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //23
        std::list<std::tuple<EntityEnum, std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<EntityEnum, std::string>> expected_get_followsT_list = expected_get_followsT_lists[i - 1];
      std::vector<Entity*> actual_list = pkb->GetRelationship(PKBRelRefs::kFollowsT, stmt_no_str);
      CHECK(AreEntityListsEqual(expected_get_followsT_list, actual_list));
    }
  }

  SECTION("Parent") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_get_parent_lists = {
        std::list<std::tuple<EntityEnum, std::string>>{},    //1
        std::list<std::tuple<EntityEnum, std::string>>{},      //2
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[4],
            ml_source_tuples[5],
            ml_source_tuples[6],
            ml_source_tuples[18],},     //3
        std::list<std::tuple<EntityEnum, std::string>>{},  //4
        std::list<std::tuple<EntityEnum, std::string>>{},   //5
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[7],
            ml_source_tuples[8],},                                             //6
        std::list<std::tuple<EntityEnum, std::string>>{},      //7
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[9],
            ml_source_tuples[10],
            ml_source_tuples[11],
            ml_source_tuples[16],
            ml_source_tuples[17],
        },                                             //8
        std::list<std::tuple<EntityEnum, std::string>>{}, //9
        std::list<std::tuple<EntityEnum, std::string>>{},     //10
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[12],
            ml_source_tuples[13],
            ml_source_tuples[14],
            ml_source_tuples[15],}, //11
        std::list<std::tuple<EntityEnum, std::string>>{},  //12
        std::list<std::tuple<EntityEnum, std::string>>{}, //13
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //14
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //15
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //16
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //17
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //18
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[20],
            ml_source_tuples[21],},  //19
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //20
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //21
        std::list<std::tuple<EntityEnum, std::string>>{}, //22
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //23
        std::list<std::tuple<EntityEnum,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<EntityEnum, std::string>> expected_get_parent_list = expected_get_parent_lists[i - 1];
      std::vector<Entity*> actual_list = pkb->GetRelationship(PKBRelRefs::kParent, stmt_no_str);
      int j = 0;
      for (auto expected_parent : expected_get_parent_list) {
        Entity* actual_parent_entity = actual_list[j];
        CHECK(pkb->GetNameFromEntity(actual_parent_entity) == std::get<1>(expected_parent));
        CHECK(actual_parent_entity->GetEntityEnum() == std::get<0>(expected_parent));
        j += 1;
      }
    }
  }

  SECTION("Parent*") {
    std::vector<std::list<std::tuple<EntityEnum, std::string>>> expected_get_parentT_lists = {
        std::list<std::tuple<EntityEnum, std::string>>{},    //1
        std::list<std::tuple<EntityEnum, std::string>>{},      //2
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[4],
            ml_source_tuples[5],
            ml_source_tuples[6],
            ml_source_tuples[7],
            ml_source_tuples[8],
            ml_source_tuples[9],
            ml_source_tuples[10],
            ml_source_tuples[11],
            ml_source_tuples[12],
            ml_source_tuples[13],
            ml_source_tuples[14],
            ml_source_tuples[15],
            ml_source_tuples[16],
            ml_source_tuples[17],
            ml_source_tuples[18],},     //3
        std::list<std::tuple<EntityEnum, std::string>>{},  //4
        std::list<std::tuple<EntityEnum, std::string>>{},   //5
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[7],
            ml_source_tuples[8],
            ml_source_tuples[9],
            ml_source_tuples[10],
            ml_source_tuples[11],
            ml_source_tuples[12],
            ml_source_tuples[13],
            ml_source_tuples[14],
            ml_source_tuples[15],
            ml_source_tuples[16],
            ml_source_tuples[17],},                                             //6
        std::list<std::tuple<EntityEnum, std::string>>{},      //7
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[9],
            ml_source_tuples[10],
            ml_source_tuples[11],
            ml_source_tuples[12],
            ml_source_tuples[13],
            ml_source_tuples[14],
            ml_source_tuples[15],
            ml_source_tuples[16],
            ml_source_tuples[17],
        },                                             //8
        std::list<std::tuple<EntityEnum, std::string>>{}, //9
        std::list<std::tuple<EntityEnum, std::string>>{},     //10
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[12],
            ml_source_tuples[13],
            ml_source_tuples[14],
            ml_source_tuples[15],}, //11
        std::list<std::tuple<EntityEnum, std::string>>{},  //12
        std::list<std::tuple<EntityEnum, std::string>>{}, //13
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //14
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //15
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //16
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //17
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //18
        std::list<std::tuple<EntityEnum, std::string>>{
            ml_source_tuples[20],
            ml_source_tuples[21],},  //19
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //20
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //21
        std::list<std::tuple<EntityEnum, std::string>>{}, //22
        std::list<std::tuple<EntityEnum, std::string>>{},                                             //23
        std::list<std::tuple<EntityEnum,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<EntityEnum, std::string>> expected_get_parentT_list = expected_get_parentT_lists[i - 1];
      std::vector<Entity*> actual_list = pkb->GetRelationship(PKBRelRefs::kParentT, stmt_no_str);
      CHECK(AreEntityListsEqual(expected_get_parentT_list, actual_list));
    }
  }

  SECTION("Uses statement") {
    // var on lhs
    CHECK(pkb->GetRelationship(PKBRelRefs::kUses, "1").empty());

    // same var on either side
    std::vector<Entity*> mappings2 = pkb->GetRelationship(PKBRelRefs::kUses, "12");
    CHECK(mappings2.size() == 1);
    CHECK(mappings2.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings2.front()) == "psubsystem");

    // multiple var
    std::list<std::tuple<EntityEnum, std::string>>
        expected_list_var3 = std::list<std::tuple<EntityEnum, std::string>>{
        std::make_tuple(EntityEnum::kVariableEntity, "chara"),
        std::make_tuple(EntityEnum::kVariableEntity, "byte")
    };
    auto first_var = pkb->GetRelationship(PKBRelRefs::kUses, "9").at(0);
    CHECK(first_var->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(first_var) == "chara");
    auto second_var = pkb->GetRelationship(PKBRelRefs::kUses, "9").at(1);
    CHECK(second_var->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(second_var) == "byte");

    // print
    std::vector<Entity*> mappings5 = pkb->GetRelationship(PKBRelRefs::kUses, "20");
    CHECK(mappings5.size() == 1);
    CHECK(mappings5.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings5.front()) == "pr0cessSuccessfuI");

    // read
    CHECK(pkb->GetRelationship(PKBRelRefs::kUses, "7").empty());
  }

  SECTION("Modifies statement") {
    // var on lhs
    std::vector<Entity*> mappings1 = pkb->GetRelationship(PKBRelRefs::kModifies,"1");
    CHECK(mappings1.size() == 1);
    CHECK(mappings1.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings1.front()) == "psubsystem");

    // same var on either side
    std::vector<Entity*> mappings2 = pkb->GetRelationship(PKBRelRefs::kModifies,"12");
    CHECK(mappings2.size() == 1);
    CHECK(mappings2.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings2.front()) == "psubsystem");

    // multiple var
    std::vector<Entity*> mappings3 = pkb->GetRelationship(PKBRelRefs::kModifies,"9");
    CHECK(mappings3.size() == 1);
    CHECK(mappings3.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings3.front()) == "chara");

    // read
    std::vector<Entity*> mappings4 = pkb->GetRelationship(PKBRelRefs::kModifies,"7");
    CHECK(mappings4.size() == 1);
    CHECK(mappings4.front()->GetEntityEnum() == EntityEnum::kVariableEntity);
    CHECK(pkb->GetNameFromEntity(mappings4.front()) == "byte");

    // print
    CHECK(pkb->GetRelationship(PKBRelRefs::kModifies, "20").empty());
  }

  SECTION("Parent Else Block") {
      // Children of statement 3
      std::vector<Entity*> children_of_3 = pkb->GetRelationship(PKBRelRefs::kParent, "3");
      CHECK(children_of_3.size() == 4);
      CHECK(children_of_3[0]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_3[0]) == "4");
      CHECK(children_of_3[1]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_3[1]) == "5");
      CHECK(children_of_3[2]->GetEntityEnum() == EntityEnum::kWhileEntity);
      CHECK(pkb->GetNameFromEntity(children_of_3[2]) == "6");
      CHECK(children_of_3[3]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_3[3]) == "18");

      std::vector<Entity*> stmt_18_parent = pkb->GetRelationship(PKBRelRefs::kChild, "18");
      CHECK(stmt_18_parent.size() == 1);
      CHECK(stmt_18_parent[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(stmt_18_parent[0]) == "3");

      // Children of statement 8
      std::vector<Entity*> children_of_8 = pkb->GetRelationship(PKBRelRefs::kParent, "8");
      CHECK(children_of_8.size() == 5);
      CHECK(children_of_8[0]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_8[0]) == "9");
      CHECK(children_of_8[1]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_8[1]) == "10");
      CHECK(children_of_8[2]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(children_of_8[2]) == "11");
      CHECK(children_of_8[3]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_8[3]) == "16");
      CHECK(children_of_8[4]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_8[4]) == "17");

      std::vector<Entity*> stmt_17_parent = pkb->GetRelationship(PKBRelRefs::kChild, "17");
      CHECK(stmt_17_parent.size() == 1);
      CHECK(stmt_17_parent[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(stmt_17_parent[0]) == "8");

      // Children of statement 11
      std::vector<Entity*> children_of_11 = pkb->GetRelationship(PKBRelRefs::kParent, "11");
      CHECK(children_of_11.size() == 4);
      CHECK(children_of_11[0]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_11[0]) == "12");
      CHECK(children_of_11[1]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_11[1]) == "13");
      CHECK(children_of_11[2]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(children_of_11[2]) == "14");
      CHECK(children_of_11[3]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_11[3]) == "15");


      std::vector<Entity*> stmt_15_parent = pkb->GetRelationship(PKBRelRefs::kChild, "15");
      CHECK(stmt_15_parent.size() == 1);
      CHECK(stmt_15_parent[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(stmt_15_parent[0]) == "11");


      // Children of statement 19
      std::vector<Entity*> children_of_19 = pkb->GetRelationship(PKBRelRefs::kParent, "19");
      CHECK(children_of_19.size() == 2);
      CHECK(children_of_19[0]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_19[0]) == "20");
      CHECK(children_of_19[1]->GetEntityEnum() == EntityEnum::kPrintEntity);
      CHECK(pkb->GetNameFromEntity(children_of_19[1]) == "21");


      std::vector<Entity*> stmt_21_parent = pkb->GetRelationship(PKBRelRefs::kChild, "21");
      CHECK(stmt_21_parent.size() == 1);
      CHECK(stmt_21_parent[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(stmt_21_parent[0]) == "19");
  }

  SECTION("Reverse Relationship") {
      std::vector<Entity*> previous_stmt2 = pkb->GetRelationship(PKBRelRefs::kFollowedBy, "2");
      CHECK(previous_stmt2.size() == 1);
      CHECK(previous_stmt2[0]->GetEntityEnum() == EntityEnum::kAssignEntity);
      CHECK(pkb->GetNameFromEntity(previous_stmt2[0]) == "1");

      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "4").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "7").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "9").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "12").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "15").empty());

      std::vector<Entity*> previous_stmt16 = pkb->GetRelationship(PKBRelRefs::kFollowedBy, "16");
      CHECK(previous_stmt16.size() == 1);
      CHECK(previous_stmt16[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(previous_stmt16[0]) == "11");

      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "17").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "18").empty());

      std::vector<Entity*> previous_stmt19 = pkb->GetRelationship(PKBRelRefs::kFollowedBy, "19");
      CHECK(previous_stmt19.size() == 1);
      CHECK(previous_stmt19[0]->GetEntityEnum() == EntityEnum::kIfEntity);
      CHECK(pkb->GetNameFromEntity(previous_stmt19[0]) == "3");

      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "20").empty());
      CHECK(pkb->GetRelationship(PKBRelRefs::kFollowedBy, "21").empty());
  }
}

TEST_CASE("CFG and Next relationship tests") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops2_source.txt");

  SECTION("Next relationships") {
    std::vector<std::vector<std::tuple<EntityEnum, std::string>>> expected_next_lists = {
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

    for (int i = 0; i < 23; ++i) {
      std::vector<Entity*> next = pkb->GetRelationship(PKBRelRefs::kNext, std::to_string(i+1));
      CHECK(next.size() == expected_next_lists[i].size());
      if (next.size() == 1) {
        CHECK(next[0]->GetEntityEnum() == std::get<0>(expected_next_lists[i][0]));
        CHECK(dynamic_cast<Statement*>(next[0])->GetStatementNumber()->GetNum() == stoi(std::get<1>(expected_next_lists[i][0])));
      } else if (next.size() == 2) {
        bool one_enum_or_the_other1 = (next[0]->GetEntityEnum() == std::get<0>(expected_next_lists[i][1]))
            || (next[0]->GetEntityEnum() == std::get<0>(expected_next_lists[i][0]));
        CHECK(one_enum_or_the_other1);
        bool one_num_or_the_other1 = dynamic_cast<Statement*>(next[0])->GetStatementNumber()->GetNum() == stoi(std::get<1>(expected_next_lists[i][0]))
            || dynamic_cast<Statement*>(next[0])->GetStatementNumber()->GetNum() == stoi(std::get<1>(expected_next_lists[i][1]));
        CHECK(one_num_or_the_other1);
        bool one_enum_or_the_other2 = (next[1]->GetEntityEnum() == std::get<0>(expected_next_lists[i][1]))
            || (next[1]->GetEntityEnum() == std::get<0>(expected_next_lists[i][0]));
        CHECK(one_enum_or_the_other2);
        bool one_num_or_the_other2 = dynamic_cast<Statement*>(next[1])->GetStatementNumber()->GetNum() == stoi(std::get<1>(expected_next_lists[i][0]))
            || dynamic_cast<Statement*>(next[1])->GetStatementNumber()->GetNum() == stoi(std::get<1>(expected_next_lists[i][1]));
        CHECK(one_num_or_the_other2);
      }
    }
  }
}
