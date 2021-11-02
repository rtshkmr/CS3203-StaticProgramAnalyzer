#include <component/SourceProcessor/Extractors/TransitiveExtractor.h>

#include <utility>
#include "NextBipTExtractor.h"

NextBipTExtractor::NextBipTExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> NextBipTExtractor::GetRelationship(RelDirection dir, int target) {
  PopulateRelationships();
  return pkb_->GetRelationship(GetPKBRelRef(dir), std::to_string(target));
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

bool NextBipTExtractor::HasRelationship(RelDirection dir, int target) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(target));
}

bool NextBipTExtractor::HasRelationship(RelDirection dir, int first, int second) {
  PopulateRelationships();
  return pkb_->HasRelationship(GetPKBRelRef(dir), std::to_string(first), std::to_string(second));
}

void NextBipTExtractor::PopulateRelationships() {
  if (pkb_->HasRelationship(PKBRelRefs::kNextBipT)) return;
  auto* t_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  auto* reverse_t_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  if (!pkb_->HasRelationship(PKBRelRefs::kNextBip)) {
     rtm_->HasRelationship(PKBRelRefs::kNextBip);
  }
  auto non_t_string_map = pkb_->GetRelationshipMap(PKBRelRefs::kNextBip);
  auto* non_t_map = pkb_->ConvertStringToEntityMapping(non_t_string_map);
  TransitiveExtractor extractor = TransitiveExtractor<Entity>();
  // dfs doesnt work on cyclic graphs doofus

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
