#include "catch.hpp"
#include "component/SourceProcessor/Extractors/ModifiesExtractor.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.ModifiesExtractor.Extract Modifies basic conditions") {
  // setup
  Deliverable deliverable;
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_7_ = GetAssign7();
  IfEntity* if_1_ = GetIf1();
  ElseEntity* else_1_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity1();

  SECTION("Procedure with no container") {
    // proc with no container
    /*
     * procedure proc1 {
     *  read x;
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(new ReadEntity(var_x_));

    deliverable.proc_list_.push_back(proc1);
    std::list<Variable*> proc_var_list = {
        var_x_ // from read
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc1, & proc_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(proc1)->second;
    std::list<Variable*> expected_var_list = {var_x_};

    CHECK(actual_var_list == expected_var_list);
  }

  SECTION("Procedure with no var") {
    // proc with no container
    /*
     * procedure proc1 {
     *  print x;
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(new PrintEntity(var_x_));

    deliverable.proc_list_.push_back(proc1);

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    CHECK(deliverable.container_modifies_hash_.count(proc1) == 0);
  }

  SECTION("Procedure calls procedure with no var") {
    // proc with no container
    /*
     * procedure proc1 {
     *  call proc2;
     * }
     * procedure proc2 {
     *  print x;
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    CallEntity* call2 = new CallEntity(proc2);
    proc2->AddStatement(new PrintEntity(var_x_));
    proc1->AddStatement(call2);

    deliverable.proc_list_.push_back(proc1);
    deliverable.proc_list_.push_back(proc2);

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    CHECK(deliverable.container_modifies_hash_.count(proc1) == 0);
    CHECK(deliverable.container_modifies_hash_.count(proc2) == 0);
    CHECK(deliverable.modifies_hash_.count(call2) == 0);
  }

  SECTION("Procedure with 1 if container") {
    // proc with if container
    /*
     * procedure proc1 {
     *  read x;
     *  print y;
     *  z = 1;
     *  if (x==0) {
     *    y = z;
     *  } else {
     *    z = y;
     *  }
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(new ReadEntity(var_x_));
    proc1->AddStatement(new PrintEntity(var_y_));
    proc1->AddStatement(assign_1_);
    if_1_->AddStatement(assign_4_);
    else_1_->AddStatement(assign_5_);
    if_1_->SetElseEntity(else_1_);
    proc1->AddStatement(if_1_);

    deliverable.proc_list_.push_back(proc1);
    std::list<Variable*> proc_var_list = {
        var_x_, // from read
        var_z_,
        var_y_,
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc1, & proc_var_list));
    std::list<Variable*> if_var_list = {
        var_y_,
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(if_1_, & if_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(proc1)->second;
    std::list<Variable*> expected_var_list = {var_x_, var_z_, var_y_};

    CHECK(actual_var_list == expected_var_list);

    // no change to inner container
    std::list<Variable*> expected_if_var_list = {var_y_, var_z_};
    CHECK(* deliverable.container_modifies_hash_.find(if_1_)->second == expected_if_var_list);
  }

  SECTION("Procedure with 1 while container") {
    // proc with while container
    /*
     * procedure proc2 {
     *  z = 1;
     *  i = z;
     *  while (x==0) {
     *    y = z;
     *    y = i;
     *  }
     *  read x;
     *  print y;
     * }
     */
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    proc2->AddStatement(assign_1_);
    proc2->AddStatement(assign_7_);
    while_1_->AddStatement(assign_4_);
    while_1_->AddStatement(assign_6_);
    proc2->AddStatement(while_1_);
    proc2->AddStatement(new ReadEntity(var_x_));
    proc2->AddStatement(new PrintEntity(var_x_));

    deliverable.proc_list_.push_back(proc2);
    std::list<Variable*> proc_var_list = {
        var_z_,
        var_i_,
        var_x_,
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc2, & proc_var_list));
    std::list<Variable*> while_var_list = {
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(while_1_, & while_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(proc2)->second;
    std::list<Variable*> expected_var_list = {var_z_, var_i_, var_x_, var_y_};

    CHECK(
        * deliverable.container_modifies_hash_.find(while_1_)->second
            == while_var_list); // no change to inner container
    CHECK(actual_var_list == expected_var_list);
  }

  SECTION("Procedure with 1 call container") {
    // proc with call container
    /*
     * procedure proc3 {
     * y = z
     * call proc4
     * }
     * procedure proc4 {
     * z = y
     * }
     */
    Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
    Procedure* proc4 = new Procedure(new ProcedureName("proc4"));
    CallEntity* call4 = new CallEntity(proc4);
    proc3->AddStatement(assign_4_);
    proc3->AddStatement(call4);
    proc4->AddStatement(assign_5_);

    deliverable.proc_list_.push_back(proc3);
    deliverable.proc_list_.push_back(proc4);
    std::list<Variable*> proc3_var_list = {
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc3, &proc3_var_list));
    std::list<Variable*> proc4_var_list = {
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc4, &proc4_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(&deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(proc3)->second;
    std::list<Variable*> expected_var_list = {var_y_, var_z_};
    CHECK(actual_var_list == expected_var_list);

    CHECK(*deliverable.container_modifies_hash_.find(proc4)->second == proc4_var_list); // no change to inner container

    // call stmt
    std::list<Variable*> actual_var_list2 = *deliverable.modifies_hash_.find(call4)->second;
    CHECK(actual_var_list2 == proc4_var_list);
  }
}

TEST_CASE("1.ModifiesExtractor.Extract Modifies nested containers") {
  // setup
  Deliverable deliverable;
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_7_ = GetAssign7();
  AssignEntity* assign_8_ = GetAssign8();
  IfEntity* if_1_ = GetIf1();
  IfEntity* if_2_ = GetIf2();
  IfEntity* if_3_ = GetIf3();
  ElseEntity* else_1_ = GetElse();
  ElseEntity* else_2_ = GetElse();
  ElseEntity* else_3_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity1();
  WhileEntity* while_2_ = GetWhileEntity2();
  WhileEntity* while_3_ = GetWhileEntity3();

  // test for proc with no nesting of containers is redundant bcos deliverable is already populated by PSub.

  SECTION("Procedure with 3 if container") {
    // proc with if container in if block and else block
    /*
     * procedure proc1 {
     *  if (x==0) {
     *    y = z;
     *    if (y<=3) {
     *     i = z;
     *    } else {
     *      z = 1;
     *      if (z>1) {
     *       x = z;
     *      } else {
     *       z = 1;
     *      }
     *    }
     *  } else {
     *    i = z;
     *  }
     * }
     */
    if_1_->AddStatement(assign_4_);
    else_1_->AddStatement(assign_7_);
    if_1_->SetElseEntity(else_1_);

    if_2_->AddStatement(assign_7_);
    else_2_->AddStatement(assign_1_);
    if_2_->SetElseEntity(else_2_);
    if_1_->AddStatement(if_2_);

    if_3_->AddStatement(assign_8_);
    else_3_->AddStatement(assign_1_);
    if_3_->SetElseEntity(else_3_);
    else_2_->AddStatement(if_3_);

    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(if_1_);

    deliverable.proc_list_.push_back(proc1);
    std::list<Variable*> if1_var_list = {
        var_y_,
        var_i_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(if_1_, & if1_var_list));
    std::list<Variable*> if2_var_list = {
        var_i_,
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(if_2_, & if2_var_list));
    std::list<Variable*> if3_var_list = {
        var_x_,
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(if_3_, & if3_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(if_1_)->second;
    std::list<Variable*> expected_var_list = {var_y_, var_i_, var_z_, var_x_};

    CHECK(actual_var_list == expected_var_list);

    // else should not be in modifies hash
    CHECK(deliverable.container_modifies_hash_.count(else_2_) == 0);

    // no change to inner container
    std::list<Variable*> expected_if3_var_list = {var_x_, var_z_};
    CHECK(* deliverable.container_modifies_hash_.find(if_3_)->second == expected_if3_var_list);
    // intermediate change to secondary container
    CHECK(* deliverable.container_modifies_hash_.find(if_2_)->second == std::list<Variable*>{var_i_, var_z_, var_x_});
  }

  SECTION("Procedure with 3 nested while container") {
    // proc with while container
    /*
     * procedure proc2 {
     *  while (x==0) {
     *    y = z;
     *    while (y<=3) {
     *      z = y;
     *      while (z>1) {
     *        x = z;
     *      }
     *    }
     *  }
     * }
     */
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    while_1_->AddStatement(assign_4_);
    while_2_->AddStatement(assign_5_);
    while_3_->AddStatement(assign_8_);
    while_1_->AddStatement(while_2_);
    while_2_->AddStatement(while_3_);
    proc2->AddStatement(while_1_);

    deliverable.proc_list_.push_back(proc2);
    std::list<Variable*> while_var_list = {
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(while_1_, & while_var_list));
    std::list<Variable*> while2_var_list = {
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(while_2_, & while2_var_list));
    std::list<Variable*> while3_var_list = {
        var_x_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(while_3_, & while3_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(while_1_)->second;
    std::list<Variable*> expected_var_list = {var_y_, var_z_, var_x_};
    CHECK(actual_var_list == expected_var_list);

    CHECK(* deliverable.container_modifies_hash_.find(while_3_)->second == while3_var_list); // no change to inner container

    // intermediate change to secondary container
    CHECK(* deliverable.container_modifies_hash_.find(while_2_)->second == std::list<Variable*>{var_z_, var_x_});
  }

  SECTION("Procedure with 2 call container") {
    // proc with call container
    /*
     * procedure proc3 {
     * y = z
     * call proc4
     * }
     * procedure proc4 {
     * z=3
     * call proc2
     * }
     * procedure proc2 {
     * x=z
     * }
     */
    Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
    Procedure* proc4 = new Procedure(new ProcedureName("proc4"));
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    CallEntity* call4 = new CallEntity(proc4);
    CallEntity* call2 = new CallEntity(proc2);
    proc3->AddStatement(assign_4_);
    proc3->AddStatement(call4);
    proc4->AddStatement(assign_3_);
    proc4->AddStatement(call2);
    proc2->AddStatement(assign_8_);

    deliverable.proc_list_.push_back(proc3);
    deliverable.proc_list_.push_back(proc4);
    deliverable.proc_list_.push_back(proc2);
    std::list<Variable*> proc3_var_list = {
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc3, &proc3_var_list));
    std::list<Variable*> proc4_var_list = {
        var_z_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc4, &proc4_var_list));
    std::list<Variable*> proc2_var_list = {
        var_x_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(proc2, &proc2_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(&deliverable);

    std::list<Variable*> actual_var_list = *deliverable.container_modifies_hash_.find(proc3)->second;
    std::list<Variable*> expected_var_list = {var_y_, var_z_, var_x_};
    CHECK(actual_var_list == expected_var_list);

    CHECK(*deliverable.container_modifies_hash_.find(proc2)->second == proc2_var_list); // no change to inner container

    // intermediate change to secondary container
    std::list<Variable*> secondary_var_list = {var_z_, var_x_};
    CHECK(*deliverable.container_modifies_hash_.find(proc4)->second == secondary_var_list);

    // call stmts
    CHECK(*deliverable.modifies_hash_.find(call4)->second == secondary_var_list);
    CHECK(*deliverable.modifies_hash_.find(call2)->second == proc2_var_list);
  }

  SECTION("Procedure with multiple container") {
    // proc with if container
    /*
     * procedure proc1 {
     *  if (x==0) {
     *    y = z;
     *  } else {
     *    y = i;
     *  }
     *  while (x==0) {
     *    i = z;
     *  }
     * }
     */
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    if_1_->AddStatement(assign_4_);
    else_1_->AddStatement(assign_6_);
    if_1_->SetElseEntity(else_1_);
    proc1->AddStatement(if_1_);
    while_1_->AddStatement(assign_7_);
    proc1->AddStatement(while_1_);

    deliverable.proc_list_.push_back(proc1);
    std::list<Variable*> if_var_list = {
        var_y_
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(if_1_, & if_var_list));
    std::list<Variable*> while_var_list = {
        var_i_,
    };
    deliverable.container_modifies_hash_.insert(std::make_pair(while_1_, & while_var_list));

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(& deliverable);

    CHECK(
        * deliverable.container_modifies_hash_.find(while_1_)->second== while_var_list); // no change to inner container

    // intermediate change to if container turns out to be the same
    std::list<Variable*> expected_if_var_list = {var_y_};
    CHECK(* deliverable.container_modifies_hash_.find(if_1_)->second == expected_if_var_list);
  }
}

TEST_CASE("1.ModifiesExtractor.Extract modifies nested calls") {
  // setup
  Deliverable deliverable;
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_7_ = GetAssign7();
  ReadEntity* read_n = GetReadN();
  ReadEntity* read_m = GetReadM();
  PrintEntity* print_n = GetPrintN();
  ElseEntity* else_1 = GetElse();
  ElseEntity* else_2 = GetElse();
  IfEntity* if_1 = GetIf1();
  IfEntity* if_2 = GetIf1();
  WhileEntity* while_1 = GetWhileEntity1();
  WhileEntity* while_2 = GetWhileEntity1();

  /*
     * procedure 1
     *  call 4
     *  if (x==0)
     *    while (x==0)
     *      call 2
     *      y=z
     *  else
     *    while (x==0)
     *      call 3
     *  call 5
     *
     *  procedure 2
     *  if (x==0)
     *    readm
     *  else
     *    call 4
     *    printn
     *
     *  procedure 3
     *  z=y
     *
     *  procedure 4
     *  i=z
     *
     *  procedure 5
     *  readn
     */
  Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
  Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
  Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
  Procedure* proc4 = new Procedure(new ProcedureName("proc4"));
  Procedure* proc5 = new Procedure(new ProcedureName("proc5"));
  CallEntity* call2 = new CallEntity(proc2);
  CallEntity* call3 = new CallEntity(proc3);
  CallEntity* call4 = new CallEntity(proc4);
  CallEntity* call5 = new CallEntity(proc5);

  while_1->AddStatement(call2);
  while_1->AddStatement(assign_4_);
  if_1->AddStatement(while_1);
  while_2->AddStatement(call3);
  else_1->AddStatement(while_2);
  if_1->SetElseEntity(else_1);

  proc1->AddStatement(call4);
  proc1->AddStatement(if_1);
  proc1->AddStatement(call5);

  if_2->AddStatement(read_m);
  else_2->AddStatement(call4);
  else_2->AddStatement(print_n);
  if_2->SetElseEntity(else_2);
  proc2->AddStatement(if_2);

  proc3->AddStatement(assign_5_);
  proc4->AddStatement(assign_7_);
  proc5->AddStatement(read_n);

  std::list<Variable*> proc1_var_list = {
      var_y_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(proc1, &proc1_var_list));
  std::list<Variable*> proc2_var_list = {
      var_m_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(proc2, &proc2_var_list));
  std::list<Variable*> proc3_var_list = {
      var_z_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(proc3, &proc3_var_list));
  std::list<Variable*> proc4_var_list = {
      var_i_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(proc4, &proc4_var_list));
  std::list<Variable*> proc5_var_list = {
      var_n_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(proc5, &proc5_var_list));
  std::list<Variable*> while1_var_list = {
      var_y_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(while_1, &while1_var_list));
  std::list<Variable*> if2_var_list = {
      var_m_
  };
  deliverable.container_modifies_hash_.insert(std::make_pair(if_2, &if2_var_list));

  SECTION("Called procedures parsed later") {
    deliverable.proc_list_.push_back(proc1);
    deliverable.proc_list_.push_back(proc2);
    deliverable.proc_list_.push_back(proc3);
    deliverable.proc_list_.push_back(proc4);
    deliverable.proc_list_.push_back(proc5);

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(&deliverable);

    CHECK(*deliverable.container_modifies_hash_.find(proc3)->second == proc3_var_list); // no change to inner container

    // intermediate changes to secondary containers
    std::list<Variable*> var_list1 = {var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(proc2)->second == var_list1);
    CHECK(*deliverable.container_modifies_hash_.find(if_2)->second == var_list1);

    std::list<Variable*> var_list2 = {var_y_, var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(while_1)->second == var_list2);

    std::list<Variable*> var_list3 = {var_z_};
    CHECK(*deliverable.container_modifies_hash_.find(while_2)->second == var_list3);

    std::list<Variable*> var_list4 = {var_z_, var_y_, var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(if_1)->second == var_list4);

    std::list<Variable*> var_list5 = {var_y_, var_i_, var_z_, var_m_, var_n_};
    CHECK(*deliverable.container_modifies_hash_.find(proc1)->second == var_list5);

    // call stmts
    CHECK(*deliverable.modifies_hash_.find(call2)->second == var_list1);
    CHECK(*deliverable.modifies_hash_.find(call3)->second == proc3_var_list);
    CHECK(*deliverable.modifies_hash_.find(call4)->second == proc4_var_list);
    CHECK(*deliverable.modifies_hash_.find(call5)->second == proc5_var_list);
  }

  SECTION("Called procedures parsed first") {
    deliverable.proc_list_.push_back(proc5);
    deliverable.proc_list_.push_back(proc4);
    deliverable.proc_list_.push_back(proc3);
    deliverable.proc_list_.push_back(proc2);
    deliverable.proc_list_.push_back(proc1);

    ModifiesExtractor modifies_extractor{};
    modifies_extractor.Extract(&deliverable);

    CHECK(*deliverable.container_modifies_hash_.find(proc3)->second == proc3_var_list); // no change to inner container

    // intermediate changes to secondary containers
    std::list<Variable*> var_list1 = {var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(proc2)->second == var_list1);
    CHECK(*deliverable.container_modifies_hash_.find(if_2)->second == var_list1);

    std::list<Variable*> var_list2 = {var_y_, var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(while_1)->second == var_list2);

    std::list<Variable*> var_list3 = {var_z_};
    CHECK(*deliverable.container_modifies_hash_.find(while_2)->second == var_list3);

    std::list<Variable*> var_list4 = {var_z_, var_y_, var_m_, var_i_};
    CHECK(*deliverable.container_modifies_hash_.find(if_1)->second == var_list4);

    std::list<Variable*> var_list5 = {var_y_, var_i_, var_z_, var_m_, var_n_};
    CHECK(*deliverable.container_modifies_hash_.find(proc1)->second == var_list5);

    // call stmts
    CHECK(*deliverable.modifies_hash_.find(call2)->second == var_list1);
    CHECK(*deliverable.modifies_hash_.find(call3)->second == proc3_var_list);
    CHECK(*deliverable.modifies_hash_.find(call4)->second == proc4_var_list);
    CHECK(*deliverable.modifies_hash_.find(call5)->second == proc5_var_list);
  }
}
