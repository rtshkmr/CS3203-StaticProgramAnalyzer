#include <util/Utility.h>
#include "RuntimeExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  pkb_ = pkb;
  next_t_extractor_ = NextTExtractor(pkb);
  affects_extractor_.SetPKB(pkb);
  next_bip_extractor_ = NextBipExtractor(pkb);
  next_bip_t_extractor_ = NextBipTExtractor(pkb);
  next_bip_t_extractor_.SetMediator(this);
  affects_t_extractor_ = AffectsTExtractor(this, pkb);
}

/**
 * Gets relationships for 1 specific entity, e.g. Uses(3, s)
 * @param ref Relationship type
 * @param entity String form of entity to query
 * @return Entities that are on the RHS of the relationship with this entity.
 */
std::vector<Entity*> RuntimeExtractor::GetRelationship(PKBRelRefs ref, std::string target) {
  int target_num = Utility::ConvertStringToInt(target);
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(target_num);
    case PKBRelRefs::kPreviousT: return GetPrevT(target_num);
    case PKBRelRefs::kAffects: return GetAffects(target_num);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(target_num);
    case PKBRelRefs::kAffectsT: return GetAffectsT(target_num);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(target_num);
    case PKBRelRefs::kNextBip: return GetNextBip(target_num);
    case PKBRelRefs::kPrevBip: return GetPrevBip(target_num);
    case PKBRelRefs::kNextBipT: return GetNextBipT(target_num);
    case PKBRelRefs::kPrevBipT: return GetPrevBipT(target_num);
    default: return std::vector<Entity*>{};
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
std::vector<Entity*> RuntimeExtractor::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) {
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(de);
    case PKBRelRefs::kPreviousT: return GetPrevT(de);
    case PKBRelRefs::kAffects: return GetAffects(de);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(de);
    case PKBRelRefs::kAffectsT: return GetAffectsT(de);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(de);
    case PKBRelRefs::kNextBip: return GetNextBip(de);
    case PKBRelRefs::kPrevBip: return GetPrevBip(de);
    case PKBRelRefs::kNextBipT: return GetNextBipT(de);
    case PKBRelRefs::kPrevBipT: return GetPrevBipT(de);
    default: return std::vector<Entity*>{};
  }
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
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(first, second);
    case PKBRelRefs::kPreviousT: return GetPrevT(first, second);
    case PKBRelRefs::kAffects: return GetAffects(first, second);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(first, second);
    case PKBRelRefs::kAffectsT: return GetAffectsT(first, second);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(first, second);
    case PKBRelRefs::kNextBip: return GetNextBip(first, second);
    case PKBRelRefs::kPrevBip: return GetPrevBip(first, second);
    case PKBRelRefs::kNextBipT: return GetNextBipT(first, second);
    case PKBRelRefs::kPrevBipT: return GetPrevBipT(first, second);
    default: return std::vector<std::tuple<Entity*, Entity*>>{};
  }
}

/**
 * To query for existence of relationship, e.g. Parent(_, _)
 * @param ref Relationship type
 * @return true if any relationship exists.
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return HasAffects();
    case PKBRelRefs::kAffectedBy: return HasAffectedBy();
    case PKBRelRefs::kAffectsT: return HasAffectsT();
    case PKBRelRefs::kAffectedByT: return HasAffectedByT();
    case PKBRelRefs::kNextBip: // fallthrough
    case PKBRelRefs::kPrevBip: // fallthrough
    case PKBRelRefs::kNextBipT: // fallthrough
    case PKBRelRefs::kPrevBipT: return HasNextBip();
    default: return false;
  }
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
  switch (ref) {
    case PKBRelRefs::kNextT: // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext, first);
    case PKBRelRefs::kAffects: return HasAffects(target_num);
    case PKBRelRefs::kAffectedBy: return HasAffectedBy(target_num);
    case PKBRelRefs::kAffectsT: return HasAffectsT(target_num);
    case PKBRelRefs::kAffectedByT: return HasAffectedByT(target_num);
    case PKBRelRefs::kNextBip: return HasNextBip(target_num);
    case PKBRelRefs::kPrevBip: return HasPrevBip(target_num);
    case PKBRelRefs::kNextBipT: return HasNextBip(target_num);
    case PKBRelRefs::kPrevBipT: return HasPrevBip(target_num);
    default: return false;
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
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  int first_num = Utility::ConvertStringToInt(first);
  int second_num = Utility::ConvertStringToInt(second);
  switch (ref) {
    case PKBRelRefs::kNextT: return HasNextT(first_num, second_num);
    case PKBRelRefs::kPreviousT: return HasNextT(second_num, first_num);
    case PKBRelRefs::kAffects: return HasAffects(first_num, second_num);
    case PKBRelRefs::kAffectedBy: return HasAffectedBy(first_num, second_num);
    case PKBRelRefs::kAffectsT: return HasAffectsT(first_num, second_num);
    case PKBRelRefs::kAffectedByT: return HasAffectedByT(first_num, second_num);
    case PKBRelRefs::kNextBip: return HasNextBip(first_num, second_num);
    case PKBRelRefs::kPrevBip: return HasNextBip(second_num, first_num);
    case PKBRelRefs::kNextBipT: return HasNextBipT(first_num, second_num);
    case PKBRelRefs::kPrevBipT: return HasNextBipT(second_num, first_num);
    default: return false;
  }
}

/**
 * To query for existence of relationship with 2 type-specified entities, e.g. Uses(p, v)
 * @param ref Relationship type
 * @param first Type of first entity
 * @param second Type of second entity
 * @return true if any relationship with the specified types exists.
 */
bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return HasAffects();
    case PKBRelRefs::kAffectedBy: return HasAffectedBy();
    case PKBRelRefs::kAffectsT: return HasAffects();
    case PKBRelRefs::kAffectedByT: return HasAffectedBy();
    case PKBRelRefs::kNextBip: // fallthrough
    case PKBRelRefs::kPrevBip: // fallthrough
    case PKBRelRefs::kNextBipT: // fallthrough
    case PKBRelRefs::kPrevBipT: return HasNextBip();
    default: return false;
  }
}

std::vector<Entity*> RuntimeExtractor::GetNextT(int target) {
  return next_t_extractor_.GetRelationship(RelDirection::kForward, target);
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(int target) {
  return next_t_extractor_.GetRelationship(RelDirection::kReverse, target);
}

std::vector<Entity*> RuntimeExtractor::GetAffects(int target) {
  return affects_extractor_.GetAffects(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedBy(int target) {
  return affects_extractor_.GetAffectedBy(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectsT(int target) {
  return affects_t_extractor_.GetRelationship(RelDirection::kForward, target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedByT(int target) {
  return affects_t_extractor_.GetRelationship(RelDirection::kReverse, target);
}

std::vector<Entity*> RuntimeExtractor::GetNextBip(int target) {
  return next_bip_extractor_.GetRelationship(RelDirection::kForward, target);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBip(int target) {
  return next_bip_extractor_.GetRelationship(RelDirection::kReverse, target);
}

std::vector<Entity*> RuntimeExtractor::GetNextBipT(int target) {
  return next_bip_t_extractor_.GetRelationship(RelDirection::kForward, target);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBipT(int target) {
  return next_bip_t_extractor_.GetRelationship(RelDirection::kReverse, target);
}

std::vector<Entity*> RuntimeExtractor::GetNextT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kForward);
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kReverse);
}

std::vector<Entity*> RuntimeExtractor::GetAffects(DesignEntity de) {
  if (de == DesignEntity::kStmt || de == DesignEntity::kAssign) {
    return affects_extractor_.GetAllAffects();
  } else {
    return std::vector<Entity*>{};
  }
}

std::vector<Entity*> RuntimeExtractor::GetAffectedBy(DesignEntity de) {
  if (de == DesignEntity::kStmt || de == DesignEntity::kAssign) {
    return affects_extractor_.GetAllAffectedBy();
  } else {
    return std::vector<Entity*>{};
  }
}
std::vector<Entity*> RuntimeExtractor::GetAffectsT(DesignEntity de) {
  if (de == DesignEntity::kStmt || de == DesignEntity::kAssign) {
    return affects_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kForward);
  } else {
    return std::vector<Entity*>();
  }
}
std::vector<Entity*> RuntimeExtractor::GetAffectedByT(DesignEntity de) {
  if (de == DesignEntity::kStmt || de == DesignEntity::kAssign) {
    return affects_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kReverse);
  } else {
    return std::vector<Entity*>();
  }
}

std::vector<Entity*> RuntimeExtractor::GetNextBip(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_extractor_.GetFirstEntityOfRelationship(RelDirection::kForward);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBip(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_extractor_.GetFirstEntityOfRelationship(RelDirection::kReverse);
}

std::vector<Entity*> RuntimeExtractor::GetNextBipT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kForward);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBipT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_t_extractor_.GetFirstEntityOfRelationship(RelDirection::kReverse);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextT(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_t_extractor_.GetRelationshipByTypes(RelDirection::kForward);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevT(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_t_extractor_.GetRelationshipByTypes(RelDirection::kReverse);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffects(DesignEntity first, DesignEntity second) {
  return affects_extractor_.GetAllPair();
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedBy(DesignEntity first, DesignEntity second) {
  return affects_extractor_.GetAllPair();
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectsT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedByT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}

bool RuntimeExtractor::HasAffects() {
  return affects_extractor_.HasAffects();
}

bool RuntimeExtractor::HasAffectedBy() {
  return affects_extractor_.HasAffects(); // Can be replace with (_,_)
}

bool RuntimeExtractor::HasAffectsT() {
  return affects_t_extractor_.HasRelationship(RelDirection::kForward);
}

bool RuntimeExtractor::HasAffectedByT() {
  return affects_t_extractor_.HasRelationship(RelDirection::kReverse);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextBip(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_extractor_.GetRelationshipByTypes(RelDirection::kForward);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevBip(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_extractor_.GetRelationshipByTypes(RelDirection::kReverse);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextBipT(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_t_extractor_.GetRelationshipByTypes(RelDirection::kForward);
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevBipT(DesignEntity first, DesignEntity second) {
  bool valid_first = first == DesignEntity::kStmt || first == DesignEntity::kProgLine;
  bool valid_second = second == DesignEntity::kStmt || second == DesignEntity::kProgLine;
  if (!valid_first && !valid_second) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_t_extractor_.GetRelationshipByTypes(RelDirection::kReverse);
}

bool RuntimeExtractor::HasNextBip() {
  return next_bip_extractor_.HasRelationship(RelDirection::kForward);
}

bool RuntimeExtractor::HasAffects(int target) {
  return affects_extractor_.HasAffects(target);
}

bool RuntimeExtractor::HasAffectedBy(int target) {
  return affects_extractor_.HasAffectedBy(target);
}

bool RuntimeExtractor::HasAffectsT(int target) {
  return affects_t_extractor_.HasRelationship(RelDirection::kForward, target);
}

bool RuntimeExtractor::HasAffectedByT(int target) {
  return affects_t_extractor_.HasRelationship(RelDirection::kReverse, target);
}

bool RuntimeExtractor::HasNextBip(int first) {
  return next_bip_extractor_.HasRelationship(RelDirection::kForward, first);
}

bool RuntimeExtractor::HasPrevBip(int first) {
  return next_bip_extractor_.HasRelationship(RelDirection::kReverse, first);
}

bool RuntimeExtractor::HasNextT(int first, int second) {
  return next_t_extractor_.HasRelationship(RelDirection::kForward, first, second);
}

bool RuntimeExtractor::HasAffects(int first, int second) {
  return affects_extractor_.HasAffects(first, second);
}

bool RuntimeExtractor::HasAffectedBy(int first, int second) {
  return affects_extractor_.HasAffectedBy(first, second);
}

bool RuntimeExtractor::HasAffectsT(int first, int second) {
  return affects_t_extractor_.HasRelationship(RelDirection::kForward, first, second);
}
bool RuntimeExtractor::HasAffectedByT(int first, int second) {
  return affects_t_extractor_.HasRelationship(RelDirection::kReverse, first, second);
}

bool RuntimeExtractor::HasNextBip(int first, int second) {
  return next_bip_extractor_.HasRelationship(RelDirection::kForward, first, second);
}

bool RuntimeExtractor::HasNextBipT(int first, int second) {
  return next_bip_t_extractor_.HasRelationship(RelDirection::kForward, first, second);
}

void RuntimeExtractor::Delete() {
  next_t_extractor_.Delete();
  affects_extractor_.Delete();
  affects_t_extractor_.Delete();
}

bool RuntimeExtractor::IsRuntimeRelationship(PKBRelRefs ref) {
  return runtime_relationships.count(ref);
}