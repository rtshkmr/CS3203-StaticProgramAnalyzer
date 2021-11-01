#include <component/PKB/PKB.h>
#include "TestUtil.h"

bool test_util::AreEntityListsEqual(const std::list<std::tuple<EntityEnum, std::string>>& l1, std::vector<Entity*> l2) {
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
