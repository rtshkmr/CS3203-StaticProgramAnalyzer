#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H

#include <list>
#include <string>
#include <component/QueryProcessor/types/Types.h>

/**
 * QueryExtractor is in charge of tokenizing, parsing and validating a query,
 * before sending it off to QueryEvaluator.
 */

class QueryExtractor {
  private:
    std::string& query;
    std::list<Group> groups;
    std::list<Synonym> synonyms;
    Synonym target;
  public:
    QueryExtractor(std::string& query) : query(query) {};
    void extractQuery();
    std::list<Group> getGroups() { return groups; };
    std::list<Synonym> getSynonymList() { return synonyms; };
    Synonym getTarget() { return target; };
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
