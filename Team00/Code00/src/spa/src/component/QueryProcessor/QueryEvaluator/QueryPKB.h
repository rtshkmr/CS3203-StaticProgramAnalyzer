//
// Created by Max Ng on 11/9/21.
//

#ifndef AUTOTESTER_QUERYPKB_H
#define AUTOTESTER_QUERYPKB_H

#include <list>
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"

std::list<std::tuple<DesignEntity, std::string>> queryPKBSuchThat(PKB pkb, RelRef r, std::string stmt, bool isFirstParam);

#endif //AUTOTESTER_QUERYPKB_H
