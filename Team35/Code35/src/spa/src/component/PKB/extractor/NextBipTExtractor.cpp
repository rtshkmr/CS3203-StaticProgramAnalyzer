#include <component/SourceProcessor/Extractors/TransitiveExtractor.h>

#include <utility>
#include "NextBipTExtractor.h"

NextBipTExtractor::NextBipTExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> NextBipTExtractor::GetRelationship(PKBRelRefs ref, const std::string &target) {
  PopulateRelationships();
  return pkb_->GetRelationship(ref, target);
}

std::vector<Entity*> NextBipTExtractor::GetFirstEntityOfRelationship(PKBRelRefs ref) {
  PopulateRelationships();
  return pkb_->GetFirstEntityOfRelationship(ref, DesignEntity::kStmt);
}

std::vector<std::tuple<Entity*, Entity*>> NextBipTExtractor::GetRelationshipByTypes(PKBRelRefs ref) {
  PopulateRelationships();
  return pkb_->GetRelationshipByTypes(ref, DesignEntity::kStmt, DesignEntity::kStmt);
}

bool NextBipTExtractor::HasRelationship(PKBRelRefs ref) {
  PopulateRelationships();
  return pkb_->HasRelationship(ref);
}

bool NextBipTExtractor::HasRelationship(PKBRelRefs ref, std::string target) {
  PopulateRelationships();
  return pkb_->HasRelationship(ref, std::move(target));
}

bool NextBipTExtractor::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  PopulateRelationships();
  return pkb_->HasRelationship(ref, std::move(first), std::move(second));
}

void NextBipTExtractor::PopulateRelationships() {
  if (pkb_->HasRelationship(PKBRelRefs::kNextBipT)) return;
  TransitiveExtractor extractor = TransitiveExtractor<Entity>();
  auto* t_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  auto* reverse_t_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  // if (!pkb_->HasRelationship(PKBRelRefs::kNextBip))
  // rte hasrel for nextbip
  auto non_t_string_map = pkb_->GetRelationshipMap(PKBRelRefs::kNextBip);
  auto* non_t_map = pkb_->ConvertStringToEntityMapping(non_t_string_map);
  extractor.Extract(t_map, reverse_t_map, non_t_map);
  pkb_->PopulateRelationship(t_map, PKBRelRefs::kNextBipT);
  pkb_->PopulateRelationship(reverse_t_map, PKBRelRefs::kPrevBipT);
}
