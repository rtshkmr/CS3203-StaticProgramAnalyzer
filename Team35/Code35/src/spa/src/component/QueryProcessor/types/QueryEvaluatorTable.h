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
    bool AddColumn(std::string synonym);

    bool AddRowForAllColumn(std::string synonym, int index, std::string value);

    // Add target synonym column with values to table
    bool AddTargetSynonym(std::list<std::string> synonymList);

    // Delete row
    bool DeleteRow(int index);

    // Add row (and new col)
    bool AddRow(std::string synonym, int index, std::string value);

    // Return vector of target synonym
    std::vector<std::string> GetResults();

    // Return vector of specified synonym
    std::vector<std::string> GetColumn(std::string synonym);

    // Empty column but keep the synonym (header) in the table
    bool RemoveColumn(std::string synonym);

    bool ContainsColumn(std::string synonym);

    int GetSize();
  private:
    std::unordered_map<std::string, std::vector<std::string>> um;
    std::string target_synonym;
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H
