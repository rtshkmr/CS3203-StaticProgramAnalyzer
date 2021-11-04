/**
 * This class contains any generic utility methods.
 */

#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_UTIL_UTILITY_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_UTIL_UTILITY_H_

#include <string>
#include <model/Entity.h>
#include <model/Statement.h>
#include <component/PKB/PKB.h>

class Utility {
 public:
  static int ConvertStringToInt(const std::string& input);
  static AssignEntity* GetAssignEntityFromStmtNum(PKB* pkb, int target);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_UTIL_UTILITY_H_
