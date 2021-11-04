#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>
#include <component/PKB/extractor/RuntimeColleague.h>
#include <component/PKB/extractor/RuntimeMediator.h>
#include <unordered_map>

class AffectsTExtractor : RuntimeColleague {
 public:
  AffectsTExtractor() = default;
  explicit AffectsTExtractor(RuntimeMediator* rte, PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target);
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir);
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir);
  bool HasRelationship(RelDirection dir);
  bool HasRelationship(RelDirection dir, int target);
  bool HasRelationship(RelDirection dir, int first, int second);
  void Delete();
 private:
  RuntimeMediator* rte_ = nullptr;
  PKB* pkb_ = nullptr;
  bool isCached = false;
  std::unordered_map<int, std::list<int>*> affects_t_map_ = {};
  std::unordered_map<int, std::list<int>*> affected_by_t_map_ = {};
  void InitCache();
  std::vector<Entity*> ConvertIntToEntity(std::set<int> set_to_convert);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
