/**
 * This file continas useful test objects that anyone can use.
 */

#ifndef AUTOTESTER_TEAM00_CODE00_SRC_UNIT_TESTING_UTILS_TESTUTILS_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_UNIT_TESTING_UTILS_TESTUTILS_H_

#include <string>
#include <vector>
#include <model/Entity.h>

class TestUtils {
 public:

  static bool AreListsEqual(const std::list<Statement*>& as, const std::list<Statement*>& bs);
  static bool AreVectorsEqual(const std::vector<Entity*>& as, const std::vector<Entity*>& bs);
  static bool AreListsEqual(const std::list<Procedure*>& a, const std::list<Procedure*>& b);
  static bool AreVectorsEqual(const std::vector<std::tuple<Entity*, Entity*>>& as, const std::vector<std::tuple<Entity*, Entity*>>& bs);
  static void AddStatementList(Container* container, const std::list<Statement*>& stmt_list);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_UNIT_TESTING_UTILS_TESTUTILS_H_
