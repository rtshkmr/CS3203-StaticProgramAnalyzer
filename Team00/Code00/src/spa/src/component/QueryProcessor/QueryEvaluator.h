#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H

#include <list>
#include "types/Types.h"
#include <string>
#include <vector>

class QueryEvaluator {
    public:
        std::vector<std::string> EvaluateQuery(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATOR_H
