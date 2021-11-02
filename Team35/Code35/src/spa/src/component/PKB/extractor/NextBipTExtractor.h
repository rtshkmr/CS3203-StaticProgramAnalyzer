#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_

#include <component/PKB/PKB.h>

class NextBipTExtractor {
 public:
  NextBipTExtractor() = default;
  explicit NextBipTExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, const std::string& target);
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref);
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref);
  bool HasRelationship(PKBRelRefs ref);
  bool HasRelationship(PKBRelRefs ref, std::string target);
  bool HasRelationship(PKBRelRefs ref, std::string first, std::string second);
 private:
  PKB* pkb_;
  void PopulateRelationships();
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
