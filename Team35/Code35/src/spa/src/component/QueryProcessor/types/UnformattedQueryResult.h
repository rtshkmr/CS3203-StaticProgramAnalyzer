#ifndef AUTOTESTER_UNFORMATTEDQUERYRESULT_H
#define AUTOTESTER_UNFORMATTEDQUERYRESULT_H

#include <vector>
#include "QueryEvaluatorTable.h"

class UnformattedQueryResult {
  private:
    bool passed_all_boolean_group;
    std::vector<QueryEvaluatorTable *> table_list;

  public:
    UnformattedQueryResult(bool boolean_result);
    void SetBooleanResult(bool result);
    bool GetBooleanResult();
    std::vector<QueryEvaluatorTable *> GetTables();
    void AddTable(QueryEvaluatorTable *);
};

#endif //AUTOTESTER_UNFORMATTEDQUERYRESULT_H
