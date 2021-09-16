#include <datatype/DataType.h>
#include <model/Entity.h>
#include <model/Statement.h>
#include "catch.hpp"

/// 1. Test Variable
TEST_CASE("1.Model.Entity.Variable") {
  VariableName* variable_vtester = new VariableName("Vtester");
  VariableName* variable_xtester = new VariableName("xtesteR");

  Variable* var_vtester = new Variable(variable_vtester);
  Variable* var_xtester = new Variable(variable_xtester);

  SECTION("GetName") {
    REQUIRE(var_vtester->GetName() == variable_vtester);
    REQUIRE(var_xtester->GetName() == variable_xtester);

    REQUIRE_FALSE(var_vtester->GetName() == variable_xtester);
    REQUIRE_FALSE(var_xtester->GetName() == variable_vtester);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_var_vtester = dynamic_cast<Entity*>(var_vtester);
    Entity* entity_var_xtester = dynamic_cast<Entity*>(var_xtester);
    REQUIRE(entity_var_vtester);
    REQUIRE(entity_var_xtester);

    Variable* cast_var_vtester = dynamic_cast<Variable*>(entity_var_vtester);
    Variable* cast_var_xtester = dynamic_cast<Variable*>(entity_var_xtester);
    REQUIRE(cast_var_vtester);
    REQUIRE(cast_var_xtester);

    REQUIRE(var_vtester == cast_var_vtester);
    REQUIRE(var_xtester == cast_var_xtester);
  }
}

/// 2. Test Procedure
TEST_CASE("1.Model.Entity.Procedure") {
  ProcedureName* pname_vtester = new ProcedureName("Vtester");
  ProcedureName* pname_xtester = new ProcedureName("xtesteR");

  Procedure* proc_vtester = new Procedure(pname_vtester);
  Procedure* proc_xtester = new Procedure(pname_xtester);

  SECTION("GetName") {
    REQUIRE(proc_vtester->GetName() == pname_vtester);
    REQUIRE(proc_xtester->GetName() == pname_xtester);

    REQUIRE_FALSE(proc_vtester->GetName() == pname_xtester);
    REQUIRE_FALSE(proc_xtester->GetName() == pname_vtester);
  }

  SECTION("Casting to Entity and back") {
    Entity* entity_proc_vtester = dynamic_cast<Entity*>(proc_vtester);
    Entity* entity_proc_xtester = dynamic_cast<Entity*>(proc_xtester);
    REQUIRE(entity_proc_vtester);
    REQUIRE(entity_proc_xtester);

    Procedure* cast_proc_vtester = dynamic_cast<Procedure*>(entity_proc_vtester);
    Procedure* cast_proc_xtester = dynamic_cast<Procedure*>(entity_proc_xtester);
    REQUIRE(cast_proc_vtester);
    REQUIRE(cast_proc_xtester);

    REQUIRE(proc_vtester == cast_proc_vtester);
    REQUIRE(proc_xtester == cast_proc_xtester);
  }

  SECTION("Casting to Container and back") {
    Container* container_proc_vtester = dynamic_cast<Container*>(proc_vtester);
    Container* container_proc_xtester = dynamic_cast<Container*>(proc_xtester);
    REQUIRE(container_proc_vtester);
    REQUIRE(container_proc_xtester);

    Procedure* cast_proc_vtester = dynamic_cast<Procedure*>(container_proc_vtester);
    Procedure* cast_proc_xtester = dynamic_cast<Procedure*>(container_proc_xtester);
    REQUIRE(cast_proc_vtester);
    REQUIRE(cast_proc_xtester);

    REQUIRE(proc_vtester == cast_proc_vtester);
    REQUIRE(proc_xtester == cast_proc_xtester);
  }
  
  SECTION("Cast to Container and AddStmt") {
    Container* container_proc_vtester = dynamic_cast<Container*>(proc_vtester);

    // 3 sample statements
    ReadEntity* stmt1 = new ReadEntity(new Variable(new VariableName("aaa")));
    PrintEntity* stmt2 = new PrintEntity(new Variable(new VariableName("bbb")));
    CallEntity* stmt3 = new CallEntity(proc_xtester);

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
  Procedure* proc1 = new Procedure(new ProcedureName("HellO"));
  Procedure* proc2 = new Procedure(new ProcedureName("MellO"));
  Procedure* proc3 = new Procedure(new ProcedureName("KellO"));

  Program* program = new Program(proc1);
  program->AddProcedure(proc2);
  program->AddProcedure(proc3);

  REQUIRE(program->GetProcedureList()->front() == proc1);
  program->GetProcedureList()->pop_front();
  REQUIRE(program->GetProcedureList()->front() == proc2);
  program->GetProcedureList()->pop_front();
  REQUIRE(program->GetProcedureList()->front() == proc3);
}