#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>
#include "RuntimeColleague.h"
#include "RuntimeMediator.h"
#include <unordered_map>

class AffectsBipExtractor : public RuntimeColleague {
 public:
  AffectsBipExtractor() = default;
  explicit AffectsBipExtractor(RuntimeMediator* rte, PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                           DesignEntity first,
                                                                           DesignEntity second) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, int target) override;
  bool HasRelationship(RelDirection dir, int first, int second) override;
  bool HasRelationship(RelDirection dir, DesignEntity first, DesignEntity second) override;

 private:
  RuntimeMediator* rte_ = nullptr;
  PKB* pkb_ = nullptr;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_AFFECTSBIPEXTRACTOR_H_
