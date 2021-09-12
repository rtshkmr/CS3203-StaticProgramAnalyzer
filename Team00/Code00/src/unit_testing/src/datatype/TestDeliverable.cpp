#include "catch.hpp"
#include "datatype/Deliverable.h"
#include "../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("Deliverable") {
  SECTION("AddUsesRelationship for containers") {
    Deliverable deliverable;

    IfEntity* if_entity1 = if_1_;
    ElseEntity* else_entity1 = else_1_;

    if_entity1->AddStatement(assign_4_);
    else_entity1->AddStatement(assign_6_);
    if_entity1->SetElseEntity(else_1_);

    deliverable.AddUsesRelationship(dynamic_cast<Container*>(if_entity1), var_x_);
    REQUIRE(deliverable.container_use_hash_.find(if_entity1) != deliverable.container_use_hash_.end());
    REQUIRE(deliverable.container_use_hash_.find(if_entity1)->second->front() == var_x_);

    std::list<Variable*> var_list = {var_z_, var_y_};
    deliverable.AddUsesRelationship(dynamic_cast<Container*>(if_entity1), &var_list);
    REQUIRE(deliverable.container_use_hash_.find(if_entity1)->second->size() == 3);
  }
}