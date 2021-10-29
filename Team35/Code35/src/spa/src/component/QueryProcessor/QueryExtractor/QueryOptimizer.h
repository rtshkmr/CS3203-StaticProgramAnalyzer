#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include <vector>
#include <component/QueryProcessor/types/Types.h>

class QueryOptimizer {
  public:
    static void ReorderGroups(std::vector<Group*>* groups);
};


#endif //AUTOTESTER_QUERYOPTIMIZER_H
