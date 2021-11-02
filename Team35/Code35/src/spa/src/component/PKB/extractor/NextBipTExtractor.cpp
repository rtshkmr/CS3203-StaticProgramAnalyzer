#include <component/SourceProcessor/Extractors/TransitiveExtractor.h>

#include <utility>
#include "NextBipTExtractor.h"

NextBipTExtractor::NextBipTExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> NextBipTExtractor::GetRelationship(RelDirection dir, std::string target) {
  PopulateRelationships();
  return pkb_->GetRelationship(GetPKBRelRef(dir), target);
}

std::vector<Entity*> NextBipTExtractor::GetFirstEntityOfRelationship(RelDirection dir) {
  PopulateRelationships();
  return pkb_->GetFirstEntityOfRelationship(GetPKBRelRef(dir), DesignEntity::kStmt);
}

std::vector<std::tuple<Entity*, Entity*>> NextBipTExtractor::GetRelationshipByTypes(RelDirection dir) {
  PopulateRelationships();
  return pkb_->GetRelationshipByTypes(GetPKBRelRef(dir), DesignEntity::kStmt, DesignEntity::kStmt);
}

bool NextBipTExtractor::HasRelationship(RelDirection dir) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, std::string target) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::move(target));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, std::string first, std::string second) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::move(first), std::move(second));
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

PKBRelRefs NextBipTExtractor::GetPKBRelRef(RelDirection dir) {
  return dir == RelDirection::kForward ? PKBRelRefs::kNextBipT : PKBRelRefs::kPrevBipT;
}

void NextBipTExtractor::SetMediator(RuntimeMediator* rtm) {
  rtm_ = rtm;
}
