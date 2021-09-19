#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H

#include <vector>
#include <string>
#include <algorithm>

class QueryProjector {
  public:
    static std::list<std::string> FormatQuery(std::vector<std::string> results);
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYPROJECTOR_H
