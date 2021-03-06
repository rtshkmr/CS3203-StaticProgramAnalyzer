/**
 * This class contains any generic utility methods implementation.
 */

#include <cassert>
#include <exception/SpaException.h>
#include "Utility.h"

/**
 * Perfectly converts String to Integer using stoi without partial conversion given by stoi.
 * @param input The String to be converted.
 * @return The integer value after being converted.
 * @throws SyntaxException when a non-integer in passed in or when integers that had exceeded the range.
 */
int Utility::ConvertStringToInt(const std::string &input) {
  size_t num_chars = 0;
  int value;
  try {
    value = stoi(input, &num_chars);
  } catch (std::exception ia) {
    throw SyntaxException("Argument is not smaller that max int.");
  }
  if (num_chars != input.size()) {
    throw SyntaxException("Constant is not valid. Numbers mixed with letters.");
  }
  return value;
}

/**
 * Using the StmtRef, retrieve AssignEntity pointer from PKB.
 * @param pkb The pkb to check from.
 * @param target The StmtRef to be checked.
 * @return The AssignEntity pointer if found; nullptr otherwise.
 */
AssignEntity* Utility::GetAssignEntityFromStmtNum(PKB* pkb, int target) {
  std::vector<Entity*> ae_vec_target =
      pkb->GetPatternEntities(DesignEntity::kAssign, std::to_string(target));

  if (ae_vec_target.empty()) //target given is not an assign entity.
    return nullptr;

  assert (ae_vec_target.size() == 1); // must be 1
  return dynamic_cast<AssignEntity*>(ae_vec_target[0]);
}

/**
 * Checks if the input DesignEntity enum can be a Assign-type.
 * @param de DesignEntity to be checked.
 * @return true if it is; false otherwise.
 */
bool Utility::IsAssignDesignEntity(DesignEntity de) {
  return de == DesignEntity::kAssign || de == DesignEntity::kStmt || de == DesignEntity::kProgLine;
}

/**
 * Converts hashmap of string mapping into string mapping.
 * @param map Hashmap mapping string to vector of Entities.
 * @return Mapping of Entity to list of Entities.
 */
std::unordered_map<Entity*, std::list<Entity*>*>* Utility::ConvertStringToEntityMapping(
    std::vector<Entity*> stmt_list,
    const std::unordered_map<std::string, std::vector<Entity*>> &map) {
  auto* entity_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  for (auto[key, value] : map) {
    Entity* first_arg = stmt_list[Utility::ConvertStringToInt(key) - 1];
    auto* second_arg = new std::list<Entity*>{};
    second_arg->insert(second_arg->end(), value.begin(), value.end());
    entity_map->insert({first_arg, second_arg});
  }
  return entity_map;
}
