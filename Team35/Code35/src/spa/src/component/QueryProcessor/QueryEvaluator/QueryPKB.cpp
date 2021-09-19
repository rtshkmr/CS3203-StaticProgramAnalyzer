//
// Created by Max Ng on 11/9/21.
//

#include "QueryPKB.h"

bool QueryPkbForRelationshipExistence(PKB database, RelRef r) {
  switch(r) {
    case RelRef::kFollows:
      return database.HasFollows();
    case RelRef::kParent:
      return database.HasParent();
    case RelRef::kUsesS:
      return database.HasUses();
    case RelRef::kModifiesS:
      return database.HasModifies();
    default:
      return false;
  }
}

std::list<std::tuple<DesignEntity, std::string>> QueryPKBSuchThat(PKB database, RelRef r, const std::string& stmt, bool is_first_param) {
  switch(r) {
    case RelRef::kFollows:
      return is_first_param ? database.GetFollows(stmt) : database.GetPrevious(stmt);
    case RelRef::kParent:
      return is_first_param ? database.GetParent(stmt) : database.GetChild(stmt);
    case RelRef::kUsesS:
      return is_first_param ? database.GetUses(stmt) : database.GetUsedBy(stmt);
    case RelRef::kModifiesS:
      return is_first_param ? database.GetModifies(stmt) : database.GetModifiedBy(stmt);
    case RelRef::kFollowsT:
      return is_first_param ? database.GetFollowsT(stmt) : database.GetPreviousT(stmt);
    case RelRef::kParentT:
      return is_first_param ? database.GetParentT(stmt) : database.GetChildT(stmt);
    default:
      std::list<std::tuple<DesignEntity, std::string>> list;
      return list;
  }
}

std::vector<AssignEntity> QueryPkbPattern(PKB database, bool is_assign_synonym, const std::string& value) {
  return is_assign_synonym ? database.GetPatternByAssign(value) : database.GetPatternByVariable(value);
}
