#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_DBMANAGER_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_DBMANAGER_H_

#include "PKB.h"
#include "component/PKB/extractor/RuntimeExtractor.h"

/**
 * This class provides a facade for the PKB and RuntimeExtractor.
 */
class DBManager {
 public:
  DBManager(PKB* pkb);
  void Delete();
  void TurnOffOptimization();

  // PKB API
  // Population of PKB
  void PopulateDataStructures(Deliverable d);

  // Getting relationships from PKB
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);

  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de);

  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de,
                                                    std::vector<Entity *> scoped_entities,
                                                    ScopeIndication scope_indication);

  std::vector<Entity*> GetRelationshipByTypes(PKBRelRefs ref,
                                              DesignEntity first_de,
                                              DesignEntity second_de,
                                              std::vector<Entity *> left_scoped_entities,
                                              std::vector<Entity *> right_scoped_entities,
                                              ScopeIndication scope_indication);

  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref,
                                                                   DesignEntity first,
                                                                   DesignEntity second);

  // Getting entities from PKB by type
  std::vector<Entity*> GetDesignEntities(DesignEntity de);

  // Getting entities for pattern matching
  std::vector<Entity*> GetPatternEntities(DesignEntity de, std::string stmtRef);

  // Check if relationship exists
  bool HasRelationship(PKBRelRefs ref);
  bool HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second);
  bool HasRelationship(PKBRelRefs ref, std::string first, std::string second);
  bool HasRelationship(PKBRelRefs ref, std::string ident_string);

  DesignEntity GetDesignEntityFromEntity(Entity* entity);

  static std::string GetNameFromEntity(Entity* entity);

 private:
  PKB* pkb_;
  RuntimeExtractor* runtime_extractor_;
  bool is_optimized_ = true;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_DBMANAGER_H_
