#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H

#include <vector>
#include <string>
#include <algorithm>
#include "types/UnformattedQueryResult.h"

class QueryProjector {
  public:
    QueryProjector(std::vector<Synonym> target_synonym_list);
    std::vector<std::string> FormatQuery(UnformattedQueryResult unformatted_results);

    // made public purely for testing
    static std::vector<std::vector<std::string>> StringifyTable(std::vector<Synonym*> synonyms,
                                                         std::vector<std::vector<Entity*>> entity_table);
    static std::vector<std::vector<std::string>> CrossProductTables(std::vector<std::vector<std::string>> t1,
                                                             std::vector<std::vector<std::string>> t2);
    static std::vector<std::vector<std::string>> ReorderTable(std::vector<Synonym> desired_order,
                                                       std::list<Synonym*> current_order,
                                                       std::vector<std::vector<std::string>> table);
    static std::vector<std::string> JoinTuples(std::vector<std::vector<std::string>> table);
  private:
    std::vector<Synonym> target_synonym_list;
    static std::vector<std::string> FormatMultipleTables(std::vector<std::vector<std::vector<std::string>>> tables,
                                                         std::list<Synonym*> table_synonym_order,
                                                         std::vector<Synonym> target_synonym_list);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
