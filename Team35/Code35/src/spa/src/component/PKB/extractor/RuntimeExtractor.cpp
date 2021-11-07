#include <util/Utility.h>
#include "RuntimeExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  next_t_extractor_ = NextTExtractor(pkb);
  affects_extractor_ = AffectsExtractor(this, pkb);
  next_bip_extractor_ = NextBipExtractor(pkb);
  next_bip_t_extractor_ = NextBipTExtractor(this, pkb);
  affects_t_extractor_ = AffectsTExtractor(this, pkb);
  affects_bip_extractor_ = AffectsBipExtractor(this, pkb);
  affects_bip_t_extractor_ = AffectsBipTExtractor(this, pkb);
}

/**
 * Gets relationships for 1 specific entity, e.g. Uses(3, s)
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Entities that are on the RHS of the relationship with this entity.
 */
std::vector<Entity*> RuntimeExtractor::GetRelationship(PKBRelRefs ref, std::string target) {
  int target_num = Utility::ConvertStringToInt(target);
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) ? pair.first->GetRelationship(pair.second, target_num) : std::vector<Entity*>{};
}

/**
 * Gets relationships for 1 type-specified entity, e.g. Uses(s, _)
 * @param ref Relationship type
 * @param de Type of entity to query
 * @param scoped_entities Entities that have already been scoped
 * @param scope_indication Enum describing if result has already been scoped
 * @return All entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> RuntimeExtractor::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) {
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) ? pair.first->GetFirstEntityOfRelationship(pair.second, de) : std::vector<Entity*>{};
}

/**
 * Gets relationships for 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return All possible combinations of entities in the relationship.
 */
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetRelationshipByTypes(PKBRelRefs ref,
                                                                                   DesignEntity first,
                                                                                   DesignEntity second) {
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) ? pair.first->GetRelationshipByTypes(pair.second, first, second)
                      : std::vector<std::tuple<Entity*, Entity*>>{};
}

/**
 * Gets relationships for 2 type-specified entities, e.g. Affects(a, b) where a and b are entities in the scoped vectors.
 * @param ref Relationship type
 * @param left_entities Scoped entities on the LHS of the relationship
 * @param right_entities Scoped entities on the RHS of the relationship
 * @return All possible combinations of entities in the relationship in the scope.
 */
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAllRelationshipsScoped(PKBRelRefs ref,
                                                                                      std::vector<Entity*> left_entities,
                                                                                      std::vector<Entity*> right_entities,
                                                                                      ScopeIndication scope_indication,
                                                                                      type_combo types) {
  DesignEntity type_one = std::get<0>(types);
  DesignEntity type_two = std::get<1>(types);
  std::vector<std::tuple<Entity*, Entity*>> results;
  if (scope_indication == ScopeIndication::kLeftScope || scope_indication == ScopeIndication::kRightScope) {
    std::vector<Entity*> to_iter = scope_indication == ScopeIndication::kLeftScope ? left_entities : right_entities;
    if (scope_indication == ScopeIndication::kRightScope) ref = ReverseRelationship(ref);
    for (Entity* scoped_entity : to_iter) {
      // todo: See if can replace the below with a single function that returns vector<entity_pair>
      std::vector<Entity*> add = GetRelationship(ref, PKB::GetNameFromEntity(scoped_entity));
      for (Entity* unscoped_entity : add) {
        DesignEntity unscoped_type = PKB::GetDesignEntityFromEntity(unscoped_entity);
        if (scope_indication == ScopeIndication::kLeftScope) {
          if (unscoped_type == type_two || type_two == DesignEntity::kStmt || type_two == DesignEntity::kProgLine)
            results.emplace_back(scoped_entity, unscoped_entity);
        } else {
          if (unscoped_type == type_one || type_one == DesignEntity::kStmt || type_one == DesignEntity::kProgLine)
            results.emplace_back(unscoped_entity, scoped_entity);
        }
      }
    }
    return results;
  }

  for (int i = 0; i < left_entities.size(); ++i) {
    std::string left_name = PKB::GetNameFromEntity(left_entities[i]);
    std::string right_name = PKB::GetNameFromEntity(right_entities[i]);
    if (HasRelationship(ref, left_name, right_name)) {
      results.emplace_back(left_entities[i], right_entities[i]);
    }
  }
  return results;
}

/**
 * To query for existence of relationship, e.g. Parent(_, _)
 * @param ref Relationship type
 * @return true if any relationship exists.
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref) {
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) != nullptr && pair.first->HasRelationship(pair.second);
}

/**
 * To query for existence of relationship for 1 specific entity, e.g. Next(3, _)
 * @param ref Relationship type
 * @param entity String form of statement number to query
 * @return true if any relationship with the specified types exists.
 * @throws SyntaxException - when it is not an integer or outside of 2^32-1
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, std::string first) {
  int target_num = Utility::ConvertStringToInt(first);
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) != nullptr && pair.first->HasRelationship(pair.second, target_num);
}

/**
 * To query for existence of relationship with 2 specific entities, e.g. Next("3", "4")
 * @param ref Relationship type
 * @param first String of first statement number
 * @param second String of second statement number
 * @return true if relationship between the prog_line / assignment exists.
 * @throws SyntaxException - when it is not an integer or outside of 2^32-1
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  int first_num = Utility::ConvertStringToInt(first);
  int second_num = Utility::ConvertStringToInt(second);
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) != nullptr && pair.first->HasRelationship(pair.second, first_num, second_num);
}

/**
 * To query for existence of relationship with 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return true if any relationship with the specified types exists.
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) {
  std::pair<RuntimeColleague*,RelDirection> pair = GetExtractorAndDirection(ref);
  return (pair.first) != nullptr && pair.first->HasRelationship(pair.second, first, second);
}

std::pair<RuntimeColleague*,RelDirection> RuntimeExtractor::GetExtractorAndDirection(PKBRelRefs ref) {
  switch (ref) {
    case PKBRelRefs::kNextT: return std::pair(&next_t_extractor_, RelDirection::kForward);
    case PKBRelRefs::kPreviousT: return std::pair(&next_t_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kAffects: return std::pair(&affects_extractor_, RelDirection::kForward);
    case PKBRelRefs::kAffectedBy: return std::pair(&affects_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kAffectsT: return std::pair(&affects_t_extractor_, RelDirection::kForward);
    case PKBRelRefs::kAffectedByT:  return std::pair(&affects_t_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kNextBip: return std::pair(&next_bip_extractor_, RelDirection::kForward);
    case PKBRelRefs::kPrevBip: return std::pair(&next_bip_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kNextBipT: return std::pair(&next_bip_t_extractor_, RelDirection::kForward);
    case PKBRelRefs::kPrevBipT: return std::pair(&next_bip_t_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kAffectsBip: return std::pair(&affects_bip_extractor_, RelDirection::kForward);
    case PKBRelRefs::kAffectedByBip: return std::pair(&affects_bip_extractor_, RelDirection::kReverse);
    case PKBRelRefs::kAffectsBipT: return std::pair(&affects_bip_t_extractor_, RelDirection::kForward);
    case PKBRelRefs::kAffectedByBipT: return std::pair(&affects_bip_t_extractor_, RelDirection::kReverse);
    default: return std::pair(nullptr, RelDirection::kForward);
  }
}

void RuntimeExtractor::Delete() {
  next_t_extractor_.Delete();
  affects_extractor_.Delete();
  affects_t_extractor_.Delete();
  next_bip_extractor_.Delete();
}

bool RuntimeExtractor::IsRuntimeRelationship(PKBRelRefs ref) {
  return runtime_relationships.count(ref) == 1;
}

PKBRelRefs RuntimeExtractor::ReverseRelationship(PKBRelRefs ref) {
  switch (ref) {
    case PKBRelRefs::kNextT: return PKBRelRefs::kPreviousT;
    case PKBRelRefs::kPreviousT: return PKBRelRefs::kNextT;
    case PKBRelRefs::kAffects: return PKBRelRefs::kAffectedBy;
    case PKBRelRefs::kAffectedBy: return PKBRelRefs::kAffects;
    case PKBRelRefs::kAffectsT: return PKBRelRefs::kAffectedByT;
    case PKBRelRefs::kAffectedByT: return PKBRelRefs::kAffectsT;
    case PKBRelRefs::kNextBip: return PKBRelRefs::kPrevBip;
    case PKBRelRefs::kPrevBip: return PKBRelRefs::kNextBip;
    case PKBRelRefs::kNextBipT: return PKBRelRefs::kPrevBipT;
    case PKBRelRefs::kPrevBipT: return PKBRelRefs::kNextBipT;
    default: return PKBRelRefs::kInvalid;
  }
}