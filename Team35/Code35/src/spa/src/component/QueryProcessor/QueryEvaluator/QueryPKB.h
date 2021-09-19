//
// Created by Max Ng on 11/9/21.
//

#ifndef AUTOTESTER_QUERYPKB_H
#define AUTOTESTER_QUERYPKB_H

#include <list>
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include "model/Statement.h"

bool QueryPkbForRelationshipExistence(PKB database, RelRef r);
std::list<std::tuple<DesignEntity, std::string>> QueryPKBSuchThat(PKB pkb, RelRef r, const std::string& stmt, bool is_first_param);
std::vector<AssignEntity> QueryPkbPattern(PKB database, bool is_assign_synonym, const std::string& value);

#endif //AUTOTESTER_QUERYPKB_H
