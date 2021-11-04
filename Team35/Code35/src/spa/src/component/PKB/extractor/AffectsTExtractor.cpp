#include <cassert>
#include "../../../model/CFG.h"
#include "AffectsTExtractor.h"

AffectsTExtractor::AffectsTExtractor(RuntimeMediator* rte, PKB* pkb) {
  this->rte_ = rte;
  this->pkb_ = pkb;
}

bool AffectsTExtractor::HasRelationship(RelDirection dir, int first, int second) {
  if (!isCached) InitCache();

  if (dir == RelDirection::kReverse) {
    int tmp = first;
    first = second;
    second = tmp;
  }

  std::set<int> checked = {};
  std::list<int> toCheck = {first};
  while (toCheck.size() > 0) {
    int check = toCheck.front();
    toCheck.pop_front();

    //before size
    size_t before = checked.size();
    checked.insert(check);
    if (checked.size() == before) {
      continue; //already checked before.
    }

    auto itr = affects_t_map_.find(check);
    if (itr == affects_t_map_.end()) {
      continue; //no value found.
    }

    std::list<int>* values = itr->second;
    if (std::find(values->begin(), values->end(), second) != values->end()) {
      return true; // found
    } else {
      // append list to set.
      for (int val : *values) {
        toCheck.push_back(val);
      }
    }
  }
  return false;
}

std::vector<Entity*> AffectsTExtractor::GetAffectsT(int target) {
  return std::vector<Entity*>();
}

std::vector<Entity*> AffectsTExtractor::GetAffectedByT(int target) {
  return std::vector<Entity*>();
}

void AffectsTExtractor::Delete() {

}

void AffectsTExtractor::InitCache() {
  if (isCached) return;

  std::vector<std::tuple<Entity*, Entity*>> all_pairs = rte_->GetRelationshipByTypes(PKBRelRefs::kAffects, DesignEntity::kAssign, DesignEntity::kAssign);
  for (auto pair : all_pairs) {
    AssignEntity* ae1 = dynamic_cast<AssignEntity*>(std::get<0>(pair));
    AssignEntity* ae2 = dynamic_cast<AssignEntity*>(std::get<1>(pair));
    assert(ae1 && ae2);

    AddRelationshipToMap(&affects_t_map_, ae1->GetStatementNumber()->GetNum(), ae2->GetStatementNumber()->GetNum());
    AddRelationshipToMap(&affected_by_t_map_, ae2->GetStatementNumber()->GetNum(), ae1->GetStatementNumber()->GetNum());
  }
  isCached = true;
}

//TODO: Reference from Deliverables?
template <typename X, typename Y>
void AffectsTExtractor::AddRelationshipToMap(std::unordered_map<X, std::list<Y>*>* map, X key, Y value) {
  if (map->count(key)) {
    std::list<Y>* values = map->find(key)->second;
    if (std::find(values->begin(), values->end(), value) == values->end()) {
      values->push_back(value);
    }
  } else {
    auto* list = new std::list<Y>();
    list->push_back(value);
    map->insert(std::make_pair(key, list));
  }
}