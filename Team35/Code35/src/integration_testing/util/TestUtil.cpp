#include <component/PKB/PKB.h>
#include "TestUtil.h"

bool test_util::AreEntityListsEqual(const std::list<std::tuple<EntityEnum, std::string>> &l1, const std::vector<Entity*>& l2) {
  for (Entity* a: l2) {
    bool found_a = false;
    for (std::tuple<EntityEnum, std::string> b: l1) {
      bool match = a->GetEntityEnum() == std::get<0>(b) && PKB::GetNameFromEntity(a) == std::get<1>(b);
      found_a = found_a || match;
    }
    if (!found_a) {
      return false;
    }
  }
  return l2.size() == l1.size();
}

bool test_util::AreAllPairsEqual(const std::list<std::tuple<std::tuple<EntityEnum, std::string>,
                                                      std::tuple<EntityEnum, std::string>>>& l1,
                                 const std::vector<std::tuple<Entity*, Entity*>>& l2) {
  for (std::tuple<Entity*, Entity*> a: l2) {
    bool found_a = false;
    Entity* first_entity = std::get<0>(a);
    Entity* second_entity = std::get<1>(a);
    for (auto b: l1) {
      std::tuple<EntityEnum, std::string> first_pair = std::get<0>(b);
      std::tuple<EntityEnum, std::string> second_pair = std::get<1>(b);
      bool first_match = first_entity->GetEntityEnum() == std::get<0>(first_pair)
          && PKB::GetNameFromEntity(first_entity) == std::get<1>(first_pair);
      bool second_match = second_entity->GetEntityEnum() == std::get<0>(second_pair)
          && PKB::GetNameFromEntity(second_entity) == std::get<1>(second_pair);
      found_a = found_a || (first_match && second_match);
    }
    if (!found_a) {
      return false;
    }
  }
  return l2.size() == l1.size();
}