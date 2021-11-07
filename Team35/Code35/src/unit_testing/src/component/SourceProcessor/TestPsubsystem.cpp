#include "catch.hpp"
#include "component/SourceProcessor/PSubsystem.h"

using psub::PSubsystem;

/// These test is made with assumption that Tokenizer and CreateEntity are correct. (Higher level testing)

TEST_CASE("1.PSubsystem.Basic Single Line Creation") {
  PSubsystem p_subsystem;
  p_subsystem.InitDataStructures();

  //TODO: expand on other single line
  //TODO: do test case with all kinds of statement
  SECTION("Procedure Test with 1 Assign") {
    p_subsystem.ProcessStatement("procedure firstProc {");
    p_subsystem.ProcessStatement("x = x + 100;");
    p_subsystem.ProcessStatement("}");
    Deliverable* deliverable = p_subsystem.GetDeliverables();

    ProcedureName pname("firstProc");
    REQUIRE(deliverable->GetProcList()->size() == 1);
    REQUIRE(* (* deliverable->GetProcList()->front()).GetName() == pname);

    VariableName vname("x");
    REQUIRE(deliverable->GetVariableList()->size() == 1);
    REQUIRE(* (* deliverable->GetVariableList()->front()).GetVariableName() == vname);

    REQUIRE(deliverable->GetConstantList()->size() == 1);
    ConstantValue* constant_list_front = const_cast<ConstantValue*>(deliverable->GetConstantList()->front()->GetValue());
    REQUIRE(constant_list_front->GetValue() == 100);

    //ensure StmtList and AssignList holds the same object
    REQUIRE(deliverable->GetStatementList()->size() == 1);
    AssignEntity* assign_entity = static_cast<AssignEntity*>(* (deliverable->GetStatementList()->begin()));
    AssignEntity* assign_entity2 = deliverable->GetAssignList()->front();
    REQUIRE(assign_entity == assign_entity2);
    REQUIRE(* assign_entity->GetVariableObj()->GetVariableName() == vname);
    REQUIRE(assign_entity->GetAssignmentExpr()->CheckExact("x + 100"));
    ConstantValue* expression_constant_front = const_cast<ConstantValue*>(
        assign_entity->GetExprConstants().front()->GetValue());
    REQUIRE(expression_constant_front->GetValue() == 100);

    //ensure trace down from Program is the same variable
    REQUIRE(deliverable->GetProgram()->GetProcedureList()->front() == deliverable->GetProcList()->front());
  }

}
