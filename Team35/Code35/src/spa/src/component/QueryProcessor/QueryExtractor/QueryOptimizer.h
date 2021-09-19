#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include <string>
#include <list>
#include <vector>
#include <component/QueryProcessor/types/Types.h>

class QueryOptimizer {
  public:
    static void GroupClauses(std::vector<Clause*>* clauses, std::list<Group*>* groups, Synonym* target);
};

#endif //AUTOTESTER_QUERYOPTIMIZER_H