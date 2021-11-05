#include <util/Utility.h>
#include "AffectsBipExtractor.h"
#include "model/CFG.h"

AffectsBipExtractor::AffectsBipExtractor(RuntimeMediator* rte, PKB* pkb) {
  this->rte_ = rte;
  this->pkb_ = pkb;
}

std::vector<Entity*> AffectsBipExtractor::GetRelationship(RelDirection dir, int target) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->GetRelationship(selector, std::to_string(target));
}

std::vector<Entity*> AffectsBipExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->GetFirstEntityOfRelationship(selector, de);
}

std::vector<std::tuple<Entity*, Entity*>> AffectsBipExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                      DesignEntity first,
                                                                                      DesignEntity second) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->GetRelationshipByTypes(selector, first, second);
}

bool AffectsBipExtractor::HasRelationship(RelDirection dir) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->HasRelationship(selector);
}

bool AffectsBipExtractor::HasRelationship(RelDirection dir, int target) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->HasRelationship(selector, std::to_string(target));
}

bool AffectsBipExtractor::HasRelationship(RelDirection dir, int first, int second) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
  return rte_->HasRelationship(selector, std::to_string(first), std::to_string(second));
}