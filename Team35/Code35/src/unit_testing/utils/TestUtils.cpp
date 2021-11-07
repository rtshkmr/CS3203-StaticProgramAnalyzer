#include "TestUtils.h"
#include <vector>
#include <unordered_set>

/**
 * Checks if the contents of 2 lists are equal, which may not be in the same order.
 */
bool TestUtils::AreListsEqual(const std::list<Statement*>& as, const std::list<Statement*>& bs) {
  for (Statement* a: as) {
    bool found_a = false;
    for (Statement* b: bs) {
      found_a = found_a || a == b;
    }
    if (!found_a) {
      return false;
    }
  }
  return as.size() == bs.size();
}

bool TestUtils::AreVectorsEqual(const std::vector<Entity*>& as, const std::vector<Entity*>& bs) {
  for (Entity* a: as) {
    bool found_a = false;
    for (Entity* b: bs) {
      found_a = found_a || a == b;
    }
    if (!found_a) {
      return false;
    }
  }
  return as.size() == bs.size();
}

bool TestUtils::AreListsEqual(const std::list<Procedure*>& a, const std::list<Procedure*>& b) {
  for (Procedure* a_proc: a) {
    bool found_a = false;
    for (Procedure* b_proc: b) {
      found_a = found_a || *a_proc->GetName() == *b_proc->GetName();
    }
    if (!found_a) {
      return false;
    }
  }
  return a.size() == b.size();
}

bool TestUtils::AreVectorsEqual(const std::vector<std::tuple<Entity*, Entity*>>& as, const std::vector<std::tuple<Entity*, Entity*>>& bs) {
  for (const std::tuple<Entity*, Entity*>& a: as) {
    bool found_a = false;
    for (const std::tuple<Entity*, Entity*>& b: bs) {
      found_a = found_a || a == b;
    }
    if (!found_a) {
      return false;
    }
  }
  return as.size() == bs.size();
}

void TestUtils::AddStatementList(Container* container, const std::list<Statement*>& stmt_list) {
  for (Statement* stmt: stmt_list) {
    container->AddStatement(stmt);
  }
}
