#include "DBManager.h"

DBManager::DBManager(PKB* pkb) {
  this->pkb_ = pkb;
  this->runtime_extractor_ = new RuntimeExtractor(pkb);
}

void DBManager::PopulateDataStructures(Deliverable d) {
  pkb_->PopulateDataStructures(d);
}

std::vector<Entity*> DBManager::GetRelationship(PKBRelRefs ref, std::string entity) {
  switch (ref) {
    // todo: add runtime cases
    default: return pkb_->GetRelationship(ref, entity);
  }
}

std::vector<Entity*> DBManager::GetRelationshipByType(PKBRelRefs ref, DesignEntity de) {
  switch (ref) {
    default: return pkb_->GetRelationshipByType(ref, de);
  }
}

std::vector<std::tuple<Entity*, Entity*>> DBManager::GetRelationshipByTypes(PKBRelRefs ref,
                                                                            DesignEntity first,
                                                                            DesignEntity second) {
  switch (ref) {
    default: return pkb_->GetRelationshipByTypes(ref, first, second);
  }
}

bool DBManager::HasRelationship(PKBRelRefs ref) {
  switch (ref) {
    default: return pkb_->HasRelationship(ref);
  }
}

bool DBManager::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) {
  switch (ref) {
    default: return pkb_->HasRelationship(ref, first, second);
  }
}

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
