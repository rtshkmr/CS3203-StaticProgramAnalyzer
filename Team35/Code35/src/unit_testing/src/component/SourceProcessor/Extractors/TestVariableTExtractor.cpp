#include "catch.hpp"
#include "component/SourceProcessor/Extractors/VariableTExtractor.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.VariableTExtractor.Extract variables basic conditions") {
  // setup
  auto* deliverable = new Deliverable{};
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_7_ = GetAssign7();
  IfEntity* if_1_ = GetIf1();
  ElseEntity* else_1_ = GetElse();
  WhileEntity* while_1_ = GetWhileEntity1();

  SECTION("Procedure with no container") {
    // proc with no container
    /*
     * procedure proc1 {
     *  print x;
     * }
     * procedure proc2 {
     *  read x;
     * }
     */
    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(new PrintEntity(var_x_));

    deliverable->GetProcList()->push_back(proc1);
    std::list<Variable*> proc_var_list = {
        var_x_ // from print
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc1, &proc_var_list));

    auto* proc2 = new Procedure(new ProcedureName("proc1"));
    proc2->AddStatement(new ReadEntity(var_x_));

    deliverable->GetProcList()->push_back(proc2);
    std::list<Variable*> proc_var_list2 = {
        var_x_ // from read
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc2, &proc_var_list2));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    SECTION("Uses") {
      std::list<Variable*> actual_var_list = *deliverable->GetUseCMap()->find(proc1)->second;
      std::list<Variable*> expected_var_list = {var_x_};
      CHECK(actual_var_list == expected_var_list);
      CHECK(deliverable->GetModifiesCMap()->count(proc1) == 0);
    }

    SECTION("Modifies") {
      std::list<Variable*> actual_var_list = *deliverable->GetModifiesCMap()->find(proc2)->second;
      std::list<Variable*> expected_var_list = {var_x_};
      CHECK(actual_var_list == expected_var_list);
      CHECK(deliverable->GetUseCMap()->count(proc2) == 0);
    }
  }

  SECTION("Procedure calls procedure with no var") {
    SECTION("uses") {
      // proc with no container
      /*
       * procedure proc1 {
       *  call proc2;
       * }
       * procedure proc2 {
       *  read x;
       * }
       */
      auto* proc1 = new Procedure(new ProcedureName("proc1"));
      auto* proc2 = new Procedure(new ProcedureName("proc2"));
      auto* call2 = new CallEntity(proc2);
      proc2->AddStatement(new ReadEntity(var_x_));
      proc1->AddStatement(call2);

      deliverable->GetProcList()->push_back(proc1);
      deliverable->GetProcList()->push_back(proc2);

      auto variable_t_extractor = VariableTExtractor();
      variable_t_extractor.Extract(deliverable, VariableRel::kUses);

      CHECK(deliverable->GetUseCMap()->count(proc1) == 0);
      CHECK(deliverable->GetUseCMap()->count(proc2) == 0);
      CHECK(deliverable->GetUseSMap()->count(call2) == 0);
    }

    SECTION("modifies") {
      // proc with no container
      /*
       * procedure proc1 {
       *  call proc2;
       * }
       * procedure proc2 {
       *  print x;
       * }
       */
      auto* proc1 = new Procedure(new ProcedureName("proc1"));
      auto* proc2 = new Procedure(new ProcedureName("proc2"));
      auto* call2 = new CallEntity(proc2);
      proc2->AddStatement(new PrintEntity(var_x_));
      proc1->AddStatement(call2);

      deliverable->GetProcList()->push_back(proc1);
      deliverable->GetProcList()->push_back(proc2);

      auto variable_t_extractor = VariableTExtractor();
      variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

      CHECK(deliverable->GetModifiesCMap()->count(proc1) == 0);
      CHECK(deliverable->GetModifiesCMap()->count(proc2) == 0);
      CHECK(deliverable->GetModifiesSMap()->count(call2) == 0);
    }
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
     *    y = i;
     *  }
     * }
     */
    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(new ReadEntity(var_x_));
    proc1->AddStatement(new PrintEntity(var_y_));
    proc1->AddStatement(assign_1_);
    if_1_->AddStatement(assign_4_);
    else_1_->AddStatement(assign_6_);
    if_1_->SetElseEntity(else_1_);
    proc1->AddStatement(if_1_);

    deliverable->GetProcList()->push_back(proc1);
    std::list<Variable*> proc_use_list = {
        var_y_, // from print
        var_x_, // from if
        var_z_,
        var_i_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc1, &proc_use_list));
    std::list<Variable*> if_use_list = {
        var_x_, // from if condition
        var_z_,
        var_i_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(if_1_, &if_use_list));
    std::list<Variable*> proc_mod_list = {
        var_x_, // from read
        var_z_,
        var_y_,
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc1, &proc_mod_list));
    std::list<Variable*> if_mod_list = {
        var_y_,
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(if_1_, &if_mod_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(proc1)->second;
    std::list<Variable*> expected_use_list = {var_y_, var_x_, var_z_, var_i_};

    // use
    CHECK(actual_use_list == expected_use_list);
    //intermediate change
    std::list<Variable*> expected_if_use_list = {var_x_, var_z_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(if_1_)->second == expected_if_use_list);

    // mod
    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(proc1)->second;
    std::list<Variable*> expected_mod_list = {var_x_, var_z_, var_y_};
    CHECK(actual_mod_list == expected_mod_list);
    // no change to inner container
    std::list<Variable*> expected_if_mod_list = {var_y_};
    CHECK(*deliverable->GetModifiesCMap()->find(if_1_)->second == expected_if_mod_list);
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
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    proc2->AddStatement(assign_1_);
    proc2->AddStatement(assign_7_);
    while_1_->AddStatement(assign_4_);
    while_1_->AddStatement(assign_6_);
    proc2->AddStatement(while_1_);
    proc2->AddStatement(new ReadEntity(var_x_));
    proc2->AddStatement(new PrintEntity(var_x_));

    deliverable->GetProcList()->push_back(proc2);
    std::list<Variable*> proc_use_list = {
        var_y_, // from print
        var_z_,
        var_x_, // from while
        var_i_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc2, &proc_use_list));
    std::list<Variable*> while_use_list = {
        var_x_, // from condition
        var_z_,
        var_i_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(while_1_, &while_use_list));
    std::list<Variable*> proc_mod_list = {
        var_z_,
        var_i_,
        var_x_,
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc2, &proc_mod_list));
    std::list<Variable*> while_mod_list = {
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(while_1_, &while_mod_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(proc2)->second;
    std::list<Variable*> expected_use_list = {var_y_, var_z_, var_x_, var_i_};

    // use
    CHECK(*deliverable->GetUseCMap()->find(while_1_)->second == while_use_list); // no change to inner container
    CHECK(actual_use_list == expected_use_list);

    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(proc2)->second;
    std::list<Variable*> expected_mod_list = {var_z_, var_i_, var_x_, var_y_};

    // mod
    CHECK(*deliverable->GetModifiesCMap()->find(while_1_)->second
              == while_mod_list); // no change to inner container
    CHECK(actual_mod_list == expected_mod_list);
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
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    proc3->AddStatement(assign_4_);
    auto* call4 = new CallEntity(proc4);
    proc3->AddStatement(call4);
    proc4->AddStatement(assign_5_);

    deliverable->GetProcList()->push_back(proc3);
    deliverable->GetProcList()->push_back(proc4);
    std::list<Variable*> proc3_use_list = {
        var_z_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc3, &proc3_use_list));
    std::list<Variable*> proc4_use_list = {
        var_y_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc4, &proc4_use_list));
    std::list<Variable*> proc3_mod_list = {
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc3, &proc3_mod_list));
    std::list<Variable*> proc4_mod_list = {
        var_z_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc4, &proc4_mod_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // use
    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(proc3)->second;
    std::list<Variable*> expected_use_list = {var_z_, var_y_};
    CHECK(actual_use_list == expected_use_list);
    // call stmt
    std::list<Variable*> actual_use_list2 = *deliverable->GetUseSMap()->find(call4)->second;
    CHECK(actual_use_list2 == proc4_use_list);
    CHECK(*deliverable->GetUseCMap()->find(proc4)->second == proc4_use_list); // no change to inner container

    // mod
    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(proc3)->second;
    std::list<Variable*> expected_mod_list = {var_y_, var_z_};
    CHECK(actual_mod_list == expected_mod_list);
    CHECK(*deliverable->GetModifiesCMap()->find(proc4)->second == proc4_mod_list); // no change to inner container
    // call stmt
    std::list<Variable*> actual_var_list2 = *deliverable->GetModifiesSMap()->find(call4)->second;
    CHECK(actual_var_list2 == proc4_mod_list);
  }
}

TEST_CASE("1.VariableTExtractor.Extract variables nested containers") {
  // setup
  auto* deliverable = new Deliverable{};
  AssignEntity* assign_1_ = GetAssign1();
  AssignEntity* assign_3_ = GetAssign3();
  AssignEntity* assign_4_ = GetAssign4();
  AssignEntity* assign_5_ = GetAssign5();
  AssignEntity* assign_6_ = GetAssign6();
  AssignEntity* assign_7_ = GetAssign7();
  AssignEntity* assign_8_ = GetAssign8();
  AssignEntity* assign_11_ = GetAssign11();
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

    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    proc1->AddStatement(if_1_);

    deliverable->GetProcList()->push_back(proc1);
    std::list<Variable*> if1_mod_list = {
        var_y_,
        var_i_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(if_1_, &if1_mod_list));
    std::list<Variable*> if2_mod_list = {
        var_i_,
        var_z_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(if_2_, &if2_mod_list));
    std::list<Variable*> if3_mod_list = {
        var_x_,
        var_z_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(if_3_, &if3_mod_list));
    std::list<Variable*> if1_use_list = {
        var_x_, // from if condition
        var_z_,
    };
    deliverable->GetUseCMap()->insert(std::make_pair(if_1_, &if1_use_list));
    std::list<Variable*> if2_use_list = {
        var_y_, // from if condition
        var_z_,
    };
    deliverable->GetUseCMap()->insert(std::make_pair(if_2_, &if2_use_list));
    std::list<Variable*> if3_use_list = {
        var_z_, // from if condition
    };
    deliverable->GetUseCMap()->insert(std::make_pair(if_3_, &if3_use_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // mod
    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(if_1_)->second;
    std::list<Variable*> expected_mod_list = {var_y_, var_i_, var_z_, var_x_};
    CHECK(actual_mod_list == expected_mod_list);
    // else should not be in modifies hash
    CHECK(deliverable->GetModifiesCMap()->count(else_2_) == 0);
    // no change to inner container
    std::list<Variable*> expected_if3_mod_list = {var_x_, var_z_};
    CHECK(*deliverable->GetModifiesCMap()->find(if_3_)->second == expected_if3_mod_list);
    // intermediate change to secondary container
    CHECK(*deliverable->GetModifiesCMap()->find(if_2_)->second == std::list<Variable*>{var_i_, var_z_, var_x_});

    // use
    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(if_1_)->second;
    std::list<Variable*> expected_use_list = {var_x_, var_z_, var_y_};
    CHECK(actual_use_list == expected_use_list);
    // else should not be in modifies hash
    CHECK(deliverable->GetUseCMap()->count(else_2_) == 0);
    // no change to innermost container
    std::list<Variable*> expected_if3_use_list = {var_z_};
    CHECK(*deliverable->GetUseCMap()->find(if_3_)->second == expected_if3_use_list);
    // intermediate change to secondary container
    CHECK(*deliverable->GetUseCMap()->find(if_2_)->second == std::list<Variable*>{var_y_, var_z_});

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
     *        m = n;
     *      }
     *    }
     *  }
     * }
     */
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    while_1_->AddStatement(assign_4_);
    while_2_->AddStatement(assign_5_);
    while_3_->AddStatement(assign_11_);
    while_1_->AddStatement(while_2_);
    while_2_->AddStatement(while_3_);
    proc2->AddStatement(while_1_);

    deliverable->GetProcList()->push_back(proc2);
    std::list<Variable*> while_mod_list = {
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(while_1_, &while_mod_list));
    std::list<Variable*> while2_mod_list = {
        var_z_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(while_2_, &while2_mod_list));
    std::list<Variable*> while3_mod_list = {
        var_m_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(while_3_, &while3_mod_list));
    std::list<Variable*> while_use_list = {
        var_x_, // from condition
        var_z_,
    };
    deliverable->GetUseCMap()->insert(std::make_pair(while_1_, &while_use_list));
    std::list<Variable*> while2_use_list = {
        var_y_,
    };
    deliverable->GetUseCMap()->insert(std::make_pair(while_2_, &while2_use_list));
    std::list<Variable*> while3_use_list = {
        var_z_,
        var_n_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(while_3_, &while3_use_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    //mod
    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(while_1_)->second;
    std::list<Variable*> expected_mod_list = {var_y_, var_z_, var_m_};
    CHECK(actual_mod_list == expected_mod_list);
    CHECK(*deliverable->GetModifiesCMap()->find(while_3_)->second
              == while3_mod_list); // no change to inner container
    // intermediate change to secondary container
    CHECK(*deliverable->GetModifiesCMap()->find(while_2_)->second == std::list<Variable*>{var_z_, var_m_});

    // use
    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(while_1_)->second;
    std::list<Variable*> expected_use_list = {var_x_, var_z_, var_y_, var_n_};
    CHECK(actual_use_list == expected_use_list);
    CHECK(*deliverable->GetUseCMap()->find(while_3_)->second == while3_use_list); // no change to inner container
    std::list<Variable*> secondary_use_list = {var_y_, var_z_, var_n_};
    // intermediate change to secondary container
    CHECK(*deliverable->GetUseCMap()->find(while_2_)->second == secondary_use_list);
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
     * m=n
     * }
     */
    auto* proc3 = new Procedure(new ProcedureName("proc3"));
    auto* proc4 = new Procedure(new ProcedureName("proc4"));
    auto* proc2 = new Procedure(new ProcedureName("proc2"));
    auto* call4 = new CallEntity(proc4);
    auto* call2 = new CallEntity(proc2);
    proc3->AddStatement(assign_4_);
    proc3->AddStatement(call4);
    proc4->AddStatement(assign_3_);
    proc4->AddStatement(call2);
    proc2->AddStatement(assign_11_);

    deliverable->GetProcList()->push_back(proc3);
    deliverable->GetProcList()->push_back(proc4);
    deliverable->GetProcList()->push_back(proc2);
    std::list<Variable*> proc3_mod_list = {
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc3, &proc3_mod_list));
    std::list<Variable*> proc4_mod_list = {
        var_z_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc4, &proc4_mod_list));
    std::list<Variable*> proc2_mod_list = {
        var_m_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(proc2, &proc2_mod_list));
    std::list<Variable*> proc3_use_list = {
        var_z_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc3, &proc3_use_list));
    std::list<Variable*> proc2_use_list = {
        var_n_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(proc2, &proc2_use_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // mod
    std::list<Variable*> actual_mod_list = *deliverable->GetModifiesCMap()->find(proc3)->second;
    std::list<Variable*> expected_mod_list = {var_y_, var_z_, var_m_};
    CHECK(actual_mod_list == expected_mod_list);
    CHECK(*deliverable->GetModifiesCMap()->find(proc2)->second == proc2_mod_list); // no change to inner container
    // intermediate change to secondary container
    std::list<Variable*> secondary_mod_list = {var_z_, var_m_};
    CHECK(*deliverable->GetModifiesCMap()->find(proc4)->second == secondary_mod_list);
    // call stmts
    CHECK(*deliverable->GetModifiesSMap()->find(call4)->second == secondary_mod_list);
    CHECK(*deliverable->GetModifiesSMap()->find(call2)->second == proc2_mod_list);

    // use
    std::list<Variable*> actual_use_list = *deliverable->GetUseCMap()->find(proc3)->second;
    std::list<Variable*> expected_use_list = {var_z_, var_n_};
    CHECK(actual_use_list == expected_use_list);
    CHECK(*deliverable->GetUseCMap()->find(proc2)->second == proc2_use_list); // no change to inner container
    std::list<Variable*> secondary_use_list = {var_n_};
    // intermediate change to secondary container
    CHECK(*deliverable->GetUseCMap()->find(proc4)->second == secondary_use_list);
    // call stmts
    CHECK(*deliverable->GetUseSMap()->find(call4)->second == secondary_use_list);
    CHECK(*deliverable->GetUseSMap()->find(call2)->second == secondary_use_list);

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
    auto* proc1 = new Procedure(new ProcedureName("proc1"));
    if_1_->AddStatement(assign_4_);
    else_1_->AddStatement(assign_6_);
    if_1_->SetElseEntity(else_1_);
    proc1->AddStatement(if_1_);
    while_1_->AddStatement(assign_7_);
    proc1->AddStatement(while_1_);

    deliverable->GetProcList()->push_back(proc1);
    std::list<Variable*> if_mod_list = {
        var_y_
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(if_1_, &if_mod_list));
    std::list<Variable*> while_mod_list = {
        var_i_,
    };
    deliverable->GetModifiesCMap()->insert(std::make_pair(while_1_, &while_mod_list));
    std::list<Variable*> if_use_list = {
        var_x_, // from if condition
        var_z_,
        var_i_
    };
    deliverable->GetUseCMap()->insert(std::make_pair(if_1_, &if_use_list));
    std::list<Variable*> while_use_list = {
        var_z_,
    };
    deliverable->GetUseCMap()->insert(std::make_pair(while_1_, &while_use_list));

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // mod
    CHECK(*deliverable->GetModifiesCMap()->find(while_1_)->second
              == while_mod_list); // no change to inner container
    // intermediate change to if container turns out to be the same
    std::list<Variable*> expected_if_mod_list = {var_y_};
    CHECK(*deliverable->GetModifiesCMap()->find(if_1_)->second == expected_if_mod_list);

    // use
    CHECK(*deliverable->GetUseCMap()->find(while_1_)->second == while_use_list); // no change to inner container
    // no change to inner container
    std::list<Variable*> expected_if_use_list = {var_x_, var_z_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(if_1_)->second == expected_if_use_list);
  }
}

TEST_CASE("1.VariableTExtractor.Extract variables nested calls") {
  // setup
  auto* deliverable = new Deliverable{};
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
     *  i=i
     *
     *  procedure 5
     *  readn
     */
  auto* proc1 = new Procedure(new ProcedureName("proc1"));
  auto* proc2 = new Procedure(new ProcedureName("proc2"));
  auto* proc3 = new Procedure(new ProcedureName("proc3"));
  auto* proc4 = new Procedure(new ProcedureName("proc4"));
  auto* proc5 = new Procedure(new ProcedureName("proc5"));
  auto* call2 = new CallEntity(proc2);
  auto* call3 = new CallEntity(proc3);
  auto* call4 = new CallEntity(proc4);
  auto* call5 = new CallEntity(proc5);

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

  std::list<Variable*> proc1_mod_list = {
      var_y_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(proc1, &proc1_mod_list));
  std::list<Variable*> proc2_mod_list = {
      var_m_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(proc2, &proc2_mod_list));
  std::list<Variable*> proc3_mod_list = {
      var_z_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(proc3, &proc3_mod_list));
  std::list<Variable*> proc4_mod_list = {
      var_i_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(proc4, &proc4_mod_list));
  std::list<Variable*> proc5_mod_list = {
      var_n_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(proc5, &proc5_mod_list));
  std::list<Variable*> while1_mod_list = {
      var_y_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(while_1, &while1_mod_list));
  std::list<Variable*> if2_mod_list = {
      var_m_
  };
  deliverable->GetModifiesCMap()->insert(std::make_pair(if_2, &if2_mod_list));
  std::list<Variable*> proc1_use_list = {
      var_x_,
      var_z_
  };
  deliverable->GetUseCMap()->insert(std::make_pair(proc1, &proc1_use_list));
  std::list<Variable*> proc2_use_list = {
      var_x_,
      var_n_
  };
  deliverable->GetUseCMap()->insert(std::make_pair(proc2, &proc2_use_list));
  std::list<Variable*> proc3_use_list = {
      var_y_
  };
  deliverable->GetUseCMap()->insert(std::make_pair(proc3, &proc3_use_list));
  std::list<Variable*> proc4_use_list = {
      var_i_
  };
  deliverable->GetUseCMap()->insert(std::make_pair(proc4, &proc4_use_list));
  std::list<Variable*> while1_use_list = {
      var_x_,
      var_z_
  };
  deliverable->GetUseCMap()->insert(std::make_pair(while_1, &while1_use_list));
  std::list<Variable*> while2_use_list = {
      var_x_,
  };
  deliverable->GetUseCMap()->insert(std::make_pair(while_2, &while2_use_list));
  std::list<Variable*> if1_use_list = {
      var_x_,
  };
  deliverable->GetUseCMap()->insert(std::make_pair(if_1, &if1_use_list));
  std::list<Variable*> if2_use_list = {
      var_x_,
      var_n_,
  };
  deliverable->GetUseCMap()->insert(std::make_pair(if_2, &if2_use_list));

  SECTION("Called procedures parsed later") {
    deliverable->GetProcList()->push_back(proc1);
    deliverable->GetProcList()->push_back(proc2);
    deliverable->GetProcList()->push_back(proc3);
    deliverable->GetProcList()->push_back(proc4);
    deliverable->GetProcList()->push_back(proc5);

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // mod
    CHECK(*deliverable->GetModifiesCMap()->find(proc3)->second == proc3_mod_list); // no change to inner container
    // intermediate changes to secondary containers
    std::list<Variable*> mod_list1 = {var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(proc2)->second == mod_list1);
    CHECK(*deliverable->GetModifiesCMap()->find(if_2)->second == mod_list1);
    std::list<Variable*> mod_list2 = {var_y_, var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(while_1)->second == mod_list2);
    std::list<Variable*> mod_list3 = {var_z_};
    CHECK(*deliverable->GetModifiesCMap()->find(while_2)->second == mod_list3);
    std::list<Variable*> mod_list4 = {var_z_, var_y_, var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(if_1)->second == mod_list4);
    std::list<Variable*> mod_list5 = {var_y_, var_i_, var_z_, var_m_, var_n_};
    CHECK(*deliverable->GetModifiesCMap()->find(proc1)->second == mod_list5);
    // call stmts
    CHECK(*deliverable->GetModifiesSMap()->find(call2)->second == mod_list1);
    CHECK(*deliverable->GetModifiesSMap()->find(call3)->second == proc3_mod_list);
    CHECK(*deliverable->GetModifiesSMap()->find(call4)->second == proc4_mod_list);
    CHECK(*deliverable->GetModifiesSMap()->find(call5)->second == proc5_mod_list);

    //use
    CHECK(*deliverable->GetUseCMap()->find(proc3)->second == proc3_use_list); // no change to inner container
    // intermediate changes to secondary containers
    std::list<Variable*> use_list1 = {var_x_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(proc2)->second == use_list1);
    CHECK(*deliverable->GetUseCMap()->find(if_2)->second == use_list1);
    std::list<Variable*> use_list2 = {var_x_, var_z_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(while_1)->second == use_list2);
    std::list<Variable*> use_list3 = {var_x_, var_y_};
    CHECK(*deliverable->GetUseCMap()->find(while_2)->second == use_list3);
    std::list<Variable*> use_list4 = {var_x_, var_y_, var_z_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(if_1)->second == use_list4);
    std::list<Variable*> use_list5 = {var_x_, var_z_, var_i_, var_y_, var_n_};
    CHECK(*deliverable->GetUseCMap()->find(proc1)->second == use_list5);
    // call stmts
    CHECK(*deliverable->GetUseSMap()->find(call2)->second == use_list1);
    CHECK(*deliverable->GetUseSMap()->find(call3)->second == proc3_use_list);
    CHECK(*deliverable->GetUseSMap()->find(call4)->second == proc4_use_list);
    CHECK(deliverable->GetUseSMap()->count(call5) == 0);

  }

  SECTION("Called procedures parsed first") {
    deliverable->GetProcList()->push_back(proc5);
    deliverable->GetProcList()->push_back(proc4);
    deliverable->GetProcList()->push_back(proc3);
    deliverable->GetProcList()->push_back(proc2);
    deliverable->GetProcList()->push_back(proc1);

    auto variable_t_extractor = VariableTExtractor();
    variable_t_extractor.Extract(deliverable, VariableRel::kUses);
    variable_t_extractor.Extract(deliverable, VariableRel::kModifies);

    // mod
    CHECK(*deliverable->GetModifiesCMap()->find(proc3)->second == proc3_mod_list); // no change to inner container
    // intermediate changes to secondary containers
    std::list<Variable*> mod_list1 = {var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(proc2)->second == mod_list1);
    CHECK(*deliverable->GetModifiesCMap()->find(if_2)->second == mod_list1);
    std::list<Variable*> mod_list2 = {var_y_, var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(while_1)->second == mod_list2);
    std::list<Variable*> mod_list3 = {var_z_};
    CHECK(*deliverable->GetModifiesCMap()->find(while_2)->second == mod_list3);
    std::list<Variable*> mod_list4 = {var_z_, var_y_, var_m_, var_i_};
    CHECK(*deliverable->GetModifiesCMap()->find(if_1)->second == mod_list4);
    std::list<Variable*> mod_list5 = {var_y_, var_i_, var_z_, var_m_, var_n_};
    CHECK(*deliverable->GetModifiesCMap()->find(proc1)->second == mod_list5);
    // call stmts
    CHECK(*deliverable->GetModifiesSMap()->find(call2)->second == mod_list1);
    CHECK(*deliverable->GetModifiesSMap()->find(call3)->second == proc3_mod_list);
    CHECK(*deliverable->GetModifiesSMap()->find(call4)->second == proc4_mod_list);
    CHECK(*deliverable->GetModifiesSMap()->find(call5)->second == proc5_mod_list);

    // use
    CHECK(*deliverable->GetUseCMap()->find(proc3)->second == proc3_use_list); // no change to inner container
    // intermediate changes to secondary containers
    std::list<Variable*> use_list1 = {var_x_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(proc2)->second == use_list1);
    CHECK(*deliverable->GetUseCMap()->find(if_2)->second == use_list1);
    std::list<Variable*> use_list2 = {var_x_, var_z_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(while_1)->second == use_list2);
    std::list<Variable*> use_list3 = {var_x_, var_y_};
    CHECK(*deliverable->GetUseCMap()->find(while_2)->second == use_list3);
    std::list<Variable*> use_list4 = {var_x_, var_y_, var_z_, var_n_, var_i_};
    CHECK(*deliverable->GetUseCMap()->find(if_1)->second == use_list4);
    std::list<Variable*> use_list5 = {var_x_, var_z_, var_i_, var_y_, var_n_};
    CHECK(*deliverable->GetUseCMap()->find(proc1)->second == use_list5);
    // call stmts
    CHECK(*deliverable->GetUseSMap()->find(call2)->second == use_list1);
    CHECK(*deliverable->GetUseSMap()->find(call3)->second == proc3_use_list);
    CHECK(*deliverable->GetUseSMap()->find(call4)->second == proc4_use_list);
    CHECK(deliverable->GetUseSMap()->count(call5) == 0);
  }
}
