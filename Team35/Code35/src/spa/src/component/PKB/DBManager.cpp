#include "DBManager.h"

DBManager::DBManager(PKB* pkb) {
  this->pkb_ = pkb;
  this->runtime_extractor_ = new RuntimeExtractor(pkb);
}

void DBManager::PopulateDataStructures(Deliverable d) {
  pkb_->PopulateDataStructures(d);
}

/**
 * Gets relationships for 1 specific entity, e.g. Uses(3, _)
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Vector of entities that satisfy the query
 */
std::vector<Entity*> DBManager::GetRelationship(PKBRelRefs ref, std::string entity) {
  switch (ref) {
    case PKBRelRefs::kNext: // todo: add runtime cases
    default: return pkb_->GetRelationship(ref, entity);
  }
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @return Vector of entities that satisfy the query
 */
std::vector<Entity*> DBManager::GetRelationshipByType(PKBRelRefs ref, DesignEntity de) {
  switch (ref) {
    default: return pkb_->GetRelationshipByType(ref, de);
  }
}

/**
 * Gets relationships for 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return Vector of entities that satisfy the query
 */
std::vector<std::tuple<Entity*, Entity*>> DBManager::GetRelationshipByTypes(PKBRelRefs ref,
                                                                            DesignEntity first,
                                                                            DesignEntity second) {
  switch (ref) {
    default: return pkb_->GetRelationshipByTypes(ref, first, second);
  }
}

/**
 * To query for existence of relationship, e.g. Parent(_, _)
 * @param ref Relationship type
 * @return true if relationship exists
 */
bool DBManager::HasRelationship(PKBRelRefs ref) {
  switch (ref) {
    default: return pkb_->HasRelationship(ref);
  }
}

/**
 * To query for existence of relationship with 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return true if relationship exists
 */
bool DBManager::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) {
  switch (ref) {
    default: return pkb_->HasRelationship(ref, first, second);
  }
}

/**
 * To query for existence of relationship with 2 specific entities, e.g. Uses("3", "x")
 * @param ref Relationship type
 * @param first String of first entity
 * @param second String of second entity
 * @return true if relationship exists
 */
bool DBManager::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  switch (ref) {
    default: return pkb_->HasRelationship(ref, first, second);
  }
}

std::vector<Entity*> DBManager::GetDesignEntities(DesignEntity de) {
  return pkb_->GetDesignEntities(de);
}

std::vector<Entity*> DBManager::GetAssignEntityByStmtRef(std::string stmtRef) {
  return pkb_->GetAssignEntityByStmtRef(stmtRef);
}

std::vector<Entity*> DBManager::GetAssignEntityByVariable(std::string variable) {
  return pkb_->GetAssignEntityByVariable(variable);
}

std::vector<Entity*> DBManager::GetWhileEntityByStmtRef(std::string stmtRef) {
  return pkb_->GetWhileEntityByStmtRef(stmtRef);
}

std::vector<Entity*> DBManager::GetWhileEntityByVariable(std::string variable) {
  return pkb_->GetWhileEntityByVariable(variable);
}

std::vector<Entity*> DBManager::GetIfEntityByStmtRef(std::string stmtRef) {
  return pkb_->GetIfEntityByStmtRef(stmtRef);
}

std::vector<Entity*> DBManager::GetIfEntityByVariable(std::string variable) {
  return pkb_->GetIfEntityByVariable(variable);
}

DesignEntity DBManager::EntityToDesignEntity(Entity* entity) {
  return pkb_->EntityToDesignEntity(entity);
}

std::string DBManager::GetNameFromEntity(Entity* entity) {
  return PKB::GetNameFromEntity(entity);
}

void DBManager::Delete() {
  runtime_extractor_->Delete();
  delete runtime_extractor_;
}
