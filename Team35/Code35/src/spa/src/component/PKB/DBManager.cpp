#include "DBManager.h"
#include "util/Utility.h"

DBManager::DBManager(PKB* pkb) {
  this->pkb_ = pkb;
  this->runtime_extractor_ = new RuntimeExtractor(pkb);
}

void DBManager::Delete() {
  runtime_extractor_->Delete();
  delete runtime_extractor_;
}

void DBManager::TurnOffOptimization() {
  is_optimized_ = false;
}

void DBManager::PopulateDataStructures(Deliverable d) {
  pkb_->PopulateDataStructures(d);
}

/**
 * Gets relationships for 1 specific entity, e.g. Uses(3, s)
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Entities that are on the RHS of the relationship with this entity.
 */
std::vector<Entity*> DBManager::GetRelationship(PKBRelRefs ref, std::string entity) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->GetNextT(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kPreviousT: return runtime_extractor_->GetPrevT(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffects: return runtime_extractor_->GetAffects(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->GetAffectedBy(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectsT: return runtime_extractor_->GetAffectsT(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->GetAffectedByT(Utility::ConvertStringToInt(entity));
    default: return pkb_->GetRelationship(ref, entity);
  }
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @return All entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> DBManager::GetRelationshipByType(PKBRelRefs ref, DesignEntity de) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->GetNextT(de);
    case PKBRelRefs::kPreviousT: return runtime_extractor_->GetPrevT(de);
    case PKBRelRefs::kAffects: return runtime_extractor_->GetAffects(de);
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->GetAffectedBy(de);
    case PKBRelRefs::kAffectsT: return runtime_extractor_->GetAffectsT(de);
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->GetAffectedByT(de);
    default: return pkb_->GetRelationshipByType(ref, de);
  }
}

/**
 * Gets relationships for 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return All possible combinations of entities in the relationship.
 */
std::vector<std::tuple<Entity*, Entity*>> DBManager::GetRelationshipByTypes(PKBRelRefs ref,
                                                                            DesignEntity first,
                                                                            DesignEntity second) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->GetNextT(first, second);
    case PKBRelRefs::kPreviousT: return runtime_extractor_->GetPrevT(first, second);
    case PKBRelRefs::kAffects: return runtime_extractor_->GetAffects(first, second);
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->GetAffectedBy(first, second);
    case PKBRelRefs::kAffectsT: return runtime_extractor_->GetAffectsT(first, second);
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->GetAffectedByT(first, second);
    default: return pkb_->GetRelationshipByTypes(ref, first, second);
  }
}

/**
 * To query for existence of relationship, e.g. Parent(_, _)
 * @param ref Relationship type
 * @return true if any relationship exists.
 */
bool DBManager::HasRelationship(PKBRelRefs ref) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return runtime_extractor_->HasAffects();
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->HasAffectedBy();
    case PKBRelRefs::kAffectsT: return runtime_extractor_->HasAffects();
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->HasAffectedBy();
    default: return pkb_->HasRelationship(ref);
  }
}

/**
 * To query for existence of relationship with 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return true if any relationship with the specified types exists.
 */
bool DBManager::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return runtime_extractor_->HasAffects();
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->HasAffectedBy();
    case PKBRelRefs::kAffectsT: return runtime_extractor_->HasAffects();
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->HasAffectedBy();
    default: return pkb_->HasRelationship(ref, first, second);
  }
}

/**
 * To query for existence of relationship for 1 specific entity, e.g. Next(3, _)
 * @param ref Relationship type
 * @param entity String form of statement number to query
 * @return true if any relationship with the specified types exists.
 * @throws SyntaxException - when it is not an integer or outside of 2^32-1
 */
bool DBManager::HasRelationship(PKBRelRefs ref, std::string entity) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->HasNextT(Utility::ConvertStringToInt(entity));  // todo: use new pkb api to query for hasnext(string)
    case PKBRelRefs::kPreviousT: return runtime_extractor_->HasPrevT(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffects: return runtime_extractor_->HasAffects(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->HasAffectedBy(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectsT: return runtime_extractor_->HasAffectsT(Utility::ConvertStringToInt(entity));
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->HasAffectedByT(Utility::ConvertStringToInt(entity));
    default: return false;  // todo: insert new api here
  }
}

/**
 * To query for existence of relationship with 2 specific entities, e.g. Next("3", "4")
 * @param ref Relationship type
 * @param first String of first statement number
 * @param second String of second statement number
 * @return true if relationship between the prog_line / assignment exists.
 * @throws SyntaxException - when it is not an integer or outside of 2^32-1
 */
bool DBManager::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  switch (ref) {
    case PKBRelRefs::kNextT:
      return runtime_extractor_->HasNextT(Utility::ConvertStringToInt(first),
                                          Utility::ConvertStringToInt(second));
    case PKBRelRefs::kPreviousT:
      return runtime_extractor_->HasPrevT(Utility::ConvertStringToInt(first),
                                          Utility::ConvertStringToInt(second));
    case PKBRelRefs::kAffects:
      return runtime_extractor_->HasAffects(Utility::ConvertStringToInt(first),
                                            Utility::ConvertStringToInt(second));
    case PKBRelRefs::kAffectedBy:
      return runtime_extractor_->HasAffectedBy(Utility::ConvertStringToInt(first),
                                               Utility::ConvertStringToInt(second));
    case PKBRelRefs::kAffectsT:
      return runtime_extractor_->HasAffectsT(Utility::ConvertStringToInt(first),
                                             Utility::ConvertStringToInt(second));
    case PKBRelRefs::kAffectedByT:
      return runtime_extractor_->HasAffectedByT(Utility::ConvertStringToInt(first),
                                                Utility::ConvertStringToInt(second));
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
