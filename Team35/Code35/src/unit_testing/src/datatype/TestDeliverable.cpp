#include "catch.hpp"
#include "datatype/Deliverable.h"
#include "../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.Deliverable.Add relationships") {
  Deliverable deliverable;
  ReadEntity* rx = GetReadX();
  PrintEntity* px = GetPrintX();
  AssignEntity* a1 = GetAssign1();
  AssignEntity* a2 = GetAssign2();
  IfEntity* if_1 = GetIf1();
  IfEntity* if_2 = GetIf2();
  WhileEntity* while_1 = GetWhileEntity1();
  Procedure* proc1 = GetProc1();
  Procedure* proc2 = GetProc2();
  Procedure* proc3 = GetProc3();
  Procedure* proc4 = GetProc4();
  auto* cont_if_1 = dynamic_cast<Container*>(if_1);
  auto* cont_if_2 = dynamic_cast<Container*>(if_2);
  auto* cont_while_1 = dynamic_cast<Container*>(while_1);
  auto* cont_proc_1 = dynamic_cast<Container*>(proc1);

  SECTION ("AddFollowsRelationship") {
    auto follow_hash = deliverable.GetFollowsMap();
    auto followed_by_hash = deliverable.GetFollowedByMap();
    deliverable.AddFollowRelationship(if_1, a1);
    deliverable.AddFollowRelationship(if_1, a1);  // duplicate check
    REQUIRE(follow_hash->count(if_1));
    REQUIRE(follow_hash->find(if_1)->second->size() == 1);
    REQUIRE(follow_hash->find(if_1)->second->front() == a1);

    // adding more var to existing entry
    deliverable.AddFollowRelationship(if_1, a2);
    REQUIRE(follow_hash->find(if_1)->second->size() == 1);
    REQUIRE(follow_hash->find(if_1)->second->front() == a1);
    REQUIRE_FALSE(followed_by_hash->count(a2));

    deliverable.AddFollowRelationship(while_1, rx);
    deliverable.AddFollowRelationship(while_1, rx);  // duplicate check
    REQUIRE(follow_hash->count(while_1));
    REQUIRE(follow_hash->find(while_1)->second->size() == 1);
    REQUIRE(follow_hash->find(while_1)->second->front() == rx);

    // adding more var to existing entry
    deliverable.AddFollowRelationship(while_1, px);
    REQUIRE(follow_hash->find(while_1)->second->size() == 1);
    REQUIRE(follow_hash->find(while_1)->second->front() == rx);
    REQUIRE_FALSE(followed_by_hash->count(px));

    // reverse check
    REQUIRE(followed_by_hash->count(rx));
    REQUIRE(followed_by_hash->find(rx)->second->size() == 1);
    REQUIRE(followed_by_hash->find(rx)->second->front() == while_1);
    REQUIRE(followed_by_hash->count(a1));
    REQUIRE(followed_by_hash->find(a1)->second->size() == 1);
    REQUIRE(followed_by_hash->find(a1)->second->front() == if_1);
  }

  SECTION ("AddFollowsTransitiveRelationship and ForList") {
    auto follows_T_hash = deliverable.GetFollowsTMap();
    auto followed_by_T_hash = deliverable.GetFollowedByTMap();
    deliverable.AddFollowsTransitiveRelationship(rx, if_1);
    deliverable.AddFollowsTransitiveRelationship(rx, if_1);  // duplicate check
    std::list<Statement*> stmt_list1 = {if_1};
    REQUIRE(follows_T_hash->count(rx));
    REQUIRE(* follows_T_hash->find(rx)->second == stmt_list1);

    // adding more var to existing entry
    std::list<Statement*> stmt_list2 = {if_1, a1};
    deliverable.AddFollowsTransitiveRelationship(rx, a1);
    REQUIRE(* follows_T_hash->find(rx)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list = {a1, if_2, while_1};
    deliverable.AddFollowsTransitiveRelationshipForList(rx, & add_stmt_list);
    std::list<Statement*> expected_stmt_list = {if_1, a1, if_2, while_1};
    REQUIRE(* follows_T_hash->find(rx)->second == expected_stmt_list);

    deliverable.AddFollowsTransitiveRelationship(px, if_1);
    deliverable.AddFollowsTransitiveRelationship(px, if_1);  // duplicate check
    REQUIRE(follows_T_hash->count(px));
    REQUIRE(* follows_T_hash->find(px)->second == stmt_list1);

    // adding more var to existing entry
    deliverable.AddFollowsTransitiveRelationship(px, a1);
    REQUIRE(* follows_T_hash->find(px)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list2 = {if_1, if_2, while_1};
    deliverable.AddFollowsTransitiveRelationshipForList(rx, & add_stmt_list2);
    std::list<Statement*> expected_stmt_list2 = {if_1, a1, if_2, while_1};
    REQUIRE(* follows_T_hash->find(rx)->second == expected_stmt_list2);

    // reverse check
    deliverable.AddFollowsTransitiveRelationship(if_2, a1);
    std::list<Statement*> expected_stmt_list3 = {rx, px, if_2};
    REQUIRE(followed_by_T_hash->count(a1));
    REQUIRE(* followed_by_T_hash->find(a1)->second == expected_stmt_list3);
    std::list<Statement*> expected_stmt_list4 = {rx, px};
    REQUIRE(followed_by_T_hash->count(if_1));
    REQUIRE(* followed_by_T_hash->find(if_1)->second == expected_stmt_list4);
  }

  SECTION ("AddParentRelationship") {
    auto parent_hash = deliverable.GetParentMap();
    auto child_hash = deliverable.GetChildMap();
    deliverable.AddParentRelationship(if_1, a1);
    deliverable.AddParentRelationship(if_1, a1);  // duplicate check
    std::list<Statement*> add_stmt_list = {a1};
    REQUIRE(parent_hash->count(if_1));
    REQUIRE(* parent_hash->find(if_1)->second == add_stmt_list);
    std::list<Statement*> add_stmt_list2 = {if_1};
    REQUIRE(child_hash->count(a1));
    REQUIRE(* child_hash->find(a1)->second == add_stmt_list2);
  }

  SECTION ("AddParentTransitiveRelationship and ForList") {
    auto parent_t_hash = deliverable.GetParentTMap();
    auto child_t_hash = deliverable.GetChildTMap();
    deliverable.AddParentTransitiveRelationship(rx, if_1);
    deliverable.AddParentTransitiveRelationship(rx, if_1);  // duplicate check
    std::list<Statement*> stmt_list1 = {if_1};
    REQUIRE(parent_t_hash->count(rx));
    REQUIRE(* parent_t_hash->find(rx)->second == stmt_list1);
    std::list<Statement*> stmt_list2 = {rx};
    REQUIRE(child_t_hash->count(if_1));
    REQUIRE(* child_t_hash->find(if_1)->second == stmt_list2);
  }

  SECTION ("AddUsesRelationship for statements") {
    auto uses_hash = deliverable.GetUseSMap();
    auto used_by_hash = deliverable.GetUsedBySMap();
    deliverable.AddUsesRelationship(rx, var_x_);
    deliverable.AddUsesRelationship(rx, var_x_);  // duplicate check
    std::list<Variable*> add_var_list = {var_x_};
    REQUIRE(uses_hash->count(rx));
    REQUIRE(* uses_hash->find(rx)->second == add_var_list);
    std::list<Statement*> add_var_list2 = {rx};
    REQUIRE(used_by_hash->count(var_x_));
    REQUIRE(* used_by_hash->find(var_x_)->second == add_var_list2);
  }

  SECTION("AddUsesRelationship for containers") {
    auto container_use_hash = deliverable.GetUseCMap();
    auto container_used_by_hash = deliverable.GetUsedByCMap();
    REQUIRE_FALSE(container_use_hash->count(cont_if_1));

    // AddUsesRelationship(Container, Variable)
    deliverable.AddUsesRelationship(cont_if_1, var_x_);
    std::list<Variable*> var_x_list = {var_x_};
    REQUIRE(container_use_hash->count(cont_if_1));
    REQUIRE(* container_use_hash->find(cont_if_1)->second == var_x_list);
    std::list<Container*> list2 = {cont_if_1};
    REQUIRE(container_used_by_hash->count(var_x_));
    REQUIRE(* container_used_by_hash->find(var_x_)->second == list2);
  }

  SECTION ("AddModifiesRelationship for statements") {
    auto modifies_hash = deliverable.GetModifiesSMap();
    auto modified_by_hash = deliverable.GetModifiedBySMap();
    deliverable.AddModifiesRelationship(rx, var_x_);
    deliverable.AddModifiesRelationship(rx, var_x_);  // duplicate check
    std::list<Variable*> add_var_list = {var_x_};
    REQUIRE(modifies_hash->count(rx));
    REQUIRE(* modifies_hash->find(rx)->second == add_var_list);
    std::list<Statement*> list2 = {rx};
    REQUIRE(modified_by_hash->count(var_x_));
    REQUIRE(* modified_by_hash->find(var_x_)->second == list2);
  }

  SECTION("AddModifiesRelationship for containers") {
    auto container_modifies_hash = deliverable.GetModifiesCMap();
    auto container_modified_by_hash = deliverable.GetModifiedByCMap();
    REQUIRE_FALSE(container_modifies_hash->count(cont_if_1));

    // AddModifiesRelationship(Container, Variable)
    deliverable.AddModifiesRelationship(cont_if_1, var_x_);
    std::list<Variable*> var_x_list = {var_x_};
    REQUIRE(container_modifies_hash->count(cont_if_1));
    REQUIRE(* container_modifies_hash->find(cont_if_1)->second == var_x_list);
    std::list<Container*> list2 = {cont_if_1};
    REQUIRE(container_modified_by_hash->count(var_x_));
    REQUIRE(* container_modified_by_hash->find(var_x_)->second == list2);
  }

  SECTION ("AddCallsRelationship") {
    auto calls_hash = deliverable.GetCallsMap();
    auto called_by_hash = deliverable.GetCalledByMap();
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc1, proc2);  // duplicate check
    std::list<Procedure*> add_proc_list = {proc2};
    REQUIRE(calls_hash->count(proc1));
    REQUIRE(* calls_hash->find(proc1)->second == add_proc_list);
    std::list<Procedure*> add_proc_list2 = {proc1};
    REQUIRE(called_by_hash->count(proc2));
    REQUIRE(* called_by_hash->find(proc2)->second == add_proc_list2);
  }

  SECTION ("AddCallsTransitiveRelationship and ForList") {
    auto calls_t_hash = deliverable.GetCallsTMap();
    auto called_by_t_hash = deliverable.GetCalledByTMap();
    deliverable.AddCallsTransitiveRelationship(proc1, proc2);
    deliverable.AddCallsTransitiveRelationship(proc1, proc2);  // duplicate check
    std::list<Procedure*> proc_list1 = {proc2};
    REQUIRE(calls_t_hash->count(proc1));
    REQUIRE(* calls_t_hash->find(proc1)->second == proc_list1);
    std::list<Procedure*> proc_list2 = {proc1};
    REQUIRE(called_by_t_hash->count(proc2));
    REQUIRE(* called_by_t_hash->find(proc2)->second == proc_list2);
  }

  SECTION ("AddNextRelationship") {
    auto next_hash = deliverable.GetNextMap();
    auto prev_hash = deliverable.GetPrevMap();
    deliverable.AddNextRelationship(if_1, rx);
    deliverable.AddNextRelationship(if_1, rx);  // duplicate check
    std::list<Statement*> stmt_list1 = {rx};
    REQUIRE(next_hash->count(if_1));
    REQUIRE(* next_hash->find(if_1)->second == stmt_list1);
    std::list<Statement*> stmt_list2 = {if_1};
    REQUIRE(prev_hash->count(rx));
    REQUIRE(* prev_hash->find(rx)->second == stmt_list2);
  }
}
