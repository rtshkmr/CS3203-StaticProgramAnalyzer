#include "catch.hpp"
#include "datatype/Deliverable.h"
#include "../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("Deliverable") {
  Deliverable deliverable;
  IfEntity* if_1 = GetIf1();
  ElseEntity* else_1 = GetElse();
  AssignEntity* assign_4 = GetAssign4();
  AssignEntity* assign_6 = GetAssign6();

  SECTION("AddUsesRelationship for containers") {

    if_1->AddStatement(assign_4);
    else_1->AddStatement(assign_6);
    if_1->SetElseEntity(else_1);

    deliverable.AddUsesRelationship(dynamic_cast<Container*>(if_1), var_x_);
    REQUIRE(deliverable.container_use_hash_.find(if_1) != deliverable.container_use_hash_.end());
    REQUIRE(deliverable.container_use_hash_.find(if_1)->second->front() == var_x_);

    std::list<Variable*> var_list = {var_z_, var_y_};
    deliverable.AddUsesRelationship(dynamic_cast<Container*>(if_1), &var_list);
    REQUIRE(deliverable.container_use_hash_.find(if_1)->second->size() == 3);
  }
}