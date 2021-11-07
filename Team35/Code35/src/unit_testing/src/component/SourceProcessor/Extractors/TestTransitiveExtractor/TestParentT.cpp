#include "catch.hpp"
#include "component/SourceProcessor/Extractors/TransitiveExtractor.h"
#include "../../../../../utils/EntityUtils.h"
#include "../../../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("1.TransitiveExtractor.Extract ParentT basic conditions") {
  auto* deliverable = new Deliverable();
  ReadEntity* read_x_ = GetReadX();
  ReadEntity* read_y_ = GetReadY();
  ReadEntity* read_z_ = GetReadZ();
  PrintEntity* print_y_ = GetPrintY();
  PrintEntity* print_i_ = GetPrintI();
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_2_ = GetAssign2();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  IfEntity* if_1_ = GetIf1();
  IfEntity* if_2_ = GetIf2();
  ElseEntity* else_1_ = GetElse();
  ElseEntity* else_2_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity3();
  WhileEntity* while_3_ = GetWhileEntity3();

  SECTION("Single level statement list") {
    /*
     * procedure proc1 {
     *  assign
     *  assign
     *  read
     *  print
     * }
     */
    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    TestUtils::AddStatementList(proc1, {assign_1_, assign_3_, read_x_, print_y_, assign_4_, assign_5_});
    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetChildMap());

    // no parent
    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    CHECK(ptct->empty());
  }

  SECTION("Statement list with if") {
    /*
     * procedure proc1 {
     *  1. if
     *  2.   assign
     *  3.   read
     *     else
     *  4.   assign
     *  5.   assign
     *  6. print
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(else_1_, {assign_5_, assign_6_});
    TestUtils::AddStatementList(if_1_, {assign_1_, read_y_});
    TestUtils::AddStatementList(proc2, {if_1_, print_y_});
    if_1_->SetElseEntity(else_1_);
    deliverable->AddParentRelationship(if_1_, assign_1_);
    deliverable->AddParentRelationship(if_1_, read_y_);
    deliverable->AddParentRelationship(if_1_, assign_5_);
    deliverable->AddParentRelationship(if_1_, assign_6_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(1, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{assign_1_, read_y_, assign_5_, assign_6_};
    std::list<Statement*>* actual_children_1 = ptct->find(if_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    CHECK(ptct->size() == 1);
  }

  SECTION("Statement list with while") {
    /*
     * procedure proc2 {
     * 1. assign
     * 2. while
     * 3.   read
     * 4.   assign
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(while_3_, {read_z_, assign_5_});
    TestUtils::AddStatementList(proc2, {assign_1_, print_i_, while_3_});

    deliverable->AddParentRelationship(while_3_, read_z_);
    deliverable->AddParentRelationship(while_3_, assign_5_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());
    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(2, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{read_z_, assign_5_};
    std::list<Statement*>* actual_children_1 = ptct->find(while_3_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    CHECK(ptct->size() == 1);
  }

  SECTION("Multiple procedures") {
    // proc with call container
    /*
     * procedure proc3 {
     * 1. assign
     * 2. call proc4
     * 3. if
     * 4.   assign
     * 5. else
     * 6.   read
     * }
     * procedure proc4 {
     * 7. while
     * 8.   print
     * 9. assign
     * }
     */
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    auto* call4 = new CallEntity(proc4);
    TestUtils::AddStatementList(else_2_, {read_y_});
    TestUtils::AddStatementList(if_2_, {assign_2_});
    TestUtils::AddStatementList(proc3, {assign_1_, call4, if_2_});
    TestUtils::AddStatementList(while_1_, {print_y_});
    TestUtils::AddStatementList(proc4, {while_1_, assign_4_});
    if_2_->SetElseEntity(else_2_);
    deliverable->AddParentRelationship(if_2_, assign_2_);
    deliverable->AddParentRelationship(if_2_, read_y_);
    deliverable->AddParentRelationship(while_1_, print_y_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(3, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{assign_2_, read_y_};
    std::list<Statement*>* actual_children_1 = ptct->find(if_2_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(7, _)
    std::list<Statement*> expected_children_2 = std::list<Statement*>{print_y_};
    std::list<Statement*>* actual_children_2 = ptct->find(while_1_)->second;
    CHECK(*actual_children_2 == expected_children_2);
    CHECK(ptct->size() == 2);
  }
}

TEST_CASE("1.TransitiveExtractor.Extract ParentT nested containers") {
  auto* deliverable = new Deliverable();
  ReadEntity* read_x_ = GetReadX();
  ReadEntity* read_y_ = GetReadY();
  ReadEntity* read_z_ = GetReadZ();
  PrintEntity* print_x_ = GetPrintX();
  PrintEntity* print_i_ = GetPrintI();
  PrintEntity* print_n_ = GetPrintN();
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_2_ = GetAssign2();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  IfEntity* if_1_ = GetIf1();
  IfEntity* if_2_ = GetIf2();
  IfEntity* if_3_ = GetIf3();
  ElseEntity* else_1_ = GetElse();
  ElseEntity* else_2_ = GetElse();
  ElseEntity* else_3_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity3();
  WhileEntity* while_2_ = GetWhileEntity2();
  WhileEntity* while_3_ = GetWhileEntity3();

  SECTION("Nested if x3") {
    /*
     * procedure proc1 {
     *  1. if1
     *  2.    assign1
     *     else1
     *  3.    assign3
     *  4.    if2
     *  5.      assign4
     *        else2
     *  6.      if3
     *  7.        assign5
     *          else3
     *  8.        readz
     *  9.    printi
     *  10.printn
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(else_3_, {read_z_});
    TestUtils::AddStatementList(if_3_, {assign_5_});
    TestUtils::AddStatementList(else_2_, {if_3_});
    TestUtils::AddStatementList(if_2_, {assign_4_});
    TestUtils::AddStatementList(else_1_, {assign_3_, if_2_, print_i_});
    TestUtils::AddStatementList(if_1_, {assign_1_});
    TestUtils::AddStatementList(proc2, {if_1_, print_n_});
    if_1_->SetElseEntity(else_1_);
    if_2_->SetElseEntity(else_2_);
    if_3_->SetElseEntity(else_3_);
    deliverable->AddParentRelationship(if_1_, assign_1_);
    deliverable->AddParentRelationship(if_1_, assign_3_);
    deliverable->AddParentRelationship(if_1_, if_2_);
    deliverable->AddParentRelationship(if_1_, print_i_);
    deliverable->AddParentRelationship(if_2_, assign_4_);
    deliverable->AddParentRelationship(if_2_, if_3_);
    deliverable->AddParentRelationship(if_3_, assign_5_);
    deliverable->AddParentRelationship(if_3_, read_z_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, assign_3_, if_2_, assign_4_, if_3_, assign_5_, read_z_, print_i_};
    std::list<Statement*>* actual_children_1 = ptct->find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_1, expected_children_1));
    // Parent*(3, _)
    CHECK(ptct->find(assign_3_) == ptct->end());
    // Parent*(4, _)
    std::list<Statement*> expected_children_4 = std::list<Statement*>{assign_4_, if_3_, assign_5_, read_z_};
    std::list<Statement*>* actual_children_4 = ptct->find(if_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_4, expected_children_4));
    // Parent*(6, _)
    std::list<Statement*> expected_children_6 = std::list<Statement*>{assign_5_, read_z_};
    std::list<Statement*>* actual_children_6 = ptct->find(if_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_6, expected_children_6));

    CHECK(ptct->size() == 3);
  }

  SECTION("Nested if x3 #2") {
    /*
     * procedure proc1 {
     *  1. if1
     *  2.    assign1
     *  3.    if2
     *  4.      if3
     *  5.        assign5
     *          else3
     *  6.        readz
     *        else2
     *  7.      assign4
     *  8.    printi
     *     else1
     *  9.    assign3
     *  10.printn
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    TestUtils::AddStatementList(else_3_, {read_z_});
    TestUtils::AddStatementList(if_3_, {assign_5_});
    TestUtils::AddStatementList(else_2_, {assign_4_});
    TestUtils::AddStatementList(if_2_, {if_3_});
    TestUtils::AddStatementList(else_1_, {assign_3_});
    TestUtils::AddStatementList(if_1_, {assign_1_, if_2_, print_i_});
    TestUtils::AddStatementList(proc2, {if_1_, print_n_});
    if_1_->SetElseEntity(else_1_);
    if_2_->SetElseEntity(else_2_);
    if_3_->SetElseEntity(else_3_);
    deliverable->AddParentRelationship(if_1_, assign_1_);
    deliverable->AddParentRelationship(if_1_, if_2_);
    deliverable->AddParentRelationship(if_1_, print_i_);
    deliverable->AddParentRelationship(if_1_, assign_3_);
    deliverable->AddParentRelationship(if_2_, if_3_);
    deliverable->AddParentRelationship(if_2_, assign_4_);
    deliverable->AddParentRelationship(if_3_, assign_5_);
    deliverable->AddParentRelationship(if_3_, read_z_);

    auto statement_extractor = TransitiveExtractor<Statement>{};
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, if_2_, if_3_, assign_5_, read_z_, assign_4_, print_i_, assign_3_};
    std::list<Statement*>* actual_children_1 = ptct->find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_1, expected_children_1));
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{if_3_, assign_5_, read_z_, assign_4_};
    std::list<Statement*>* actual_children_3 = ptct->find(if_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_3, expected_children_3));
    // Parent*(4, _)
    std::list<Statement*> expected_children_4 = std::list<Statement*>{assign_5_, read_z_};
    std::list<Statement*>* actual_children_4 = ptct->find(if_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_4, expected_children_4));
    // Parent*(9, _)
    CHECK(ptct->find(assign_3_) == ptct->end());

    CHECK(ptct->size() == 3);
  }

  SECTION("Nested while x3") {
    // proc with while container
    /*
     * procedure proc3 {
     * 1. while1
     * 2.   assign1
     * 3.   while2
     * 4.     assign3
     * 5.     while3
     * 6.       assign5
     * 7.     readx
     * 8.   ready
     * 9. readz
     * }
     */
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    TestUtils::AddStatementList(while_3_, {assign_5_});
    TestUtils::AddStatementList(while_2_, {assign_3_, while_3_, read_x_});
    TestUtils::AddStatementList(while_1_, {assign_1_, while_2_, read_y_});
    TestUtils::AddStatementList(proc3, {while_1_, read_z_});

    deliverable->AddParentRelationship(while_1_, assign_1_);
    deliverable->AddParentRelationship(while_1_, while_2_);
    deliverable->AddParentRelationship(while_1_, read_y_);
    deliverable->AddParentRelationship(while_2_, assign_3_);
    deliverable->AddParentRelationship(while_2_, while_3_);
    deliverable->AddParentRelationship(while_2_, read_x_);
    deliverable->AddParentRelationship(while_3_, assign_5_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, while_2_, assign_3_, while_3_, assign_5_, read_x_, read_y_};
    std::list<Statement*>* actual_children_1 = ptct->find(while_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_1, expected_children_1));
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{assign_3_, while_3_, assign_5_, read_x_};
    std::list<Statement*>* actual_children_3 = ptct->find(while_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_3, expected_children_3));
    // Parent*(5, _)
    std::list<Statement*> expected_children_5 = std::list<Statement*>{assign_5_};
    std::list<Statement*>* actual_children_5 = ptct->find(while_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_5, expected_children_5));
    // Parent*(8, _)
    CHECK(ptct->find(read_y_) == ptct->end());

    CHECK(ptct->size() == 3);
  }

  SECTION("Multiple nesting of if and while") {
    /*
     * procedure proc1 {
     *  1.if1
     *  2.  assign1
     *  3.  while1
     *  4.    readx
     *    else1
     *  5.  while2
     *  6.    ready
     *  7.  assign6
     *  8.while3
     *  9.  printx
     *  10.  if2
     *  11.    assign2
     *  12.  else2
     *  13.    assign3
     * }
     */
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    TestUtils::AddStatementList(else_2_, {assign_3_});
    TestUtils::AddStatementList(if_2_, {assign_2_});
    TestUtils::AddStatementList(while_3_, {print_x_, if_2_});
    TestUtils::AddStatementList(while_2_, {read_y_});
    TestUtils::AddStatementList(while_1_, {read_x_});
    TestUtils::AddStatementList(else_1_, {while_2_, assign_6_});
    TestUtils::AddStatementList(if_1_, {assign_1_, while_1_});
    TestUtils::AddStatementList(proc4, {if_1_, while_3_});
    if_1_->SetElseEntity(else_1_);
    if_2_->SetElseEntity(else_2_);

    deliverable->AddParentRelationship(if_1_, assign_1_);
    deliverable->AddParentRelationship(if_1_, while_1_);
    deliverable->AddParentRelationship(if_1_, while_2_);
    deliverable->AddParentRelationship(if_1_, assign_6_);
    deliverable->AddParentRelationship(while_1_, read_x_);
    deliverable->AddParentRelationship(while_2_, read_y_);

    deliverable->AddParentRelationship(if_2_, assign_2_);
    deliverable->AddParentRelationship(if_2_, assign_3_);

    deliverable->AddParentRelationship(while_3_, print_x_);
    deliverable->AddParentRelationship(while_3_, if_2_);

    auto statement_extractor = TransitiveExtractor<Statement>();
    statement_extractor.Extract(deliverable->GetParentTMap(),
                                deliverable->GetChildTMap(),
                                deliverable->GetParentMap());

    std::unordered_map<Statement*, std::list<Statement*>*>* ptct = deliverable->GetParentTMap();
    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, while_1_, read_x_, while_2_, read_y_, assign_6_};
    std::list<Statement*>* actual_children_1 = ptct->find(if_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_1, expected_children_1));
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{read_x_};
    std::list<Statement*>* actual_children_3 = ptct->find(while_1_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_3, expected_children_3));
    // Parent*(5, _)
    std::list<Statement*> expected_children_5 = std::list<Statement*>{read_y_};
    std::list<Statement*>* actual_children_5 = ptct->find(while_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_5, expected_children_5));
    // Parent*(8, _)
    std::list<Statement*> expected_children_8 = std::list<Statement*>{print_x_, if_2_, assign_2_, assign_3_};
    std::list<Statement*>* actual_children_8 = ptct->find(while_3_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_8, expected_children_8));
    // Parent*(10, _)
    std::list<Statement*> expected_children_10 = std::list<Statement*>{assign_2_, assign_3_};
    std::list<Statement*>* actual_children_10 = ptct->find(if_2_)->second;
    CHECK(TestUtils::AreListsEqual(*actual_children_10, expected_children_10));

    CHECK(ptct->size() == 5);
  }
}
