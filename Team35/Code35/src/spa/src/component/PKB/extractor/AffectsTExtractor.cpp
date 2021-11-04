#include <cassert>
#include "../../../model/CFG.h"
#include "AffectsTExtractor.h"

AffectsTExtractor::AffectsTExtractor(RuntimeMediator* rte, PKB* pkb) {
  this->rte_ = rte;
  this->pkb_ = pkb;
}

std::vector<Entity*> AffectsTExtractor::GetRelationship(RelDirection dir, int target) {
  if (!isCached) InitCache();

  std::set<int> return_set = {};
  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;

  std::list<int> toCheck = {target};
  while (toCheck.size() > 0) {
    int check = toCheck.front();
    toCheck.pop_front();

    //before size
    size_t before = return_set.size();
    return_set.insert(check);
    if (return_set.size() == before) {
      continue; //already checked before.
    }

    auto itr = mapToCheck->find(check);
    if (itr == mapToCheck->end()) {
      continue; //no value found.
    }

    std::list<int>* values = itr->second;
    for (int val : *values) {
      toCheck.push_back(val);
    }
  }

  //check if HasAffects(self,self) exist, if not, delete
  if (!HasRelationship(RelDirection::kForward, target, target)) {
    return_set.erase(target);
  }

  return ConvertIntToEntity(return_set);
}

std::vector<Entity*> AffectsTExtractor::GetFirstEntityOfRelationship(RelDirection dir) {
  if (!isCached) InitCache();
  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;

  std::set<int> return_set = {};
  for (auto pair : *mapToCheck) {
    return_set.insert(pair.first);
  }

  return ConvertIntToEntity(return_set);
}

bool AffectsTExtractor::HasRelationship(RelDirection dir) {
  if (!isCached) InitCache();
  return affects_t_map_.size() > 0;
}

bool AffectsTExtractor::HasRelationship(RelDirection dir, int target) {
  if (!isCached) InitCache();

  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;
  auto itr = mapToCheck->find(target);
  return itr != mapToCheck->end();

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

std::vector<Entity*> AffectsTExtractor::ConvertIntToEntity(std::set<int> set_to_convert) {
  std::vector<Entity*> retList = {};
  for (auto item : set_to_convert) {
    retList.push_back(GetAssignEntityFromStmtNum(item));
  }
  return retList;
}

//TODO: Reference from AffectsExtractor?
AssignEntity* AffectsTExtractor::GetAssignEntityFromStmtNum(int target) {
  std::vector<Entity*> ae_vec_target =
      pkb_->GetPatternEntities(DesignEntity::kAssign, std::to_string(target));

  if (ae_vec_target.size() == 0) //target given is not an assign entity.
    return nullptr;

  assert (ae_vec_target.size() == 1); // must be 1
  return dynamic_cast<AssignEntity*>(ae_vec_target[0]);
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