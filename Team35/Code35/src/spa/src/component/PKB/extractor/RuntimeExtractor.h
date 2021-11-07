#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_

#include <model/Entity.h>
#include <component/QueryProcessor/types/Types.h>
#include <component/PKB/extractor/NextBipExtractor.h>
#include <component/PKB/extractor/NextBipTExtractor.h>
#include <component/PKB/extractor/RuntimeMediator.h>
#include "component/PKB/PKB.h"
#include "NextTExtractor.h"
#include "AffectsExtractor.h"
#include "AffectsTExtractor.h"
#include "AffectsBipExtractor.h"
#include "AffectsBipTExtractor.h"

const std::unordered_set<PKBRelRefs> runtime_relationships = {
    PKBRelRefs::kNextT,
    PKBRelRefs::kPreviousT,
    PKBRelRefs::kAffects,
    PKBRelRefs::kAffectedBy,
    PKBRelRefs::kAffectsT,
    PKBRelRefs::kAffectedByT,
    PKBRelRefs::kNextBip,
    PKBRelRefs::kPrevBip,
    PKBRelRefs::kNextBipT,
    PKBRelRefs::kPrevBipT,
    PKBRelRefs::kAffectsBip,
    PKBRelRefs::kAffectedByBip,
    PKBRelRefs::kAffectsBipT,
    PKBRelRefs::kAffectedByBipT
};

const std::unordered_set<DesignEntity> next_design_entities = {
    DesignEntity::kRead,
    DesignEntity::kPrint,
    DesignEntity::kCall,
    DesignEntity::kWhile,
    DesignEntity::kIf,
    DesignEntity::kAssign,
    DesignEntity::kStmt,
    DesignEntity::kProgLine
};

/**
 * This class acts as a facade for runtime extraction of relationships, for the DBManager to query. It abstracts out
 * the separate classes responsible for specific relationship extraction.
 */
class RuntimeExtractor : public RuntimeMediator {
 public:
  explicit RuntimeExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref, DesignEntity first, DesignEntity second) override;
  bool HasRelationship(PKBRelRefs ref) override;
  bool HasRelationship(PKBRelRefs ref, std::string first) override;
  bool HasRelationship(PKBRelRefs ref, std::string first, std::string second) override;
  bool HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) override;

  void Delete();
  static bool IsRuntimeRelationship(PKBRelRefs ref);

  std::vector<Entity*> GetScopedFirstEntities(PKBRelRefs ref, const std::vector<Entity*>& scoped_entities);
  std::vector<std::tuple<Entity*, Entity*>> GetAllRelationshipsScoped(PKBRelRefs ref,
                                                                      std::vector<Entity*> left_entities,
                                                                      std::vector<Entity*> right_entities,
                                                                      ScopeIndication scope_indication,
                                                                      type_combo types);
 private:
  PKB* pkb_;
  NextTExtractor next_t_extractor_;
  AffectsExtractor affects_extractor_;
  AffectsTExtractor affects_t_extractor_;
  NextBipExtractor next_bip_extractor_;
  NextBipTExtractor next_bip_t_extractor_;
  AffectsBipExtractor affects_bip_extractor_;
  AffectsBipTExtractor affects_bip_t_extractor_;

  std::pair<RuntimeColleague*,RelDirection> GetExtractorAndDirection(PKBRelRefs ref);
  static PKBRelRefs ReverseRelationship(PKBRelRefs ref);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
