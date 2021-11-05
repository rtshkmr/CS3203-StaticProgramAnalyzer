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

/**
 * Gets relationships for 1 specific entity, e.g. Uses(3, s)
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Entities that are on the RHS of the relationship with this entity.
 */
std::vector<Entity*> DBManager::GetRelationship(PKBRelRefs ref, std::string entity) {
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->GetRelationship(ref, entity);
  } else {
    return pkb_->GetRelationship(ref, entity);
  }
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @return All entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> DBManager::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) {
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->GetFirstEntityOfRelationship(ref, de);
  } else {
    return pkb_->GetFirstEntityOfRelationship(ref, de);
  }
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _) where s are entities in the scoped vector.
 * @param ref Relationship type
 * @param de Type of entity to query
 * @param scoped_entities Entities that have already been scoped
 * @param scope_indication Enum describing if result has already been scoped
 * @return All entities in the scope that can be on the LHS of the relationship.
 */
std::vector<Entity*> DBManager::GetFirstEntityOfRelationship(PKBRelRefs ref,
                                                             DesignEntity de,
                                                             std::vector<Entity*> scoped_entities,
                                                             ScopeIndication scope_indication) {
  // If there is no scope, return the unscoped_entities from PKB/RuntimeExtractor
  if (scope_indication == ScopeIndication::kNoScope) {
    return GetFirstEntityOfRelationship(ref, de);
  } else {
    // Check if pkb contains the relationship type
    const bool pkb_ref = preprocessed_rel_refs.find(ref) != preprocessed_rel_refs.end();
    if (pkb_ref) {
      // PKB contains the relationship type
      std::vector<Entity*> unscoped_entities = pkb_->GetFirstEntityOfRelationship(ref, de);

      // Scoped entities will always be a subset of unscoped entities
      // Can simply check if scoped_entities is smaller than unscoped_entities and return the smaller vector
      if (scoped_entities.size() <= unscoped_entities.size()) {
        return scoped_entities;
      } else {
        return unscoped_entities;
      }
    } else {
      // PKB does not contain the relationship type
      return runtime_extractor_->GetScopedFirstEntities(ref, scoped_entities);
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
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->GetRelationshipByTypes(ref, first, second);
  } else {
    return pkb_->GetRelationshipByTypes(ref, first, second);
  }
}

/**
 * Gets relationships for 2 type-specified entities, e.g. Uses(p, v) where p and v are entities in the scoped vectors.
 * @param ref Relationship type
 * @param first_de Type of first entity
 * @param second_de Type of second entity
 * @param left_scoped_entities Scoped entities on the LHS of the relationship
 * @param right_scoped_entities Scoped entities on the RHS of the relationship
 * @param scope_indication Enum describing if result has already been scoped
 * @return All possible combinations of entities in the relationship in the scope.
 */
std::vector<std::tuple<Entity*, Entity*>> DBManager::GetRelationshipByTypes(PKBRelRefs ref,
                                                                            DesignEntity first_de,
                                                                            DesignEntity second_de,
                                                                            std::vector<Entity*> left_scoped_entities,
                                                                            std::vector<Entity*> right_scoped_entities,
                                                                            ScopeIndication scope_indication) {
//  kNoScope:
//  kLeftScope:
//  (1, 2, 3)
//  GetRelationship(ref, 1, s)
//
//  kAllScope:
//    1. Iterate through left and right by index and check HasRElationship for each
//    2. Get all of the relationship and find the intersection


  // If there is no scope, return the unscoped_entities from PKB/RuntimeExtractor
  if (scope_indication == ScopeIndication::kNoScope) {
    return GetRelationshipByTypes(ref, first_de, second_de);
  } else {
    // Check if pkb contains the relationship type
    const bool pkb_ref = preprocessed_rel_refs.find(ref) != preprocessed_rel_refs.end();
    if (pkb_ref) {
      if (scope_indication == ScopeIndication::kAllScope) {
        std::vector<std::tuple<Entity*, Entity*>> results(left_scoped_entities.size());
        for (int i = 0; i < left_scoped_entities.size(); i++) {
          if (pkb_->HasRelationship(
            ref,
            GetNameFromEntity(left_scoped_entities[i]),
            GetNameFromEntity(right_scoped_entities[i])))
            results.emplace_back(left_scoped_entities[i], right_scoped_entities[i]);
        }
        return results;
      } else {
        std::vector<entity_pair> results;
        auto scoped_entities = scope_indication == ScopeIndication::kLeftScope ? left_scoped_entities : right_scoped_entities;
        for (Entity* entity : scoped_entities) {
          std::vector<entity_pair> pairs;
          if (scope_indication == ScopeIndication::kLeftScope)
            pairs = pkb_->GetRelationshipByFirst(ref, GetNameFromEntity(entity));
          if (scope_indication == ScopeIndication::kRightScope)
            pairs = pkb_->GetRelationshipBySecond(ref, GetNameFromEntity(entity));
          for (entity_pair pair : pairs) {
            if (GetDesignEntityFromEntity(std::get<0>(pair)) != first_de)
              continue;
            if (GetDesignEntityFromEntity(std::get<1>(pair)) != second_de)
              continue;
            results.insert(results.end(), pairs.begin(), pairs.end());
          }
        }
        return results;
      }
    } else {
      // PKB does not contain the relationship type
      return runtime_extractor_->GetAllRelationshipsScoped(ref, left_scoped_entities, right_scoped_entities);
    }
  }
}

std::vector<std::tuple<Entity*, Entity*>> DBManager::MergeScopedEntities(std::vector<Entity*> left_entities,
                                                                         std::vector<Entity*> right_entities) {
  std::vector<std::tuple<Entity*, Entity*>> results(left_entities.size());
  for (int i = 0; i < left_entities.size(); ++i) {
    results.emplace_back(left_entities[i], right_entities[i]);
  }
  return results;
}

/**
 * To query for existence of relationship, e.g. Parent(_, _)
 * @param ref Relationship type
 * @return true if any relationship exists.
 */
bool DBManager::HasRelationship(PKBRelRefs ref) {
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->HasRelationship(ref);
  } else {
    return pkb_->HasRelationship(ref);
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
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->HasRelationship(ref, first, second);
  } else {
    return pkb_->HasRelationship(ref, first, second);
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
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->HasRelationship(ref, entity);
  } else {
    return pkb_->HasRelationship(ref, entity);
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
  if (RuntimeExtractor::IsRuntimeRelationship(ref)) {
    return runtime_extractor_->HasRelationship(ref, first, second);
  } else {
    return pkb_->HasRelationship(ref, first, second);
  }
}

std::vector<Entity*> DBManager::GetDesignEntities(DesignEntity de) {
  return pkb_->GetDesignEntities(de);
}

std::vector<Entity*> DBManager::GetPatternEntities(DesignEntity de, std::string var_or_stmt) {
  return pkb_->GetPatternEntities(de, var_or_stmt);
}

DesignEntity DBManager::GetDesignEntityFromEntity(Entity* entity) {
  return pkb_->GetDesignEntityFromEntity(entity);
}

std::string DBManager::GetNameFromEntity(Entity* entity) {
  return PKB::GetNameFromEntity(entity);
}

std::vector<Entity*> DBManager::GetEntitiesWithAttributeValue(DesignEntity design_entity,
                                                              Attribute attribute,
                                                              std::string value) {
  return pkb_->GetEntitiesWithAttributeValue(design_entity, attribute, value);
}

std::vector<entity_pair> DBManager::GetEntitiesWithMatchingAttributes(type_attribute_pair type_one,
                                                                      type_attribute_pair type_two) {
  return pkb_->GetEntitiesWithMatchingAttributes(type_one, type_two);
}
