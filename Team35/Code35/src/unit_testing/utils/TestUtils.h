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
  std::vector<std::string> GetBasicSourceLines();

  static bool AreListsEqual(std::list<Statement*> as, std::list<Statement*> bs);
  static bool AreVectorsEqual(std::vector<Entity*> as, std::vector<Entity*> bs);
  static bool AreListsEqual(std::list<Procedure*> a, std::list<Procedure*> b);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_UNIT_TESTING_UTILS_TESTUTILS_H_
