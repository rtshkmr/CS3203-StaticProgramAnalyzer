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
  Container* cont_if_1 = dynamic_cast<Container*>(if_1);
  Container* cont_if_2 = dynamic_cast<Container*>(if_2);
  Container* cont_while_1 = dynamic_cast<Container*>(while_1);
  Container* cont_proc_1 = dynamic_cast<Container*>(proc1);

  SECTION ("AddFollowsRelationship") {
    deliverable.AddFollowRelationship(if_1, a1);
    deliverable.AddFollowRelationship(if_1, a1);  // duplicate check
    REQUIRE(deliverable.follow_hash_.count(if_1));
    REQUIRE(deliverable.follow_hash_.find(if_1)->second == a1);

    // adding more var to existing entry
    deliverable.AddFollowRelationship(if_1, a2);
    REQUIRE(deliverable.follow_hash_.find(if_1)->second == a1);
    REQUIRE_FALSE(deliverable.followed_by_hash_.count(a2));

    deliverable.AddFollowRelationship(while_1, rx);
    deliverable.AddFollowRelationship(while_1, rx);  // duplicate check
    REQUIRE(deliverable.follow_hash_.count(while_1));
    REQUIRE(deliverable.follow_hash_.find(while_1)->second == rx);

    // adding more var to existing entry
    deliverable.AddFollowRelationship(while_1, px);
    REQUIRE(deliverable.follow_hash_.find(while_1)->second == rx);
    REQUIRE_FALSE(deliverable.followed_by_hash_.count(px));

    // reverse check
    REQUIRE(deliverable.followed_by_hash_.count(rx));
    REQUIRE(deliverable.followed_by_hash_.find(rx)->second == while_1);
    REQUIRE(deliverable.followed_by_hash_.count(a1));
    REQUIRE(deliverable.followed_by_hash_.find(a1)->second == if_1);
  }

  SECTION ("AddFollowsTransitiveRelationship and ForList") {
    deliverable.AddFollowsTransitiveRelationship(rx, if_1);
    deliverable.AddFollowsTransitiveRelationship(rx, if_1);  // duplicate check
    std::list<Statement*> stmt_list1 = {if_1};
    REQUIRE(deliverable.follows_T_hash_.count(rx));
    REQUIRE(* deliverable.follows_T_hash_.find(rx)->second == stmt_list1);

    // adding more var to existing entry
    std::list<Statement*> stmt_list2 = {if_1, a1};
    deliverable.AddFollowsTransitiveRelationship(rx, a1);
    REQUIRE(* deliverable.follows_T_hash_.find(rx)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list = {a1, if_2, while_1};
    deliverable.AddFollowsTransitiveRelationshipForList(rx, & add_stmt_list);
    std::list<Statement*> expected_stmt_list = {if_1, a1, if_2, while_1};
    REQUIRE(* deliverable.follows_T_hash_.find(rx)->second == expected_stmt_list);

    deliverable.AddFollowsTransitiveRelationship(px, if_1);
    deliverable.AddFollowsTransitiveRelationship(px, if_1);  // duplicate check
    REQUIRE(deliverable.follows_T_hash_.count(px));
    REQUIRE(* deliverable.follows_T_hash_.find(px)->second == stmt_list1);

    // adding more var to existing entry
    deliverable.AddFollowsTransitiveRelationship(px, a1);
    REQUIRE(* deliverable.follows_T_hash_.find(px)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list2 = {if_1, if_2, while_1};
    deliverable.AddFollowsTransitiveRelationshipForList(rx, & add_stmt_list2);
    std::list<Statement*> expected_stmt_list2 = {if_1, a1, if_2, while_1};
    REQUIRE(* deliverable.follows_T_hash_.find(rx)->second == expected_stmt_list2);

    // reverse check
    deliverable.AddFollowsTransitiveRelationship(if_2, a1);
    std::list<Statement*> expected_stmt_list3 = {rx, px, if_2};
    REQUIRE(deliverable.followed_by_T_hash_.count(a1));
    REQUIRE(* deliverable.followed_by_T_hash_.find(a1)->second == expected_stmt_list3);
    std::list<Statement*> expected_stmt_list4 = {rx, px};
    REQUIRE(deliverable.followed_by_T_hash_.count(if_1));
    REQUIRE(* deliverable.followed_by_T_hash_.find(if_1)->second == expected_stmt_list4);
  }

  SECTION ("AddParentRelationship") {
    deliverable.AddParentRelationship(if_1, a1);
    deliverable.AddParentRelationship(if_1, a1);  // duplicate check
    std::list<Statement*> add_stmt_list = {a1};
    std::list<Statement*> add_2_stmt_list = {a1, a2};
    REQUIRE(deliverable.parent_to_child_hash_.count(if_1));
    REQUIRE(* deliverable.parent_to_child_hash_.find(if_1)->second == add_stmt_list);

    // adding more stmt to existing entry
    deliverable.AddParentRelationship(if_1, a2);
    REQUIRE(* deliverable.parent_to_child_hash_.find(if_1)->second == add_2_stmt_list);

    std::list<Statement*> add_stmt_list2 = {rx};
    std::list<Statement*> add_2_stmt_list2 = {rx, px};
    deliverable.AddParentRelationship(while_1, rx);
    deliverable.AddParentRelationship(while_1, rx);  // duplicate check
    REQUIRE(deliverable.parent_to_child_hash_.count(while_1));
    REQUIRE(* deliverable.parent_to_child_hash_.find(while_1)->second == add_stmt_list2);

    // adding more stmt to existing entry
    deliverable.AddParentRelationship(while_1, px);
    REQUIRE(* deliverable.parent_to_child_hash_.find(while_1)->second == add_2_stmt_list2);

    // reverse check
    REQUIRE(deliverable.child_to_parent_hash_.count(rx));
    REQUIRE(deliverable.child_to_parent_hash_.find(rx)->second == while_1);
    REQUIRE(deliverable.child_to_parent_hash_.count(a2));
    REQUIRE(deliverable.child_to_parent_hash_.find(a2)->second == if_1);
  }

  SECTION ("AddParentTransitiveRelationship and ForList") {
    deliverable.AddParentTransitiveRelationship(rx, if_1);
    deliverable.AddParentTransitiveRelationship(rx, if_1);  // duplicate check
    std::list<Statement*> stmt_list1 = {if_1};
    REQUIRE(deliverable.parent_to_child_T_hash_.count(rx));
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(rx)->second == stmt_list1);

    // adding more var to existing entry
    std::list<Statement*> stmt_list2 = {if_1, a1};
    deliverable.AddParentTransitiveRelationship(rx, a1);
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(rx)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list = {a1, if_2, while_1};
    deliverable.AddParentTransitiveRelationshipForList(rx, & add_stmt_list);
    std::list<Statement*> expected_stmt_list = {if_1, a1, if_2, while_1};
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(rx)->second == expected_stmt_list);

    deliverable.AddParentTransitiveRelationship(px, if_1);
    deliverable.AddParentTransitiveRelationship(px, if_1);  // duplicate check
    REQUIRE(deliverable.parent_to_child_T_hash_.count(px));
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(px)->second == stmt_list1);

    // adding more var to existing entry
    deliverable.AddParentTransitiveRelationship(px, a1);
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(px)->second == stmt_list2);

    // adding list
    std::list<Statement*> add_stmt_list2 = {if_1, if_2, while_1};
    deliverable.AddParentTransitiveRelationshipForList(rx, & add_stmt_list2);
    std::list<Statement*> expected_stmt_list2 = {if_1, a1, if_2, while_1};
    REQUIRE(* deliverable.parent_to_child_T_hash_.find(rx)->second == expected_stmt_list2);

    // reverse check
    deliverable.AddParentTransitiveRelationship(if_2, a1);
    std::list<Statement*> expected_stmt_list3 = {rx, px, if_2};
    REQUIRE(deliverable.child_to_parent_T_hash_.count(a1));
    REQUIRE(* deliverable.child_to_parent_T_hash_.find(a1)->second == expected_stmt_list3);
    std::list<Statement*> expected_stmt_list4 = {rx, px};
    REQUIRE(deliverable.child_to_parent_T_hash_.count(if_1));
    REQUIRE(* deliverable.child_to_parent_T_hash_.find(if_1)->second == expected_stmt_list4);
  }

  SECTION ("AddUsesRelationship for statements") {
    deliverable.AddUsesRelationship(rx, var_x_);
    deliverable.AddUsesRelationship(rx, var_x_);  // duplicate check
    std::list<Variable*> add_var_list = {var_x_};
    std::list<Variable*> add_2_var_list = {var_x_, var_y_};
    REQUIRE(deliverable.use_hash_.count(rx));
    REQUIRE(* deliverable.use_hash_.find(rx)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddUsesRelationship(rx, var_y_);
    REQUIRE(* deliverable.use_hash_.find(rx)->second == add_2_var_list);

    deliverable.AddUsesRelationship(px, var_x_);
    deliverable.AddUsesRelationship(px, var_x_);  // duplicate check
    REQUIRE(deliverable.use_hash_.count(px));
    REQUIRE(* deliverable.use_hash_.find(px)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddUsesRelationship(px, var_y_);
    REQUIRE(* deliverable.use_hash_.find(px)->second == add_2_var_list);

    deliverable.AddUsesRelationship(a1, var_x_);
    deliverable.AddUsesRelationship(a1, var_x_);  // duplicate check
    REQUIRE(deliverable.use_hash_.count(a1));
    REQUIRE(* deliverable.use_hash_.find(a1)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddUsesRelationship(a1, var_y_);
    REQUIRE(* deliverable.use_hash_.find(a1)->second == add_2_var_list);

    // reverse check
    deliverable.AddUsesRelationship(px, var_x_);
    std::list<Statement*> stmt_list = {rx, px, a1};
    REQUIRE(deliverable.used_by_hash_.count(var_x_));
    REQUIRE(* deliverable.used_by_hash_.find(var_x_)->second == stmt_list);
    REQUIRE(deliverable.used_by_hash_.count(var_y_));
    REQUIRE(* deliverable.used_by_hash_.find(var_y_)->second == stmt_list);
  }

  SECTION("AddUsesRelationship for containers") {
    REQUIRE_FALSE(deliverable.container_use_hash_.count(cont_if_1));

    // AddUsesRelationship(Container, Variable)
    deliverable.AddUsesRelationship(cont_if_1, var_x_);
    // cont_if_1 exists in hash
    REQUIRE(deliverable.container_use_hash_.count(cont_if_1));
    std::list<Variable*>* actual_var1_list = deliverable.container_use_hash_.find(cont_if_1)->second;
    // {cont_if_1, var_x_} exists in hash
    std::list<Variable*> var_x_list = {var_x_};
    REQUIRE(* actual_var1_list == var_x_list);

    // AddUsesRelationship(Container, list of Variables)
    std::list<Variable*> add_var_list = {var_z_, var_y_};
    deliverable.AddUsesRelationship(cont_if_1, & add_var_list);
    std::list<Variable*> expected_var1_list = {var_x_, var_z_, var_y_};
    REQUIRE(* actual_var1_list == expected_var1_list);

    // Check duplicate does not Get added in
    deliverable.AddUsesRelationship(cont_if_1, var_z_);  // (Container, Variable)
    std::list<Variable*> add_dup_var_list = {var_x_, var_y_, var_z_};
    deliverable.AddUsesRelationship(cont_if_1, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(* actual_var1_list == expected_var1_list);

    // Check different container same var
    // same type of container
    deliverable.AddUsesRelationship(cont_if_2, var_x_);  // (Container, Variable)
    deliverable.AddUsesRelationship(cont_if_2, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(* actual_var1_list == expected_var1_list);
    REQUIRE(deliverable.container_use_hash_.count(cont_if_2));
    REQUIRE(* deliverable.container_use_hash_.find(cont_if_2)->second == add_dup_var_list);
    // diff type of container
    deliverable.AddUsesRelationship(cont_while_1, var_x_);  // (Container, Variable)
    deliverable.AddUsesRelationship(cont_proc_1, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(deliverable.container_use_hash_.count(cont_while_1));
    REQUIRE(* deliverable.container_use_hash_.find(cont_while_1)->second == var_x_list);
    REQUIRE(deliverable.container_use_hash_.count(cont_proc_1));
    REQUIRE(* deliverable.container_use_hash_.find(cont_proc_1)->second == add_dup_var_list);

    // Check reverse relationship in container_used_by_hash_
    std::list<Container*> expected_varx_cont_list = {cont_if_1, cont_if_2, cont_while_1, cont_proc_1};
    REQUIRE(deliverable.container_used_by_hash_.count(var_x_));
    REQUIRE(* deliverable.container_used_by_hash_.find(var_x_)->second == expected_varx_cont_list);
    std::list<Container*> expected_vary_cont_list = {cont_if_1, cont_if_2, cont_proc_1};
    REQUIRE(deliverable.container_used_by_hash_.count(var_y_));
    REQUIRE(* deliverable.container_used_by_hash_.find(var_y_)->second == expected_vary_cont_list);
    std::list<Container*> expected_varz_cont_list = {cont_if_1, cont_if_2, cont_proc_1};
    REQUIRE(deliverable.container_used_by_hash_.count(var_z_));
    REQUIRE(* deliverable.container_used_by_hash_.find(var_z_)->second == expected_varz_cont_list);
  }

  SECTION ("AddModifiesRelationship for statements") {
    deliverable.AddModifiesRelationship(rx, var_x_);
    deliverable.AddModifiesRelationship(rx, var_x_);  // duplicate check
    std::list<Variable*> add_var_list = {var_x_};
    std::list<Variable*> add_2_var_list = {var_x_, var_y_};
    REQUIRE(deliverable.modifies_hash_.count(rx));
    REQUIRE(* deliverable.modifies_hash_.find(rx)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddModifiesRelationship(rx, var_y_);
    REQUIRE(* deliverable.modifies_hash_.find(rx)->second == add_2_var_list);

    deliverable.AddModifiesRelationship(px, var_x_);
    deliverable.AddModifiesRelationship(px, var_x_);  // duplicate check
    REQUIRE(deliverable.modifies_hash_.count(px));
    REQUIRE(* deliverable.modifies_hash_.find(px)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddModifiesRelationship(px, var_y_);
    REQUIRE(* deliverable.modifies_hash_.find(px)->second == add_2_var_list);

    deliverable.AddModifiesRelationship(a1, var_x_);
    deliverable.AddModifiesRelationship(a1, var_x_);  // duplicate check
    REQUIRE(deliverable.modifies_hash_.count(a1));
    REQUIRE(* deliverable.modifies_hash_.find(a1)->second == add_var_list);

    // adding more var to existing entry
    deliverable.AddModifiesRelationship(a1, var_y_);
    REQUIRE(* deliverable.modifies_hash_.find(a1)->second == add_2_var_list);

    // reverse check
    deliverable.AddModifiesRelationship(px, var_x_);
    std::list<Statement*> stmt_list = {rx, px, a1};
    REQUIRE(deliverable.modified_by_hash_.count(var_x_));
    REQUIRE(* deliverable.modified_by_hash_.find(var_x_)->second == stmt_list);
    REQUIRE(deliverable.modified_by_hash_.count(var_y_));
    REQUIRE(* deliverable.modified_by_hash_.find(var_y_)->second == stmt_list);
  }

  SECTION("AddModifiesRelationship for containers") {
    REQUIRE_FALSE(deliverable.container_modifies_hash_.count(cont_if_1));

    // AddModifiesRelationship(Container, Variable)
    deliverable.AddModifiesRelationship(cont_if_1, var_x_);
    // cont_if_1 exists in hash
    REQUIRE(deliverable.container_modifies_hash_.count(cont_if_1));
    std::list<Variable*>* actual_var1_list = deliverable.container_modifies_hash_.find(cont_if_1)->second;
    // {cont_if_1, var_x_} exists in hash
    std::list<Variable*> var_x_list = {var_x_};
    REQUIRE(* actual_var1_list == var_x_list);

    // AddModifiesRelationship(Container, list of Variables)
    std::list<Variable*> add_var_list = {var_z_, var_y_};
    deliverable.AddModifiesRelationship(cont_if_1, & add_var_list);
    std::list<Variable*> expected_var1_list = {var_x_, var_z_, var_y_};
    REQUIRE(actual_var1_list->size() == 3);
    REQUIRE(* actual_var1_list == expected_var1_list);

    // Check duplicate does not Get added in
    deliverable.AddModifiesRelationship(cont_if_1, var_z_);  // (Container, Variable)
    std::list<Variable*> add_dup_var_list = {var_x_, var_y_, var_z_};
    deliverable.AddModifiesRelationship(cont_if_1, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(actual_var1_list->size() == 3);
    REQUIRE(* actual_var1_list == expected_var1_list);

    // Check different container same var
    // same type of container
    deliverable.AddModifiesRelationship(cont_if_2, var_x_);  // (Container, Variable)
    deliverable.AddModifiesRelationship(cont_if_2, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(* actual_var1_list == expected_var1_list);
    REQUIRE(deliverable.container_modifies_hash_.count(cont_if_2));
    REQUIRE(* deliverable.container_modifies_hash_.find(cont_if_2)->second == add_dup_var_list);
    // diff type of container
    deliverable.AddModifiesRelationship(cont_while_1, var_x_);  // (Container, Variable)
    deliverable.AddModifiesRelationship(cont_proc_1, & add_dup_var_list); // (Container, list of Variables)
    REQUIRE(deliverable.container_modifies_hash_.count(cont_while_1));
    REQUIRE(* deliverable.container_modifies_hash_.find(cont_while_1)->second == var_x_list);
    REQUIRE(deliverable.container_modifies_hash_.count(cont_proc_1));
    REQUIRE(* deliverable.container_modifies_hash_.find(cont_proc_1)->second == add_dup_var_list);

    // Check reverse relationship in container_modified_by_hash_
    std::list<Container*> expected_varx_cont_list = {cont_if_1, cont_if_2, cont_while_1, cont_proc_1};
    REQUIRE(deliverable.container_modified_by_hash_.count(var_x_));
    REQUIRE(* deliverable.container_modified_by_hash_.find(var_x_)->second == expected_varx_cont_list);
    std::list<Container*> expected_vary_cont_list = {cont_if_1, cont_if_2, cont_proc_1};
    REQUIRE(deliverable.container_modified_by_hash_.count(var_y_));
    REQUIRE(* deliverable.container_modified_by_hash_.find(var_y_)->second == expected_vary_cont_list);
    std::list<Container*> expected_varz_cont_list = {cont_if_1, cont_if_2, cont_proc_1};
    REQUIRE(deliverable.container_modified_by_hash_.count(var_z_));
    REQUIRE(* deliverable.container_modified_by_hash_.find(var_z_)->second == expected_varz_cont_list);
  }

  SECTION ("AddCallsRelationship") {
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc1, proc2);  // duplicate check
    std::list<Procedure*> add_proc_list = {proc2};
    std::list<Procedure*> add_2_proc_list = {proc2, proc3};
    REQUIRE(deliverable.calls_hash_.count(proc1));
    REQUIRE(* deliverable.calls_hash_.find(proc1)->second == add_proc_list);

    // adding more proc to existing entry
    deliverable.AddCallsRelationship(proc1, proc3);
    REQUIRE(* deliverable.calls_hash_.find(proc1)->second == add_2_proc_list);

    std::list<Procedure*> add_proc_list2 = {proc3};
    std::list<Procedure*> add_2_proc_list2 = {proc3, proc4};
    deliverable.AddCallsRelationship(proc2, proc3);
    deliverable.AddCallsRelationship(proc2, proc3);  // duplicate check
    REQUIRE(deliverable.calls_hash_.count(proc2));
    REQUIRE(* deliverable.calls_hash_.find(proc2)->second == add_proc_list2);

    // adding more proc to existing entry
    deliverable.AddCallsRelationship(proc2, proc4);
    REQUIRE(* deliverable.calls_hash_.find(proc2)->second == add_2_proc_list2);

    // reverse check
    std::list<Procedure*> called_proc_list1 = {proc2};
    std::list<Procedure*> called_proc_list2 = {proc1, proc2};
    REQUIRE(deliverable.called_by_hash_.count(proc4));
    REQUIRE(*deliverable.called_by_hash_.find(proc4)->second == called_proc_list1);
    REQUIRE(deliverable.called_by_hash_.count(proc3));
    REQUIRE(*deliverable.called_by_hash_.find(proc3)->second == called_proc_list2);
  }

  SECTION ("AddCallsTransitiveRelationship and ForList") {
    deliverable.AddCallsTransitiveRelationship(proc1, proc2);
    deliverable.AddCallsTransitiveRelationship(proc1, proc2);  // duplicate check
    std::list<Procedure*> proc_list1 = {proc2};
    REQUIRE(deliverable.calls_T_hash_.count(proc1));
    REQUIRE(* deliverable.calls_T_hash_.find(proc1)->second == proc_list1);

    // adding more var to existing entry
    std::list<Procedure*> proc_list2 = {proc2, proc3};
    deliverable.AddCallsTransitiveRelationship(proc1, proc3);
    REQUIRE(* deliverable.calls_T_hash_.find(proc1)->second == proc_list2);

    // adding list
    std::list<Procedure*> add_proc_list = {proc2, proc3, proc4};
    deliverable.AddCallsTransitiveRelationshipForList(proc1, & add_proc_list);
    std::list<Procedure*> expected_proc_list = {proc2, proc3, proc4};
    REQUIRE(* deliverable.calls_T_hash_.find(proc1)->second == expected_proc_list);

    deliverable.AddCallsTransitiveRelationship(proc4, proc2);
    deliverable.AddCallsTransitiveRelationship(proc4, proc2);  // duplicate check
    REQUIRE(deliverable.calls_T_hash_.count(proc4));
    REQUIRE(* deliverable.calls_T_hash_.find(proc4)->second == proc_list1);

    // adding more var to existing entry
    deliverable.AddCallsTransitiveRelationship(proc4, proc3);
    REQUIRE(* deliverable.calls_T_hash_.find(proc4)->second == proc_list2);

    // adding list
    std::list<Procedure*> add_proc_list2 = {proc1, proc2, proc3};
    deliverable.AddCallsTransitiveRelationshipForList(proc4, & add_proc_list2);
    std::list<Procedure*> expected_proc_list2 = {proc2, proc3, proc1};
    REQUIRE(* deliverable.calls_T_hash_.find(proc4)->second == expected_proc_list2);

    // reverse check
    std::list<Procedure*> expected_proc_list3 = {proc1, proc4};
    REQUIRE(deliverable.called_by_T_hash_.count(proc2));
    REQUIRE(* deliverable.called_by_T_hash_.find(proc2)->second == expected_proc_list3);
    std::list<Procedure*> expected_proc_list4 = {proc1};
    REQUIRE(deliverable.called_by_T_hash_.count(proc4));
    REQUIRE(* deliverable.called_by_T_hash_.find(proc4)->second == expected_proc_list4);
  }
}