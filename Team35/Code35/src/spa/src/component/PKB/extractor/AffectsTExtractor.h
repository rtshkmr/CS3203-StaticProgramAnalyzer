#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>
#include <component/PKB/extractor/RuntimeColleague.h>
#include <component/PKB/extractor/RuntimeMediator.h>
#include <unordered_map>

class AffectsTExtractor : public RuntimeColleague {
 public:
  AffectsTExtractor() = default;
  explicit AffectsTExtractor(RuntimeMediator* rte, PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                   DesignEntity first,
                                                                   DesignEntity second) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, int target) override;
  bool HasRelationship(RelDirection dir, int first, int second) override;
  bool HasRelationship(RelDirection dir, DesignEntity first, DesignEntity second) override;
  void Delete();
 private:
  RuntimeMediator* rte_ = nullptr;
  PKB* pkb_ = nullptr;
  bool isCached = false;
  std::unordered_map<int, std::list<int>*> affects_t_map_ = {};
  std::unordered_map<int, std::list<int>*> affected_by_t_map_ = {};
  void InitCache();
  std::vector<Entity*> ConvertIntToEntity(const std::set<int>& set_to_convert);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
