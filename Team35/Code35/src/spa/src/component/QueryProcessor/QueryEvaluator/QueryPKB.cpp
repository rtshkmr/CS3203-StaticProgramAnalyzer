//
// Created by Max Ng on 11/9/21.
//

#include "QueryPKB.h"

bool QueryPkbForRelationshipExistence(PKB database, RelRef r) {
  switch(r) {
    case RelRef::kFollows:
      return database.hasFollows();
    case RelRef::kParent:
      return database.hasParent();
    case RelRef::kUsesS:
      return database.hasUses();
    case RelRef::kModifiesS:
      return database.hasModifies();
    default:
      return false;
  }
}

std::list<std::tuple<DesignEntity, std::string>> QueryPKBSuchThat(PKB database, RelRef r, std::string stmt, bool is_first_param) {
  switch(r) {
    case RelRef::kFollows:
      return is_first_param ? database.GetFollows(stmt) : database.GetPrevious(stmt);
    case RelRef::kParent:
      return is_first_param ? database.GetParent(stmt) : database.GetChild(stmt);
    case RelRef::kUsesS:
      return is_first_param ? database.GetUsedBy(stmt) : database.GetUses(stmt);
    case RelRef::kModifiesS:
      return is_first_param ? database.GetModifiedBy(stmt) : database.GetModifies(stmt);
    default:
      std::list<std::tuple<DesignEntity, std::string>> list;
      return list;
  }
}

std::vector<AssignEntity> QueryPkbPattern(PKB database, bool is_assign_synonym, std::string value) {
  std::vector<AssignEntity> list_of_assign;
  return list_of_assign;
  // return is_assign_synonym ? database.GetVarModified(value) : database.GetStmtModifying(value);
}
