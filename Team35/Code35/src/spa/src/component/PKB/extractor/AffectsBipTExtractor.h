#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPTEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>
#include "RuntimeColleague.h"
#include "RuntimeMediator.h"
#include <unordered_map>

class AffectsBipTExtractor : public RuntimeColleague {
 public:
  AffectsBipTExtractor() = default;
  explicit AffectsBipTExtractor(RuntimeMediator* rte, PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target);
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de);
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                   DesignEntity first,
                                                                   DesignEntity second);
  bool HasRelationship(RelDirection dir);
  bool HasRelationship(RelDirection dir, int target);
  bool HasRelationship(RelDirection dir, int first, int second);

 private:
  RuntimeMediator* rte_ = nullptr;
  PKB* pkb_ = nullptr;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPTEXTRACTOR_H_
