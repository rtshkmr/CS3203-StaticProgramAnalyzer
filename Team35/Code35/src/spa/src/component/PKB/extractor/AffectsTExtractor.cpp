#include <cassert>
#include <util/Utility.h>
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

std::vector<Entity*> AffectsTExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  if (!isCached) InitCache();
  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;

  std::set<int> return_set = {};
  for (auto pair : *mapToCheck) {
    return_set.insert(pair.first);
  }

  return ConvertIntToEntity(return_set);
}

std::vector<std::tuple<Entity*, Entity*>> AffectsTExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                    DesignEntity first,
                                                                                    DesignEntity second) {
  if (!isCached) InitCache();
  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;

  std::vector<std::tuple<Entity*, Entity*>> intermediate_table = {};
  for (auto pair : *mapToCheck) {
    std::vector<Entity*> this_key_entity = GetRelationship(RelDirection::kForward, pair.first);
    AssignEntity* ae = Utility::GetAssignEntityFromStmtNum(pkb_, pair.first);

    for (auto val: this_key_entity) {
      intermediate_table.push_back(std::make_tuple(ae, val));
    }
  }

  return intermediate_table;
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

  std::unordered_map<int, std::list<int>*>* mapToCheck = (dir == RelDirection::kForward) ? &affects_t_map_ : &affected_by_t_map_;

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

    auto itr = mapToCheck->find(check);
    if (itr == mapToCheck->end()) {
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

void AffectsTExtractor::Delete() {
  isCached = false;
  affects_t_map_ = {};
  affected_by_t_map_ = {};
}

void AffectsTExtractor::InitCache() {
  if (isCached) return;

  std::vector<std::tuple<Entity*, Entity*>> all_pairs = rte_->GetRelationshipByTypes(PKBRelRefs::kAffects, DesignEntity::kAssign, DesignEntity::kAssign);
  for (auto pair : all_pairs) {
    AssignEntity* ae1 = dynamic_cast<AssignEntity*>(std::get<0>(pair));
    AssignEntity* ae2 = dynamic_cast<AssignEntity*>(std::get<1>(pair));
    assert(ae1 && ae2);

    Deliverable::AddRelationshipToMap(&affects_t_map_,
                                      ae1->GetStatementNumber(), ae2->GetStatementNumber());
    Deliverable::AddRelationshipToMap(&affected_by_t_map_, ae2->GetStatementNumber(),
                                      ae1->GetStatementNumber());
  }
  isCached = true;
}

std::vector<Entity*> AffectsTExtractor::ConvertIntToEntity(std::set<int> set_to_convert) {
  std::vector<Entity*> retList = {};
  for (auto item : set_to_convert) {
    retList.push_back(Utility::GetAssignEntityFromStmtNum(pkb_, item));
  }
  return retList;
}