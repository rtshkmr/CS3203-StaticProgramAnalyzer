#include "AffectsBipTExtractor.h"
#include "model/CFG.h"

AffectsBipTExtractor::AffectsBipTExtractor(RuntimeMediator* rte, PKB* pkb) {
  this->rte_ = rte;
  this->pkb_ = pkb;
}

std::vector<Entity*> AffectsBipTExtractor::GetRelationship(RelDirection dir, int target) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->GetRelationship(selector, std::to_string(target));
}

std::vector<Entity*> AffectsBipTExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->GetFirstEntityOfRelationship(selector, de);
}

std::vector<std::tuple<Entity*, Entity*>> AffectsBipTExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                      DesignEntity first,
                                                                                      DesignEntity second) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->GetRelationshipByTypes(selector, first, second);
}

bool AffectsBipTExtractor::HasRelationship(RelDirection dir) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->HasRelationship(selector);
}

bool AffectsBipTExtractor::HasRelationship(RelDirection dir, int target) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->HasRelationship(selector, std::to_string(target));
}

bool AffectsBipTExtractor::HasRelationship(RelDirection dir, int first, int second) {
  PKBRelRefs selector = (dir == RelDirection::kForward) ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
  return rte_->HasRelationship(selector, std::to_string(first), std::to_string(second));
}