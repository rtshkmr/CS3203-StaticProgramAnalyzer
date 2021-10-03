#include "catch.hpp"
#include "component/SourceProcessor/SourceProcessor.h"
#include "component/SourceProcessor/Parser.h"

TEST_CASE("SP to PKB basic retrieval") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  SECTION("Procedure") {
    REQUIRE(std::list<std::string>{"Week4"} == pkb->GetDesignEntity(DesignEntity::kProcedure));
  }

  SECTION("Variable") {
    REQUIRE(std::list<std::string>{"x", "y", "z"} == pkb->GetDesignEntity(DesignEntity::kVariable));
  }

  SECTION("Constant") {
    REQUIRE(std::list<std::string>{"1", "3", "3"} == pkb->GetDesignEntity(DesignEntity::kConstant));
  }

  SECTION("Statement") {
    REQUIRE(std::list<std::string>{"1", "2", "3", "4", "5"} == pkb->GetDesignEntity(DesignEntity::kStmt));
  }

  SECTION("If") {
    REQUIRE(std::list<std::string>{} == pkb->GetDesignEntity(DesignEntity::kIf));
  }

  SECTION("While") {
    REQUIRE(std::list<std::string>{} == pkb->GetDesignEntity(DesignEntity::kWhile));
  }

  SECTION("Assign") {
    REQUIRE(std::list<std::string>{"3", "4", "5"} == pkb->GetDesignEntity(DesignEntity::kAssign));
  }

  SECTION("Call") {
    REQUIRE(std::list<std::string>{} == pkb->GetDesignEntity(DesignEntity::kCall));
  }

  SECTION("Print") {
    REQUIRE(std::list<std::string>{"2"} == pkb->GetDesignEntity(DesignEntity::kPrint));
  }

  SECTION("Read") {
    REQUIRE(std::list<std::string>{"1"} == pkb->GetDesignEntity(DesignEntity::kRead));
  }

  SECTION("Follows") {
    REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kPrint, "2")}
                == pkb->GetFollows("1"));
    REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "3")}
                == pkb->GetFollows("2"));
    REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "4")}
                == pkb->GetFollows("3"));
    REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "5")}
                == pkb->GetFollows("4"));
    REQUIRE(pkb->GetFollows("5").empty());
  }
}

TEST_CASE("SP to PKB relationships tests") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops_source.txt");

  std::vector<std::tuple<DesignEntity, std::string>> source_tuples = {
      std::make_tuple(DesignEntity::kAssign, "-1000000"), // placeholder
      std::make_tuple(DesignEntity::kAssign, "1"),
      std::make_tuple(DesignEntity::kRead, "2"),
      std::make_tuple(DesignEntity::kIf, "3"),
      std::make_tuple(DesignEntity::kAssign, "4"),
      std::make_tuple(DesignEntity::kAssign, "5"),
      std::make_tuple(DesignEntity::kWhile, "6"),
      std::make_tuple(DesignEntity::kRead, "7"),
      std::make_tuple(DesignEntity::kIf, "8"),
      std::make_tuple(DesignEntity::kAssign, "9"),
      std::make_tuple(DesignEntity::kAssign, "10"),
      std::make_tuple(DesignEntity::kIf, "11"),
      std::make_tuple(DesignEntity::kAssign, "12"),
      std::make_tuple(DesignEntity::kPrint, "13"),
      std::make_tuple(DesignEntity::kAssign, "14"),
      std::make_tuple(DesignEntity::kPrint, "15"),
      std::make_tuple(DesignEntity::kAssign, "16"),
      std::make_tuple(DesignEntity::kPrint, "17"),
      std::make_tuple(DesignEntity::kPrint, "18"),
      std::make_tuple(DesignEntity::kIf, "19"),
      std::make_tuple(DesignEntity::kPrint, "20"),
      std::make_tuple(DesignEntity::kPrint, "21"),
      std::make_tuple(DesignEntity::kPrint, "22"),
      std::make_tuple(DesignEntity::kAssign, "23")
  };

  SECTION("Follows") {
    std::vector<std::list<std::tuple<DesignEntity, std::string>>> expected_get_follows_lists = {
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[2]},    //1
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[3]},      //2
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[19]},     //3
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[5]},  //4
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[6]},   //5
        std::list<std::tuple<DesignEntity, std::string>>(),                                             //6
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[8]},      //7
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //8
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[10]}, //9
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[11]},     //10
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[16]}, //11
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[13]},  //12
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[14]}, //13
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //14
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //15
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //16
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //17
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //18
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[22]},  //19
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //20
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //21
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[23]}, //22
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //23
        std::list<std::tuple<DesignEntity,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<DesignEntity, std::string>> expected_get_follows_list = expected_get_follows_lists[i - 1];
      std::list<std::tuple<DesignEntity, std::string>> actual_list = pkb->GetFollows(stmt_no_str);
      CHECK(actual_list == expected_get_follows_list);
    }
  }

  SECTION("Follows*") {
    std::vector<std::list<std::tuple<DesignEntity, std::string>>> expected_get_followsT_lists = {
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[2],
            source_tuples[3],
            source_tuples[19],
            source_tuples[22],
            source_tuples[23],},    //1
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[3],
            source_tuples[19],
            source_tuples[22],
            source_tuples[23],},      //2
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[19],
            source_tuples[22],
            source_tuples[23],},     //3
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[5],
            source_tuples[6],},  //4
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[6]},   //5
        std::list<std::tuple<DesignEntity, std::string>>(),                                             //6
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[8]},      //7
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //8
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[10],
            source_tuples[11],
            source_tuples[16],}, //9
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[11],
            source_tuples[16],},     //10
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[16]}, //11
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[13],
            source_tuples[14],},  //12
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[14]}, //13
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //14
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //15
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //16
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //17
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //18
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[22],
            source_tuples[23],},  //19
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //20
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //21
        std::list<std::tuple<DesignEntity, std::string>>{source_tuples[23]}, //22
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //23
        std::list<std::tuple<DesignEntity,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<DesignEntity, std::string>> expected_get_followsT_list = expected_get_followsT_lists[i - 1];
      std::list<std::tuple<DesignEntity, std::string>> actual_list = pkb->GetFollowsT(stmt_no_str);
      CHECK(actual_list == expected_get_followsT_list);
    }
  }

  SECTION("Parent") {
    std::vector<std::list<std::tuple<DesignEntity, std::string>>> expected_get_parent_lists = {
        std::list<std::tuple<DesignEntity, std::string>>{},    //1
        std::list<std::tuple<DesignEntity, std::string>>{},      //2
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[4],
            source_tuples[5],
            source_tuples[6],
            source_tuples[18],},     //3
        std::list<std::tuple<DesignEntity, std::string>>{},  //4
        std::list<std::tuple<DesignEntity, std::string>>{},   //5
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[7],
            source_tuples[8],},                                             //6
        std::list<std::tuple<DesignEntity, std::string>>{},      //7
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[9],
            source_tuples[10],
            source_tuples[11],
            source_tuples[16],
            source_tuples[17],
        },                                             //8
        std::list<std::tuple<DesignEntity, std::string>>{}, //9
        std::list<std::tuple<DesignEntity, std::string>>{},     //10
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[12],
            source_tuples[13],
            source_tuples[14],
            source_tuples[15],}, //11
        std::list<std::tuple<DesignEntity, std::string>>{},  //12
        std::list<std::tuple<DesignEntity, std::string>>{}, //13
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //14
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //15
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //16
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //17
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //18
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[20],
            source_tuples[21],},  //19
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //20
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //21
        std::list<std::tuple<DesignEntity, std::string>>{}, //22
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //23
        std::list<std::tuple<DesignEntity,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<DesignEntity, std::string>> expected_get_parent_list = expected_get_parent_lists[i - 1];
      std::list<std::tuple<DesignEntity, std::string>> actual_list = pkb->GetParent(stmt_no_str);
      CHECK(actual_list == expected_get_parent_list);
    }
  }

  SECTION("Parent*") {
    std::vector<std::list<std::tuple<DesignEntity, std::string>>> expected_get_parentT_lists = {
        std::list<std::tuple<DesignEntity, std::string>>{},    //1
        std::list<std::tuple<DesignEntity, std::string>>{},      //2
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[4],
            source_tuples[5],
            source_tuples[6],
            source_tuples[7],
            source_tuples[8],
            source_tuples[9],
            source_tuples[10],
            source_tuples[11],
            source_tuples[12],
            source_tuples[13],
            source_tuples[14],
            source_tuples[15],
            source_tuples[16],
            source_tuples[17],
            source_tuples[18],},     //3
        std::list<std::tuple<DesignEntity, std::string>>{},  //4
        std::list<std::tuple<DesignEntity, std::string>>{},   //5
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[7],
            source_tuples[8],
            source_tuples[9],
            source_tuples[10],
            source_tuples[11],
            source_tuples[12],
            source_tuples[13],
            source_tuples[14],
            source_tuples[15],
            source_tuples[16],
            source_tuples[17],},                                             //6
        std::list<std::tuple<DesignEntity, std::string>>{},      //7
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[9],
            source_tuples[10],
            source_tuples[11],
            source_tuples[12],
            source_tuples[13],
            source_tuples[14],
            source_tuples[15],
            source_tuples[16],
            source_tuples[17],
        },                                             //8
        std::list<std::tuple<DesignEntity, std::string>>{}, //9
        std::list<std::tuple<DesignEntity, std::string>>{},     //10
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[12],
            source_tuples[13],
            source_tuples[14],
            source_tuples[15],}, //11
        std::list<std::tuple<DesignEntity, std::string>>{},  //12
        std::list<std::tuple<DesignEntity, std::string>>{}, //13
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //14
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //15
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //16
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //17
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //18
        std::list<std::tuple<DesignEntity, std::string>>{
            source_tuples[20],
            source_tuples[21],},  //19
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //20
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //21
        std::list<std::tuple<DesignEntity, std::string>>{}, //22
        std::list<std::tuple<DesignEntity, std::string>>{},                                             //23
        std::list<std::tuple<DesignEntity,
                             std::string>>{}, //out of bounds check
    };

    // source program has 23 lines and the number 24 is for checking results of out of bounds input
    for (int i = 1; i <= 24; i++) {
      std::string stmt_no_str = std::to_string(i);
      std::list<std::tuple<DesignEntity, std::string>> expected_get_parentT_list = expected_get_parentT_lists[i - 1];
      std::list<std::tuple<DesignEntity, std::string>> actual_list = pkb->GetParentT(stmt_no_str);
      CHECK(actual_list == expected_get_parentT_list);
    }
  }

  SECTION("Uses statement") {
    // var on lhs
    CHECK(pkb->GetUses("1").empty());

    // same var on either side
    std::list<std::tuple<DesignEntity, std::string>> mappings2 = pkb->GetUses("12");
    std::tuple<DesignEntity, std::string> expected_tuple_var2 = std::make_tuple(DesignEntity::kVariable, "psubsystem");
    CHECK(mappings2.size() == 1);
    CHECK(mappings2.front() == expected_tuple_var2);

    // multiple var
    std::list<std::tuple<DesignEntity, std::string>>
        expected_list_var3 = std::list<std::tuple<DesignEntity, std::string>>{
        std::make_tuple(DesignEntity::kVariable, "chara"),
        std::make_tuple(DesignEntity::kVariable, "byte")
    };
    CHECK(pkb->GetUses("9") == expected_list_var3);

    // print
    std::list<std::tuple<DesignEntity, std::string>> mappings5 = pkb->GetUses("20");
    std::tuple<DesignEntity, std::string>
        expected_tuple_var5 = std::make_tuple(DesignEntity::kVariable, "pr0cessSuccessfuI");
    CHECK(mappings5.size() == 1);
    CHECK(mappings5.front() == expected_tuple_var5);

    // read
    CHECK(pkb->GetUses("7").empty());
  }

  SECTION("Uses container") {
    // 1 container
    std::list<std::tuple<DesignEntity, std::string>>
        expected_list_var1 = std::list<std::tuple<DesignEntity, std::string>>{
      std::make_tuple(DesignEntity::kVariable, "fileName"),
        std::make_tuple(DesignEntity::kVariable, "pr0cessSuccessfuI"),
        std::make_tuple(DesignEntity::kVariable, "SyntaxErr0rFound")
    };
    CHECK(pkb->GetUses("19") == expected_list_var1);

    // inner container
    std::list<std::tuple<DesignEntity, std::string>>
        expected_list_var2 = std::list<std::tuple<DesignEntity, std::string>>{
        std::make_tuple(DesignEntity::kVariable, "byte"),
        std::make_tuple(DesignEntity::kVariable, "psubsystem"),
        std::make_tuple(DesignEntity::kVariable, "ProcessStmtNow"),
        std::make_tuple(DesignEntity::kVariable, "cont1nueREad1ng"),
    };
    CHECK(pkb->GetUses("11") == expected_list_var2);

    // middle container
    std::list<std::tuple<DesignEntity, std::string>>
        expected_list_var3 = std::list<std::tuple<DesignEntity, std::string>>{
        std::make_tuple(DesignEntity::kVariable, "fileName"),
        std::make_tuple(DesignEntity::kVariable, "byte"),
        std::make_tuple(DesignEntity::kVariable, "chara"),
        std::make_tuple(DesignEntity::kVariable, "SyntaxErr0rFound"),
        std::make_tuple(DesignEntity::kVariable, "psubsystem"),
        std::make_tuple(DesignEntity::kVariable, "ProcessStmtNow"),
        std::make_tuple(DesignEntity::kVariable, "cont1nueREad1ng"),
    };
    CHECK(pkb->GetUses("6") == expected_list_var3);

    // outer container
    std::list<std::tuple<DesignEntity, std::string>>
        expected_list_var4 = std::list<std::tuple<DesignEntity, std::string>>{
        std::make_tuple(DesignEntity::kVariable, "fileName"),
        std::make_tuple(DesignEntity::kVariable, "InvalidFileName"),
        std::make_tuple(DesignEntity::kVariable, "byte"),
        std::make_tuple(DesignEntity::kVariable, "chara"),
        std::make_tuple(DesignEntity::kVariable, "SyntaxErr0rFound"),
        std::make_tuple(DesignEntity::kVariable, "psubsystem"),
        std::make_tuple(DesignEntity::kVariable, "ProcessStmtNow"),
        std::make_tuple(DesignEntity::kVariable, "cont1nueREad1ng"),
    };
    CHECK(pkb->GetUses("3") == expected_list_var4);
  }

  SECTION("Modifies statement") {
    // var on lhs
    std::list<std::tuple<DesignEntity, std::string>> mappings1 = pkb->GetModifies("1");
    std::tuple<DesignEntity, std::string> expected_tuple_var1 = std::make_tuple(DesignEntity::kVariable, "psubsystem");
    CHECK(mappings1.size() == 1);
    CHECK(mappings1.front() == expected_tuple_var1);

    // same var on either side
    std::list<std::tuple<DesignEntity, std::string>> mappings2 = pkb->GetModifies("12");
    std::tuple<DesignEntity, std::string> expected_tuple_var2 = std::make_tuple(DesignEntity::kVariable, "psubsystem");
    CHECK(mappings2.size() == 1);
    CHECK(mappings2.front() == expected_tuple_var2);

    // multiple var
    std::list<std::tuple<DesignEntity, std::string>>
    expected_list_var3 = std::list<std::tuple<DesignEntity, std::string>>{
      std::make_tuple(DesignEntity::kVariable, "chara")
    };
    CHECK(pkb->GetModifies("9") == expected_list_var3);

    // read
    std::list<std::tuple<DesignEntity, std::string>> mappings5 = pkb->GetModifies("7");
    std::tuple<DesignEntity, std::string>
    expected_tuple_var5 = std::make_tuple(DesignEntity::kVariable, "byte");
    CHECK(mappings5.size() == 1);
    CHECK(mappings5.front() == expected_tuple_var5);

    // print
    CHECK(pkb->GetModifies("20").empty());
  }

  SECTION("Modifies container") {
    // 1 container all print
    CHECK(pkb->GetModifies("19").empty());

    // inner container
    std::list<std::tuple<DesignEntity, std::string>>
    expected_list_var2 = std::list<std::tuple<DesignEntity, std::string>>{
      std::make_tuple(DesignEntity::kVariable, "psubsystem"),
      std::make_tuple(DesignEntity::kVariable, "chara"),
      };
    CHECK(pkb->GetModifies("11") == expected_list_var2);

    // middle container
    std::list<std::tuple<DesignEntity, std::string>>
    expected_list_var3 = std::list<std::tuple<DesignEntity, std::string>>{
      std::make_tuple(DesignEntity::kVariable, "byte"),
      std::make_tuple(DesignEntity::kVariable, "chara"),
      std::make_tuple(DesignEntity::kVariable, "lastByte"),
      std::make_tuple(DesignEntity::kVariable, "fileName"),
      std::make_tuple(DesignEntity::kVariable, "psubsystem"),
      };
    std::list<std::tuple<DesignEntity, std::string>> actual_list1 = pkb->GetModifies("6");
    CHECK(actual_list1 == expected_list_var3);

    // outer container
    std::list<std::tuple<DesignEntity, std::string>>
    expected_list_var4 = std::list<std::tuple<DesignEntity, std::string>>{
      std::make_tuple(DesignEntity::kVariable, "chara"),
      std::make_tuple(DesignEntity::kVariable, "byte"),
      std::make_tuple(DesignEntity::kVariable, "lastByte"),
      std::make_tuple(DesignEntity::kVariable, "fileName"),
      std::make_tuple(DesignEntity::kVariable, "psubsystem"),
      };
    std::list<std::tuple<DesignEntity, std::string>> actual_list2 = pkb->GetModifies("3");
    CHECK(actual_list2 == expected_list_var4);
  }

  // todo: test nesting in else block
  // todo: test reverse relationships
}