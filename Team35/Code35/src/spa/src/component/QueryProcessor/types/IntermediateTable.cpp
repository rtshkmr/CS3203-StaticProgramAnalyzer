#include "IntermediateTable.h"

IntermediateTable::IntermediateTable() {
  std::vector<Entity *> empty_relationships_vector;
  relationships = empty_relationships_vector;

  std::vector<std::tuple<Entity *, Entity *>> empty_relationships_by_type_vector;
  relationships_by_type = empty_relationships_by_type_vector;

  has_relationship = false;
};

void IntermediateTable::InsertData(std::vector<Entity *> data) {
  relationships = data;
}

void IntermediateTable::InsertData(std::vector<std::tuple<Entity *, Entity *>> data) {
  relationships_by_type = data;
}

void IntermediateTable::InsertData(bool has_relationship) {
  this->has_relationship = has_relationship;
}

std::vector<Entity *> IntermediateTable::GetRelationships() {
  return relationships;
}

std::vector<std::tuple<Entity *, Entity *>> IntermediateTable::GetRelationshipsByType() {
  return relationships_by_type;
}

bool IntermediateTable::GetExistenceResult() {
  return has_relationship;
}
