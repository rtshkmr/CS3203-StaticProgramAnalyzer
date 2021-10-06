#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H

#include <vector>
#include <string>
#include <algorithm>
#include "types/UnformattedQueryResult.h"

class QueryProjector {
  private:
    std::vector<Synonym> target_synonym_list;
  public:
    QueryProjector(std::vector<Synonym> target_synonym_list);
    std::vector<std::string> FormatQuery(UnformattedQueryResult unformatted_results);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
