#include <datatype/DataType.h>
#include <model/Entity.h>
#include <model/Statement.h>
#include "catch.hpp"

/// 1. Test Variable
TEST_CASE("1.Model.Entity.Variable") {
  auto* variable_vtester = new VariableName("Vtester");
  auto* variable_xtester = new VariableName("xtesteR");

  auto* var_vtester = new Variable(variable_vtester);
  auto* var_xtester = new Variable(variable_xtester);

  SECTION("GetVariableName") {
    REQUIRE(var_vtester->GetVariableName() == variable_vtester);
    REQUIRE(var_xtester->GetVariableName() == variable_xtester);

    REQUIRE_FALSE(var_vtester->GetVariableName() == variable_xtester);
    REQUIRE_FALSE(var_xtester->GetVariableName() == variable_vtester);
  }

  SECTION("Casting to Entity and back") {
    auto* entity_var_vtester = dynamic_cast<Entity*>(var_vtester);
    auto* entity_var_xtester = dynamic_cast<Entity*>(var_xtester);
    REQUIRE(entity_var_vtester);
    REQUIRE(entity_var_xtester);

    auto* cast_var_vtester = dynamic_cast<Variable*>(entity_var_vtester);
    auto* cast_var_xtester = dynamic_cast<Variable*>(entity_var_xtester);
    REQUIRE(cast_var_vtester);
    REQUIRE(cast_var_xtester);

    REQUIRE(var_vtester == cast_var_vtester);
    REQUIRE(var_xtester == cast_var_xtester);
  }
}

/// 2. Test Procedure
TEST_CASE("1.Model.Entity.Procedure") {
  auto* pname_vtester = new ProcedureName("Vtester");
  auto* pname_xtester = new ProcedureName("xtesteR");

  auto* proc_vtester = new Procedure(pname_vtester);
  auto* proc_xtester = new Procedure(pname_xtester);

  SECTION("GetVariableName") {
    REQUIRE(proc_vtester->GetName() == pname_vtester);
    REQUIRE(proc_xtester->GetName() == pname_xtester);

    REQUIRE_FALSE(proc_vtester->GetName() == pname_xtester);
    REQUIRE_FALSE(proc_xtester->GetName() == pname_vtester);
  }

  SECTION("Casting to Entity and back") {
    auto* entity_proc_vtester = dynamic_cast<Entity*>(proc_vtester);
    auto* entity_proc_xtester = dynamic_cast<Entity*>(proc_xtester);
    REQUIRE(entity_proc_vtester);
    REQUIRE(entity_proc_xtester);

    auto* cast_proc_vtester = dynamic_cast<Procedure*>(entity_proc_vtester);
    auto* cast_proc_xtester = dynamic_cast<Procedure*>(entity_proc_xtester);
    REQUIRE(cast_proc_vtester);
    REQUIRE(cast_proc_xtester);

    REQUIRE(proc_vtester == cast_proc_vtester);
    REQUIRE(proc_xtester == cast_proc_xtester);
  }

  SECTION("Casting to Container and back") {
    auto* container_proc_vtester = dynamic_cast<Container*>(proc_vtester);
    auto* container_proc_xtester = dynamic_cast<Container*>(proc_xtester);
    REQUIRE(container_proc_vtester);
    REQUIRE(container_proc_xtester);

    auto* cast_proc_vtester = dynamic_cast<Procedure*>(container_proc_vtester);
    auto* cast_proc_xtester = dynamic_cast<Procedure*>(container_proc_xtester);
    REQUIRE(cast_proc_vtester);
    REQUIRE(cast_proc_xtester);

    REQUIRE(proc_vtester == cast_proc_vtester);
    REQUIRE(proc_xtester == cast_proc_xtester);
  }

  SECTION("Cast to Container and AddStmt") {
    auto* container_proc_vtester = dynamic_cast<Container*>(proc_vtester);

    // 3 sample statements
    auto* stmt1 = new ReadEntity(new Variable(new VariableName("aaa")));
    auto* stmt2 = new PrintEntity(new Variable(new VariableName("bbb")));
    auto* stmt3 = new CallEntity(proc_xtester);

    container_proc_vtester->AddStatement(stmt1);
    container_proc_vtester->AddStatement(stmt2);
    container_proc_vtester->AddStatement(stmt3);

    REQUIRE(container_proc_vtester->GetStatementList()->front() == stmt1);
    container_proc_vtester->GetStatementList()->pop_front();
    REQUIRE(container_proc_vtester->GetStatementList()->front() == stmt2);
    container_proc_vtester->GetStatementList()->pop_front();
    REQUIRE(container_proc_vtester->GetStatementList()->front() == stmt3);

  }
}

/// 3. Test Program
TEST_CASE("1.Model.Entity.Program") {
  auto* proc1 = new Procedure(new ProcedureName("HellO"));
  auto* proc2 = new Procedure(new ProcedureName("MellO"));
  auto* proc3 = new Procedure(new ProcedureName("KellO"));

  auto* program = new Program(proc1);
  program->AddProcedure(proc2);
  program->AddProcedure(proc3);

  REQUIRE(program->GetProcedureList()->front() == proc1);
  program->GetProcedureList()->pop_front();
  REQUIRE(program->GetProcedureList()->front() == proc2);
  program->GetProcedureList()->pop_front();
  REQUIRE(program->GetProcedureList()->front() == proc3);
}