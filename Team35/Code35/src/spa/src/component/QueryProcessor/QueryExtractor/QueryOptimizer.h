#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include <vector>
#include <component/QueryProcessor/types/Types.h>

class QueryOptimizer {
  private:
    bool are_optimizations_enabled;
  public:
    QueryOptimizer(bool are_optimizations_enabled) : are_optimizations_enabled(are_optimizations_enabled) {};
    void ReorderGroups(std::vector<Group*>* groups);
};


#endif //AUTOTESTER_QUERYOPTIMIZER_H
