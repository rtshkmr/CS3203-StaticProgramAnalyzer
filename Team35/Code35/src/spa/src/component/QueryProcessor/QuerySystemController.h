//
// Created by Hemanshu Gandhi on 11/9/21.
//

#ifndef AUTOTESTER_QUERYSYSTEMCONTROLLER_H
#define AUTOTESTER_QUERYSYSTEMCONTROLLER_H

#include <string>
#include <optional>
#include <list>
#include <component/QueryProcessor/types/Types.h>
#include <component/PKB/PKB.h>

class QuerySystemController {
  public:
    static std::string Evaluate(std::string *query, PKB* pkb);
};

#endif //AUTOTESTER_QUERYSYSTEMCONTROLLER_H
