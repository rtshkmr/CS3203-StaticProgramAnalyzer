#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <list>
#include "Types.h"

class QueryEvaluatorTable {
  public:
    QueryEvaluatorTable(std::string target);

    // Add column to table
    bool AddColumn(std::string synonym);

    bool AddRowForAllColumn(std::string synonym, int index, std::string value);

    bool AddMultipleRowForAllColumn(std::string synonym, int index, std::string value, int count);

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

    int GetRowSize();

    std::string GetStatementSynonym(std::unordered_map<std::string, DesignEntity> synonym_design_entity_map);
  private:
    std::unordered_map<std::string, std::vector<std::string>> um;
    std::string target_synonym;
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEVALUATORTABLE_H
