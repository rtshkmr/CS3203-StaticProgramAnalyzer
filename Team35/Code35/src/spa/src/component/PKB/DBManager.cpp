#include "DBManager.h"

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
 * Gets relationships for 1 specific entity, e.g. Uses(3, _) or Affects(3, a)
 * but not (3, _) for runtime relationships.
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Entities that are on the RHS of the relationship with this entity.
 */
std::vector<Entity*> DBManager::GetRelationship(PKBRelRefs ref, std::string entity) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->GetNextT(entity);
    case PKBRelRefs::kPreviousT: return runtime_extractor_->GetPrevT(entity);
    case PKBRelRefs::kAffects: return runtime_extractor_->GetAffects(entity);
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->GetAffectedBy(entity);
    case PKBRelRefs::kAffectsT: return runtime_extractor_->GetAffectsT(entity);
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->GetAffectedByT(entity);
    default: return pkb_->GetRelationship(ref, entity);
  }
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @return All entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> DBManager::GetRelationshipByTypeUnscoped(PKBRelRefs ref, DesignEntity de) {
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
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @param scoped_entities Entities that have already been scoped
 * @param scope_indication Enum describing if result has already been scoped
 * @return All entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> DBManager::GetRelationshipByType(PKBRelRefs ref,
                                                      DesignEntity de,
                                                      std::vector<Entity *> scoped_entities,
                                                      ScopeIndication scope_indication) {
  // Retrieve all entities from PKB/RuntimeExtractor
  std::vector<Entity*> unscoped_entities = GetRelationshipByTypeUnscoped(ref, de);

  // If there is no scope, return the unscoped_entities from PKB/RuntimeExtractor
  if (scope_indication == ScopeIndication::kNoScope) {
    // No scope
    return unscoped_entities;
  } else {
    // Scoped
    // Check if pkb contains the relationship type
    const bool pkb_ref = pkb_rel_set.find(ref) != pkb_rel_set.end();
    if (pkb_ref) {
      // PKB contains the relationship type

      // Scoped entities will always be a subset of unscoped entities
      // Can simply check if scoped_entities is smaller than unscoped_entities
      // and return the smaller vector
      if (scoped_entities.size() <= unscoped_entities.size()) {
        return scoped_entities;
      } else {
        return unscoped_entities;
      }
    } else {
      // PKB does not contain the relationship type
      // (insert RuntimeExtractor code here)

    }
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
 *
 * @param ref
 * @param first_de
 * @param second_de
 * @param left_scoped_entities
 * @param right_scoped_entities
 * @param scope_indication
 * @return
 */
std::vector<Entity*> DBManager::GetRelationshipByTypes(PKBRelRefs ref,
                                                       DesignEntity first_de,
                                                       DesignEntity second_de,
                                                       std::vector<Entity *> left_scoped_entities,
                                                       std::vector<Entity *> right_scoped_entities,
                                                       ScopeIndication scope_indication) {
  // Retrieve all entities from PKB/RuntimeExtractor
  std::vector<Entity*> unscoped_entities = pkb_->GetFirstEntityOfRelationship(ref, first_de, second_de);

  // If there is no scope, return the unscoped_entities from PKB/RuntimeExtractor
  if (scope_indication == ScopeIndication::kNoScope) {
    // No scope
    return unscoped_entities;
  } else {
    // Scoped
    // Check if pkb contains the relationship type
    const bool pkb_ref = pkb_rel_set.find(ref) != pkb_rel_set.end();
    if (pkb_ref) {
      // PKB contains the relationship type

      // Scoped entities will always be a subset of unscoped entities
      // Can simply check if lef_scoped_entities is smaller than unscoped_entities
      // and return the smaller vector
      if (left_scoped_entities.size() <= unscoped_entities.size()) {
        return left_scoped_entities;
      } else {
        return unscoped_entities;
      }
    } else {
      // PKB does not contain the relationship type
      // (insert RuntimeExtractor code here)

    }
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

bool DBManager::HasRelationship(PKBRelRefs ref, std::string ident_string) {
  const bool pkb_ref = pkb_rel_set.find(ref) != pkb_rel_set.end();
  if (pkb_ref) {
    // PKB
    return pkb_->HasRelationship(ref, ident_string);
  } else {
    // RuntimeExtractor
  }
}
/**
 * To query for existence of relationship with 2 specific entities, e.g. Uses("3", "x")
 * @param ref Relationship type
 * @param first String of first entity
 * @param second String of second entity
 * @return true if relationship between the entities exists.
 */
bool DBManager::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  switch (ref) {
    case PKBRelRefs::kNextT: return runtime_extractor_->HasNextT(first, second);
    case PKBRelRefs::kPreviousT: return runtime_extractor_->HasPrevT(first, second);
    case PKBRelRefs::kAffects: return runtime_extractor_->HasAffects(first, second);
    case PKBRelRefs::kAffectedBy: return runtime_extractor_->HasAffectedBy(first, second);
    case PKBRelRefs::kAffectsT: return runtime_extractor_->HasAffectsT(first, second);
    case PKBRelRefs::kAffectedByT: return runtime_extractor_->HasAffectedByT(first, second);
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


