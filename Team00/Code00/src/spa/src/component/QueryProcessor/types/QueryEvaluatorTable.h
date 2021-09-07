//
// Created by Max Ng on 6/9/21.
//

#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <list>

class QueryEvaluatorTable {
  public:
    QueryEvaluatorTable(std::string target);

    // Add column to table
    bool addColumn(std::string synonym);

    // Add target synonym column with values to table
    bool addTargetSynonym(std::string synonym, std::list<std::string> synonymList);

    // Delete row
    bool deleteRow(int index);

    // Add row (and new col)
    bool addRow(std::string synonym, int index, std::string value);

    // Return vector of target synonym
    std::vector<std::string> getResults();

    // Empty column
    bool removeColumn(std::string synonym);

    int getSize();
  private:
    std::unordered_map<std::string, std::vector<std::string>> um;
    std::string targetSynonym;
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H
