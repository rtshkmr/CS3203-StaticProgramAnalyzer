//
// Created by Max Ng on 11/9/21.
//

#include "QueryPKB.h"

std::list<std::tuple<DesignEntity, std::string>> queryPKBSuchThat(PKB database, RelRef r, std::string stmt, bool isFirstParam) {
  switch(r) {
    case RelRef::kFollows:
      return isFirstParam ? database.GetFollows(stmt) : database.GetPrevious(stmt);
      // CHANGE FOLLOWS TO PREVIOUS
    case RelRef::kParent:
      return isFirstParam ? database.GetParent(stmt) : database.GetChild(stmt);
      // CHANGE PARENT TO CHILD
    default:
      std::list<std::tuple<DesignEntity, std::string>> list;
      return list;
  }
}