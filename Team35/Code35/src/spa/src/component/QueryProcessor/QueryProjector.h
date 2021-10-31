#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H

#include <vector>
#include <string>
#include <algorithm>
#include "types/UnformattedQueryResult.h"

class QueryProjector {
  public:
    QueryProjector(std::vector<std::pair<Synonym*, Attribute>> target_syn_attr_list);
    std::vector<std::string> FormatQuery(UnformattedQueryResult unformatted_results);

    // made public purely for testing
    static std::vector<std::vector<std::string>> StringifyTable(std::vector<Synonym*> synonyms,
                                                         std::vector<std::vector<Entity*>> entity_table);
    static std::vector<std::vector<std::string>> CrossProductTables(std::vector<std::vector<std::string>> t1,
                                                             std::vector<std::vector<std::string>> t2);
    static std::vector<std::vector<std::string>> ReorderTable(std::vector<std::pair<Synonym*, Attribute>> desired_order,
                                                              std::vector<Synonym*> current_order,
                                                              std::vector<std::vector<std::string>> table);
    static std::vector<std::string> JoinTuples(std::vector<std::vector<std::string>> table);
  private:
    std::vector<std::pair<Synonym*, Attribute>> target_syn_attr_list;
    static std::vector<std::string> FormatMultipleTables(std::vector<std::vector<std::vector<std::string>>> tables,
                                                         std::vector<Synonym*> table_synonym_order,
                                                         std::vector<std::pair<Synonym*, Attribute>> tgt_syn_attrs);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
