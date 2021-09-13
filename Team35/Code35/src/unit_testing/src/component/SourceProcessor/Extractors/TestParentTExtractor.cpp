#include "catch.hpp"
#include "component/SourceProcessor/Extractors/ParentTExtractor.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.ParentTExtractor.Extract ParentT basic conditions") {
  Deliverable deliverable;
  SECTION("Single level statement list") {
    /*
     * procedure proc1 {
     *  assign
     *  assign
     *  read
     *  print
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(assign_1_);
    proc1->AddStatement(assign_3_);
    proc1->AddStatement(read_x_);
    proc1->AddStatement(print_y_);
    proc1->AddStatement(assign_4_);
    proc1->AddStatement(assign_5_);

    deliverable.proc_list_.push_back(proc1);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // no parent
    std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable.parent_to_child_T_hash_;
    CHECK(ptct.empty());
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
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    if_1_->AddStatement(assign_1_);
    if_1_->AddStatement(read_y_);
    else_1_->AddStatement(assign_5_);
    else_1_->AddStatement(assign_6_);
    if_1_->SetElseEntity(else_1_);
    proc2->AddStatement(if_1_);
    proc2->AddStatement(print_y_);

    deliverable.proc_list_.push_back(proc2);
    deliverable.AddParentRelationship(if_1_, assign_1_);
    deliverable.AddParentRelationship(if_1_, read_y_);
    deliverable.AddParentRelationship(if_1_, assign_5_);
    deliverable.AddParentRelationship(if_1_, assign_6_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(1, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{assign_1_, read_y_, assign_5_, assign_6_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(if_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    CHECK(deliverable.parent_to_child_T_hash_.size() == 1);
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
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    proc2->AddStatement(assign_1_);
    proc2->AddStatement(print_i_);
    while_3_->AddStatement(read_z_);
    while_3_->AddStatement(assign_5_);
    proc2->AddStatement(while_3_);

    deliverable.proc_list_.push_back(proc2);
    deliverable.AddParentRelationship(while_3_, read_z_);
    deliverable.AddParentRelationship(while_3_, assign_5_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(2, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{read_z_, assign_5_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(while_3_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    CHECK(deliverable.parent_to_child_T_hash_.size() == 1);
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
    Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
    Procedure* proc4 = new Procedure(new ProcedureName("proc4"));
    proc3->AddStatement(assign_1_);
    CallEntity* call4 = new CallEntity(proc4);
    proc3->AddStatement(call4);
    if_2_->AddStatement(assign_2_);
    else_2_->AddStatement(read_y_);
    if_2_->SetElseEntity(else_2_);
    proc3->AddStatement(if_2_);
    while_1_->AddStatement(print_y_);
    proc4->AddStatement(while_1_);
    proc4->AddStatement(assign_4_);

    deliverable.proc_list_.push_back(proc3);
    deliverable.proc_list_.push_back(proc4);
    deliverable.AddParentRelationship(if_2_, assign_2_);
    deliverable.AddParentRelationship(if_2_, read_y_);
    deliverable.AddParentRelationship(while_1_, print_y_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(3, _)
    std::list<Statement*> expected_children_1 = std::list<Statement*>{assign_2_, read_y_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(if_2_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(7, _)
    std::list<Statement*> expected_children_2 = std::list<Statement*>{print_y_};
    std::list<Statement*>* actual_children_2 = deliverable.parent_to_child_T_hash_.find(while_1_)->second;
    CHECK(*actual_children_2 == expected_children_2);
    CHECK(deliverable.parent_to_child_T_hash_.size() == 2);
  }
}

TEST_CASE("1.ParentTExtractor.Extract ParentT nested containers") {
  Deliverable deliverable;

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
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    if_1_->AddStatement(assign_1_);

    else_1_->AddStatement(assign_3_);
    if_2_->AddStatement(assign_4_);
    if_3_->AddStatement(assign_5_);
    else_3_->AddStatement(read_z_);
    if_3_->SetElseEntity(else_3_);
    else_2_->AddStatement(if_3_);
    if_2_->SetElseEntity(else_2_);
    else_1_->AddStatement(if_2_);
    else_1_->AddStatement(print_i_);
    if_1_->SetElseEntity(else_1_);

    proc2->AddStatement(if_1_);
    proc2->AddStatement(print_n_);

    deliverable.proc_list_.push_back(proc2);
    deliverable.AddParentRelationship(if_1_, assign_1_);
    deliverable.AddParentRelationship(if_1_, assign_3_);
    deliverable.AddParentRelationship(if_1_, if_2_);
    deliverable.AddParentRelationship(if_1_, print_i_);
    deliverable.AddParentRelationship(if_2_, assign_4_);
    deliverable.AddParentRelationship(if_2_, if_3_);
    deliverable.AddParentRelationship(if_3_, assign_5_);
    deliverable.AddParentRelationship(if_3_, read_z_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, assign_3_, if_2_, assign_4_, if_3_, assign_5_, read_z_, print_i_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(if_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(3, _)
    std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable.parent_to_child_T_hash_;
    CHECK(ptct.find(assign_3_) == ptct.end());
    // Parent*(4, _)
    std::list<Statement*> expected_children_4 = std::list<Statement*>{assign_4_, if_3_, assign_5_, read_z_};
    std::list<Statement*>* actual_children_4 = deliverable.parent_to_child_T_hash_.find(if_2_)->second;
    CHECK(*actual_children_4 == expected_children_4);
    // Parent*(6, _)
    std::list<Statement*> expected_children_6 = std::list<Statement*>{assign_5_, read_z_};
    std::list<Statement*>* actual_children_6 = deliverable.parent_to_child_T_hash_.find(if_3_)->second;
    CHECK(*actual_children_6 == expected_children_6);

    CHECK(deliverable.parent_to_child_T_hash_.size() == 3);
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
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    if_1_->AddStatement(assign_1_);

    if_3_->AddStatement(assign_5_);
    else_3_->AddStatement(read_z_);
    if_3_->SetElseEntity(else_3_);

    if_2_->AddStatement(if_3_);
    else_2_->AddStatement(assign_4_);
    if_2_->SetElseEntity(else_2_);

    if_1_->AddStatement(if_2_);
    if_1_->AddStatement(print_i_);
    else_1_->AddStatement(assign_3_);
    if_1_->SetElseEntity(else_1_);

    proc2->AddStatement(if_1_);
    proc2->AddStatement(print_n_);

    deliverable.proc_list_.push_back(proc2);
    deliverable.AddParentRelationship(if_1_, assign_1_);
    deliverable.AddParentRelationship(if_1_, if_2_);
    deliverable.AddParentRelationship(if_1_, print_i_);
    deliverable.AddParentRelationship(if_1_, assign_3_);
    deliverable.AddParentRelationship(if_2_, if_3_);
    deliverable.AddParentRelationship(if_2_, assign_4_);
    deliverable.AddParentRelationship(if_3_, assign_5_);
    deliverable.AddParentRelationship(if_3_, read_z_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, if_2_, if_3_, assign_5_, read_z_, assign_4_, print_i_, assign_3_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(if_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{if_3_, assign_5_, read_z_, assign_4_};
    std::list<Statement*>* actual_children_3 = deliverable.parent_to_child_T_hash_.find(if_2_)->second;
    CHECK(*actual_children_3 == expected_children_3);
    // Parent*(4, _)
    std::list<Statement*> expected_children_4 = std::list<Statement*>{assign_5_, read_z_};
    std::list<Statement*>* actual_children_4 = deliverable.parent_to_child_T_hash_.find(if_3_)->second;
    CHECK(*actual_children_4 == expected_children_4);
    // Parent*(9, _)
    std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable.parent_to_child_T_hash_;
    CHECK(ptct.find(assign_3_) == ptct.end());

    CHECK(deliverable.parent_to_child_T_hash_.size() == 3);
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
    Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
    while_1_->AddStatement(assign_1_);
    while_1_->AddStatement(while_2_);
    while_2_->AddStatement(assign_3_);
    while_2_->AddStatement(while_3_);
    while_3_->AddStatement(assign_5_);
    while_2_->AddStatement(read_x_);
    while_1_->AddStatement(read_y_);
    proc3->AddStatement(while_1_);
    proc3->AddStatement(read_z_);

    deliverable.proc_list_.push_back(proc3);
    deliverable.AddParentRelationship(while_1_, assign_1_);
    deliverable.AddParentRelationship(while_1_, while_2_);
    deliverable.AddParentRelationship(while_1_, read_y_);
    deliverable.AddParentRelationship(while_2_, assign_3_);
    deliverable.AddParentRelationship(while_2_, while_3_);
    deliverable.AddParentRelationship(while_2_, read_x_);
    deliverable.AddParentRelationship(while_3_, assign_5_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, while_2_, assign_3_, while_3_, assign_5_, read_x_, read_y_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(while_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{assign_3_, while_3_, assign_5_, read_x_};
    std::list<Statement*>* actual_children_3 = deliverable.parent_to_child_T_hash_.find(while_2_)->second;
    CHECK(*actual_children_3 == expected_children_3);
    // Parent*(5, _)
    std::list<Statement*> expected_children_5 = std::list<Statement*>{assign_5_};
    std::list<Statement*>* actual_children_5 = deliverable.parent_to_child_T_hash_.find(while_3_)->second;
    CHECK(*actual_children_5 == expected_children_5);
    // Parent*(8, _)
    std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable.parent_to_child_T_hash_;
    CHECK(ptct.find(read_y_) == ptct.end());

    CHECK(deliverable.parent_to_child_T_hash_.size() == 3);
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
    Procedure* proc4 = new Procedure(new ProcedureName("proc4"));
    if_1_->AddStatement(assign_1_);
    while_1_->AddStatement(read_x_);
    if_1_->AddStatement(while_1_);

    while_2_->AddStatement(read_y_);
    else_1_->AddStatement(while_2_);
    else_1_->AddStatement(assign_6_);
    if_1_->SetElseEntity(else_1_);

    while_3_->AddStatement(print_x_);
    if_2_->AddStatement(assign_2_);
    else_2_->AddStatement(assign_3_);
    if_2_->SetElseEntity(else_2_);
    while_3_->AddStatement(if_2_);

    proc4->AddStatement(if_1_);
    proc4->AddStatement(while_3_);

    deliverable.proc_list_.push_back(proc4);
    deliverable.AddParentRelationship(if_1_, assign_1_);
    deliverable.AddParentRelationship(if_1_, while_1_);
    deliverable.AddParentRelationship(if_1_, while_2_);
    deliverable.AddParentRelationship(if_1_, assign_6_);
    deliverable.AddParentRelationship(while_1_, read_x_);
    deliverable.AddParentRelationship(while_2_, read_y_);

    deliverable.AddParentRelationship(if_2_, assign_2_);
    deliverable.AddParentRelationship(if_2_, assign_3_);

    deliverable.AddParentRelationship(while_3_, print_x_);
    deliverable.AddParentRelationship(while_3_, if_2_);

    ParentTExtractor parent_t_extractor{};
    parent_t_extractor.Extract(&deliverable);

    // Parent*(1, _)
    std::list<Statement*> expected_children_1 =
        std::list<Statement*>{assign_1_, while_1_, read_x_, while_2_, read_y_, assign_6_};
    std::list<Statement*>* actual_children_1 = deliverable.parent_to_child_T_hash_.find(if_1_)->second;
    CHECK(*actual_children_1 == expected_children_1);
    // Parent*(3, _)
    std::list<Statement*> expected_children_3 = std::list<Statement*>{read_x_};
    std::list<Statement*>* actual_children_3 = deliverable.parent_to_child_T_hash_.find(while_1_)->second;
    CHECK(*actual_children_3 == expected_children_3);
    // Parent*(5, _)
    std::list<Statement*> expected_children_5 = std::list<Statement*>{read_y_};
    std::list<Statement*>* actual_children_5 = deliverable.parent_to_child_T_hash_.find(while_2_)->second;
    CHECK(*actual_children_5 == expected_children_5);
    // Parent*(8, _)
    std::list<Statement*> expected_children_8 = std::list<Statement*>{print_x_, if_2_, assign_2_, assign_3_};
    std::list<Statement*>* actual_children_8 = deliverable.parent_to_child_T_hash_.find(while_3_)->second;
    CHECK(*actual_children_8 == expected_children_8);
    // Parent*(10, _)
    std::list<Statement*> expected_children_10 = std::list<Statement*>{assign_2_, assign_3_};
    std::list<Statement*>* actual_children_10 = deliverable.parent_to_child_T_hash_.find(if_2_)->second;
    CHECK(*actual_children_10 == expected_children_10);

    CHECK(deliverable.parent_to_child_T_hash_.size() == 5);
  }
}
