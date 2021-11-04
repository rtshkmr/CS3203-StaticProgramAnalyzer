#include "catch.hpp"
#include "component/SourceProcessor/Extractors/TransitiveExtractor.h"
#include "../../../../../utils/EntityUtils.h"
#include "../../../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("1.FollowsTExtractor.Extract FollowsT basic conditions") {
  auto* deliverable = new Deliverable();
  ReadEntity* read_x_ = GetReadX();
  ReadEntity* read_y_ = GetReadY();
  ReadEntity* read_z_ = GetReadZ();
  ReadEntity* read_i_ = GetReadI();
  ReadEntity* read_n_ = GetReadN();
  ReadEntity* read_m_ = GetReadM();
  PrintEntity* print_x_ = GetPrintX();
  PrintEntity* print_y_ = GetPrintY();
  PrintEntity* print_z_ = GetPrintZ();
  PrintEntity* print_i_ = GetPrintI();
  PrintEntity* print_n_ = GetPrintN();
  PrintEntity* print_m_ = GetPrintM();
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_2_ = GetAssign2();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_7_ = GetAssign7();
  AssignEntity* assign_8_ = GetAssign8();
  AssignEntity* assign_9_ = GetAssign9();
  AssignEntity* assign_10_ = GetAssign10();
  IfEntity* if_1_ = GetIf1();
  IfEntity* if_2_ = GetIf2();
  IfEntity* if_3_ = GetIf3();
  IfEntity* if_4_ = GetIf4();
  ElseEntity* else_1_ = GetElse();
  ElseEntity* else_2_ = GetElse();
  ElseEntity* else_3_ = GetElse();
  ElseEntity* else_4_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity3();
  WhileEntity* while_2_ = GetWhileEntity2();
  WhileEntity* while_3_ = GetWhileEntity3();

  SECTION("Single level statement list") {
    /*
     * procedure proc1 {
     *  assign
     *  assign
     *  read
     *  print
     *  assign
     *  assign
     * }
     */
    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    TestUtils::AddStatementList(proc1, {assign_1_, assign_3_, read_x_, print_y_, assign_4_, assign_5_});

    deliverable->AddFollowRelationship(assign_1_, assign_3_);
    deliverable->AddFollowRelationship(assign_3_, read_x_);
    deliverable->AddFollowRelationship(read_x_, print_y_);
    deliverable->AddFollowRelationship(print_y_, assign_4_);
    deliverable->AddFollowRelationship(assign_4_, assign_5_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*>
        expected_follows_1 = std::list<Statement*>{assign_3_, read_x_, print_y_, assign_4_, assign_5_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{read_x_, print_y_, assign_4_, assign_5_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(3, _)
    std::list<Statement*> expected_follows_3 = std::list<Statement*>{print_y_, assign_4_, assign_5_};
    std::list<Statement*>* actual_follows_3 = deliverable->follows_T_hash_.find(read_x_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_3, expected_follows_3));
    // Follows*(4, _)
    std::list<Statement*> expected_follows_4 = std::list<Statement*>{assign_4_, assign_5_};
    std::list<Statement*>* actual_follows_4 = deliverable->follows_T_hash_.find(print_y_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_4, expected_follows_4));
    // Follows*(5, _)
    std::list<Statement*> expected_follows_5 = std::list<Statement*>{assign_5_};
    std::list<Statement*>* actual_follows_5 = deliverable->follows_T_hash_.find(assign_4_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_5, expected_follows_5));
    // Follows*(6, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(assign_5_) == fth.end());
  }

  SECTION("Statement list with if") {
    /*
     * procedure proc1 {
     *  1. if
     *  2.   assign
     *  3.   read
     *  4.   assign
     *     else
     *  5.   assign
     *  6.   assign
     *  7.   assign
     *  8. print
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(proc2, {if_1_, print_y_});
    TestUtils::AddStatementList(else_1_, {assign_5_, assign_6_, assign_7_});
    TestUtils::AddStatementList(if_1_, {assign_1_, read_y_, assign_3_});
    if_1_->SetElseEntity(else_1_);

    deliverable->AddFollowRelationship(assign_1_, read_y_);
    deliverable->AddFollowRelationship(read_y_, assign_3_);
    deliverable->AddFollowRelationship(if_1_, print_y_);
    deliverable->AddFollowRelationship(assign_5_, assign_6_);
    deliverable->AddFollowRelationship(assign_6_, assign_7_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{print_y_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{read_y_, assign_3_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(4, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(assign_3_) == fth.end());
    // Follows*(5, _)
    std::list<Statement*> expected_follows_5 = std::list<Statement*>{assign_6_, assign_7_};
    std::list<Statement*>* actual_follows_5 = deliverable->follows_T_hash_.find(assign_5_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_5, expected_follows_5));
    // Follows*(7, _)
    CHECK(fth.find(assign_7_) == fth.end());
    // Follows*(8, _)
    CHECK(fth.find(print_y_) == fth.end());
  }

  SECTION("Statement list with while") {
    /*
     * procedure proc2 {
     * 1. assign
     * 2. print
     * 3. while
     * 4.   read
     * 5.   assign
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(while_3_, {read_z_, assign_5_});
    TestUtils::AddStatementList(proc2, {assign_1_, print_i_, while_3_});

    deliverable->AddFollowRelationship(assign_1_, print_i_);
    deliverable->AddFollowRelationship(print_i_, while_3_);
    deliverable->AddFollowRelationship(read_z_, assign_5_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{print_i_, while_3_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(3, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(while_3_) == fth.end());
    // Follows*(4, _)
    std::list<Statement*> expected_follows_4 = std::list<Statement*>{assign_5_};
    std::list<Statement*>* actual_follows_4 = deliverable->follows_T_hash_.find(read_z_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_4, expected_follows_4));
    // Follows*(5, _)
    CHECK(fth.find(assign_5_) == fth.end());
  }

  SECTION("Multiple procedures") {
    // proc with call container
    /*
     * procedure proc3 {
     * 1. assign
     * 2. call proc4
     * 3. print
     * }
     * procedure proc4 {
     * 4. assign
     * 5. assign
     * 6. assign
     * }
     */
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    auto* call4 = new CallEntity(proc4);
    TestUtils::AddStatementList(proc3, {assign_1_, call4, print_y_});
    TestUtils::AddStatementList(proc4, {assign_4_, assign_5_, assign_6_});

    deliverable->AddFollowRelationship(assign_1_, call4);
    deliverable->AddFollowRelationship(call4, print_y_);
    deliverable->AddFollowRelationship(assign_4_, assign_5_);
    deliverable->AddFollowRelationship(assign_5_, assign_6_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{call4, print_y_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(3, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(print_y_) == fth.end());
    // Follows*(4, _)
    std::list<Statement*> expected_follows_4 = std::list<Statement*>{assign_5_, assign_6_};
    std::list<Statement*>* actual_follows_4 = deliverable->follows_T_hash_.find(assign_4_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_4, expected_follows_4));
    //Follows*(6, _)
    CHECK(fth.find(assign_6_) == fth.end());
  }
}

TEST_CASE("1.FollowsTExtractor.Extract FollowsT nested containers") {
  auto* deliverable = new Deliverable();
  ReadEntity* read_x_ = GetReadX();
  ReadEntity* read_y_ = GetReadY();
  ReadEntity* read_z_ = GetReadZ();
  ReadEntity* read_i_ = GetReadI();
  ReadEntity* read_n_ = GetReadN();
  ReadEntity* read_m_ = GetReadM();
  PrintEntity* print_x_ = GetPrintX();
  PrintEntity* print_y_ = GetPrintY();
  PrintEntity* print_z_ = GetPrintZ();
  PrintEntity* print_i_ = GetPrintI();
  PrintEntity* print_n_ = GetPrintN();
  PrintEntity* print_m_ = GetPrintM();
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_2_ = GetAssign2();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_7_ = GetAssign7();
  AssignEntity* assign_8_ = GetAssign8();
  AssignEntity* assign_9_ = GetAssign9();
  AssignEntity* assign_10_ = GetAssign10();
  IfEntity* if_1_ = GetIf1();
  IfEntity* if_2_ = GetIf2();
  IfEntity* if_3_ = GetIf3();
  IfEntity* if_4_ = GetIf4();
  ElseEntity* else_1_ = GetElse();
  ElseEntity* else_2_ = GetElse();
  ElseEntity* else_3_ = GetElse();
  ElseEntity* else_4_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity3();
  WhileEntity* while_2_ = GetWhileEntity2();
  WhileEntity* while_3_ = GetWhileEntity3();
  SECTION("Nested if x3") {
    /*
     * procedure proc1 {
     *  1. if1
     *  2.    assign1
     *  3.    readx
     *  4.    assign2
     *  5.    if2
     *  6.        assign3
     *  7.        assign4
     *  8.        ready
     *        else2
     *  9.        printx
     *  10.       printy
     *     else1
     *  11.    assign5
     *  12.    if3
     *  13.      printz
     *  14.      assign6
     *  15.      assign7
     *         else3
     *  16.      if4
     *  17.        assign8
     *  18.        assign9
     *  19.        assign10
     *           else4
     *  20.        readz
     *  21.        readi
     *  22.        readn
     *  23.   printi
     *  24. printn
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(else_4_, {read_z_, read_i_, read_n_});
    TestUtils::AddStatementList(if_4_, {assign_8_, assign_9_, assign_10_});
    if_4_->SetElseEntity(else_4_);
    TestUtils::AddStatementList(else_3_, {if_4_});
    TestUtils::AddStatementList(if_3_, {print_z_, assign_6_, assign_7_});
    if_3_->SetElseEntity(else_3_);
    TestUtils::AddStatementList(else_1_, {assign_5_, print_i_});
    TestUtils::AddStatementList(else_2_, {print_x_, print_y_});
    TestUtils::AddStatementList(if_2_, {assign_3_, assign_4_, read_y_});
    if_2_->SetElseEntity(else_2_);
    TestUtils::AddStatementList(if_1_, {assign_1_, read_x_, assign_2_, if_2_});
    if_1_->SetElseEntity(else_1_);
    TestUtils::AddStatementList(proc2, {if_1_, print_n_});

    deliverable->AddFollowRelationship(if_1_, print_n_);
    deliverable->AddFollowRelationship(assign_1_, read_x_);
    deliverable->AddFollowRelationship(read_x_, assign_2_);
    deliverable->AddFollowRelationship(assign_2_, if_2_);

    deliverable->AddFollowRelationship(assign_3_, assign_4_);
    deliverable->AddFollowRelationship(assign_4_, read_y_);

    deliverable->AddFollowRelationship(print_x_, print_y_);

    deliverable->AddFollowRelationship(assign_5_, if_3_);
    deliverable->AddFollowRelationship(if_3_, print_i_);

    deliverable->AddFollowRelationship(print_z_, assign_6_);
    deliverable->AddFollowRelationship(assign_6_, assign_7_);

    deliverable->AddFollowRelationship(assign_8_, assign_9_);
    deliverable->AddFollowRelationship(assign_9_, assign_10_);

    deliverable->AddFollowRelationship(read_z_, read_i_);
    deliverable->AddFollowRelationship(read_i_, read_n_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{print_n_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{read_x_, assign_2_, if_2_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(6, _)
    std::list<Statement*> expected_follows_6 = std::list<Statement*>{assign_4_, read_y_};
    std::list<Statement*>* actual_follows_6 = deliverable->follows_T_hash_.find(assign_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_6, expected_follows_6));
    // Follows*(9, _)
    std::list<Statement*> expected_follows_9 = std::list<Statement*>{print_y_};
    std::list<Statement*>* actual_follows_9 = deliverable->follows_T_hash_.find(print_x_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_9, expected_follows_9));
    // Follows*(11, _)
    std::list<Statement*> expected_follows_11 = std::list<Statement*>{if_3_, print_i_};
    std::list<Statement*>* actual_follows_11 = deliverable->follows_T_hash_.find(assign_5_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_11, expected_follows_11));
    // Follows*(13, _)
    std::list<Statement*> expected_follows_13 = std::list<Statement*>{assign_6_, assign_7_};
    std::list<Statement*>* actual_follows_13 = deliverable->follows_T_hash_.find(print_z_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_13, expected_follows_13));
    // Follows*(16, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(if_4_) == fth.end());
    // Follows*(17, _)
    std::list<Statement*> expected_follows_17 = std::list<Statement*>{assign_9_, assign_10_};
    std::list<Statement*>* actual_follows_17 = deliverable->follows_T_hash_.find(assign_8_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_17, expected_follows_17));
    // Follows*(20, _)
    std::list<Statement*> expected_follows_20 = std::list<Statement*>{read_i_, read_n_};
    std::list<Statement*>* actual_follows_20 = deliverable->follows_T_hash_.find(read_z_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_20, expected_follows_20));

    // Follows*(24, _)
    CHECK(fth.find(print_n_) == fth.end());
  }

  SECTION("Nested while x3") {
    // proc with while container
    /*
     * procedure proc3 {
     * 1. while1
     * 2.   assign1
     * 3.   assign2
     * 4.   while2
     * 5.     assign3
     * 6.     assign4
     * 7.     while3
     * 8.       assign5
     * 9.       assign6
     * 10.      assign7
     * 11.    readx
     * 12.  ready
     * 13.readz
     * }
     */
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    TestUtils::AddStatementList(while_3_, {assign_5_, assign_6_, assign_7_});
    TestUtils::AddStatementList(while_2_, {assign_3_, assign_4_, while_3_, read_x_});
    TestUtils::AddStatementList(while_1_, {assign_1_, assign_2_, while_2_, read_y_});
    TestUtils::AddStatementList(proc3, {while_1_, read_z_});

    deliverable->AddFollowRelationship(while_1_, read_z_);
    deliverable->AddFollowRelationship(assign_1_, assign_2_);
    deliverable->AddFollowRelationship(assign_2_, while_2_);
    deliverable->AddFollowRelationship(while_2_, read_y_);

    deliverable->AddFollowRelationship(assign_3_, assign_4_);
    deliverable->AddFollowRelationship(assign_4_, while_3_);
    deliverable->AddFollowRelationship(while_3_, read_x_);

    deliverable->AddFollowRelationship(assign_5_, assign_6_);
    deliverable->AddFollowRelationship(assign_6_, assign_7_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{read_z_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(while_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{assign_2_, while_2_, read_y_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(5, _)
    std::list<Statement*> expected_follows_5 = std::list<Statement*>{assign_4_, while_3_, read_x_};
    std::list<Statement*>* actual_follows_5 = deliverable->follows_T_hash_.find(assign_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_5, expected_follows_5));
    // Follows*(8, _)
    std::list<Statement*> expected_follows_8 = std::list<Statement*>{assign_6_, assign_7_};
    std::list<Statement*>* actual_follows_8 = deliverable->follows_T_hash_.find(assign_5_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_8, expected_follows_8));
    // Follows*(11, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(read_x_) == fth.end());
    // Follows*(12, _)
    CHECK(fth.find(read_y_) == fth.end());
    // Follows*(13, _)
    CHECK(fth.find(read_z_) == fth.end());
  }

  SECTION("If and while") {
    /*
     * procedure proc1 {
     *  1.if1
     *  2.  assign1
     *  3.  assign2
     *  4.  assign3
     *    else1
     *  5.  assign4
     *  6.  assign5
     *  7.  assign6
     *  8.while1
     *  9.  printx
     *  10.  printy
     *  11.  printz
     * }
     */
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    TestUtils::AddStatementList(else_1_, {assign_4_, assign_5_, assign_6_});
    TestUtils::AddStatementList(if_4_, {assign_1_, assign_2_, assign_3_});
    if_4_->SetElseEntity(else_1_);
    TestUtils::AddStatementList(while_1_, {print_x_, print_y_, print_z_});
    TestUtils::AddStatementList(proc4, {if_4_, while_1_});

    deliverable->AddFollowRelationship(if_4_, while_1_);
    deliverable->AddFollowRelationship(assign_1_, assign_2_);
    deliverable->AddFollowRelationship(assign_2_, assign_3_);

    deliverable->AddFollowRelationship(assign_4_, assign_5_);
    deliverable->AddFollowRelationship(assign_5_, assign_6_);

    deliverable->AddFollowRelationship(print_x_, print_y_);
    deliverable->AddFollowRelationship(print_y_, print_z_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{while_1_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(if_4_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{assign_5_, assign_6_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_4_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(5, _)
    std::list<Statement*> expected_follows_5 = std::list<Statement*>{print_y_, print_z_};
    std::list<Statement*>* actual_follows_5 = deliverable->follows_T_hash_.find(print_x_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_5, expected_follows_5));

    // Follows*(4, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(assign_3_) == fth.end());
    // Follows*(7, _)
    CHECK(fth.find(assign_6_) == fth.end());
    // Follows*(8, _)
    CHECK(fth.find(while_1_) == fth.end());
  }

  SECTION("Multiple nesting of if and while") {
    /*
     * procedure proc1 {
     *  1.if1
     *  2.  assign1
     *  3.  assign2
     *  4.  while1
     *  5.    readx
     *  6.    ready
     *  7.    readz
     *    else1
     *  8.  while2
     *  9.      readi
     *  10.     readn
     *  11.     readm
     *  12.  assign5
     *  13.  assign6
     *  14.while3
     *  15.    if2
     *  16.      assign7
     *  17.      assign8
     *  18.      assign9
     *  19.      else2
     *  20.        printi
     *  21.        printn
     *  22.        printm
     *  23.  printx
     *  24.  printy
     *  25.  printz
     * }
     */
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    TestUtils::AddStatementList(else_2_, {print_i_, print_n_, print_m_});
    TestUtils::AddStatementList(if_2_, {assign_7_, assign_8_, assign_9_});
    TestUtils::AddStatementList(while_3_, {if_2_, print_x_, print_y_, print_z_});
    TestUtils::AddStatementList(while_2_, {read_i_, read_n_, read_m_});
    TestUtils::AddStatementList(else_1_, {while_2_, assign_5_, assign_6_});
    TestUtils::AddStatementList(while_1_, {read_x_, read_y_, read_z_});
    TestUtils::AddStatementList(if_1_, {assign_1_, assign_2_, while_1_});
    if_1_->SetElseEntity(else_1_);
    if_2_->SetElseEntity(else_2_);
    TestUtils::AddStatementList(proc4, {if_1_, while_3_});

    deliverable->AddFollowRelationship(if_1_, while_3_);
    deliverable->AddFollowRelationship(assign_1_, assign_2_);
    deliverable->AddFollowRelationship(assign_2_, while_1_);

    deliverable->AddFollowRelationship(read_x_, read_y_);
    deliverable->AddFollowRelationship(read_y_, read_z_);

    deliverable->AddFollowRelationship(while_2_, assign_5_);
    deliverable->AddFollowRelationship(assign_5_, assign_6_);

    deliverable->AddFollowRelationship(read_i_, read_n_);
    deliverable->AddFollowRelationship(read_n_, read_m_);

    deliverable->AddFollowRelationship(assign_7_, assign_8_);
    deliverable->AddFollowRelationship(assign_8_, assign_9_);

    deliverable->AddFollowRelationship(print_i_, print_n_);
    deliverable->AddFollowRelationship(print_n_, print_m_);

    deliverable->AddFollowRelationship(if_2_, print_x_);
    deliverable->AddFollowRelationship(print_x_, print_y_);
    deliverable->AddFollowRelationship(print_y_, print_z_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetFollowsTMap(),
                                deliverable->GetFollowedByTMap(),
                                deliverable->GetFollowsMap());

    // Follows*(1, _)
    std::list<Statement*> expected_follows_1 = std::list<Statement*>{while_3_};
    std::list<Statement*>* actual_follows_1 = deliverable->follows_T_hash_.find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_1, expected_follows_1));
    // Follows*(2, _)
    std::list<Statement*> expected_follows_2 = std::list<Statement*>{assign_2_, while_1_};
    std::list<Statement*>* actual_follows_2 = deliverable->follows_T_hash_.find(assign_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_2, expected_follows_2));
    // Follows*(5, _)
    std::list<Statement*> expected_follows_5 = std::list<Statement*>{read_y_, read_z_};
    std::list<Statement*>* actual_follows_5 = deliverable->follows_T_hash_.find(read_x_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_5, expected_follows_5));
    // Follows*(8, _)
    std::list<Statement*> expected_follows_8 = std::list<Statement*>{assign_5_, assign_6_};
    std::list<Statement*>* actual_follows_8 = deliverable->follows_T_hash_.find(while_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_8, expected_follows_8));
    // Follows*(9, _)
    std::list<Statement*> expected_follows_9 = std::list<Statement*>{read_n_, read_m_};
    std::list<Statement*>* actual_follows_9 = deliverable->follows_T_hash_.find(read_i_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_9, expected_follows_9));
    // Follows*(15, _)
    std::list<Statement*> expected_follows_15 = std::list<Statement*>{print_x_, print_y_, print_z_};
    std::list<Statement*>* actual_follows_15 = deliverable->follows_T_hash_.find(if_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_15, expected_follows_15));
    // Follows*(16, _)
    std::list<Statement*> expected_follows_16 = std::list<Statement*>{assign_8_, assign_9_};
    std::list<Statement*>* actual_follows_16 = deliverable->follows_T_hash_.find(assign_7_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_16, expected_follows_16));
    // Follows*(20, _)
    std::list<Statement*> expected_follows_20 = std::list<Statement*>{print_n_, print_m_};
    std::list<Statement*>* actual_follows_20 = deliverable->follows_T_hash_.find(print_i_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_follows_20, expected_follows_20));

    // Follows*(4, _)
    std::unordered_map<Statement*, std::list<Statement*>*> fth = deliverable->follows_T_hash_;
    CHECK(fth.find(while_1_) == fth.end());
    // Follows*(7, _)
    CHECK(fth.find(read_z_) == fth.end());
    // Follows*(11, _)
    CHECK(fth.find(read_m_) == fth.end());
    // Follows*(13, _)
    CHECK(fth.find(assign_6_) == fth.end());
    // Follows*(18, _)
    CHECK(fth.find(assign_9_) == fth.end());
    // Follows*(22, _)
    CHECK(fth.find(print_m_) == fth.end());
    // Follows*(25, _)
    CHECK(fth.find(print_z_) == fth.end());
  }
}
