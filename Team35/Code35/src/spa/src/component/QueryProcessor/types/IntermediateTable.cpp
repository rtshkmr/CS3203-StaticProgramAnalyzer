#include "IntermediateTable.h"

#include <utility>

IntermediateTable::IntermediateTable() {
  std::vector<Entity *> empty_relationships_vector;
  relationships = empty_relationships_vector;

  std::vector<std::tuple<Entity *, Entity *>> empty_relationships_by_type_vector;
  relationships_by_type = empty_relationships_by_type_vector;

  has_relationship = false;
}

void IntermediateTable::InsertData(std::vector<Entity *> data) {
  relationships = std::move(data);
}

void IntermediateTable::InsertData(std::vector<std::tuple<Entity *, Entity *>> data) {
  relationships_by_type = std::move(data);
}

void IntermediateTable::InsertData(bool has_relation) {
  this->has_relationship = has_relation;
}

std::vector<Entity *> IntermediateTable::GetRelationships() {
  return relationships;
}

std::vector<std::tuple<Entity *, Entity *>> IntermediateTable::GetRelationshipsByType() {
  return relationships_by_type;
}

bool IntermediateTable::GetExistenceResult() const {
  return has_relationship;
}
