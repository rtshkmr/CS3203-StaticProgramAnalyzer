//
// Created by Max Ng on 11/9/21.
//

#include "QueryPKB.h"

std::list<std::tuple<DesignEntity, std::string>> QueryPKBSuchThat(PKB database, RelRef r, std::string stmt, bool is_first_param) {
  switch(r) {
    case RelRef::kFollows:
      return is_first_param ? database.GetFollows(stmt) : database.GetPrevious(stmt);
      // CHANGE FOLLOWS TO PREVIOUS
    case RelRef::kParent:
      return is_first_param ? database.GetParent(stmt) : database.GetChild(stmt);
      // CHANGE PARENT TO CHILD
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

  // return is_assign_synonym ? database.GetVarModified(value) : database.GetStmtModifying(value);
}
