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
  static bool IsAssignDesignEntity(DesignEntity de);
  template <typename T>
  static std::vector<Entity*> ConvertListToVector(std::list<T*> list);
};

/**
 * Converts any Entity list to Entity vector.
 * @param list of Entity child objects.
 * @return Vector of Entities.
 */
template <typename T>
std::vector<Entity*> Utility::ConvertListToVector(std::list<T*> list) {
  std::vector<Entity*> vector {std::make_move_iterator(std::begin(list)), std::make_move_iterator(std::end(list))};
  return vector;
}

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_UTIL_UTILITY_H_
