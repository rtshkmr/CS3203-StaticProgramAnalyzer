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

  // Getting relationships from PKB
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);

  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de);

  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de,
                                                    std::vector<Entity*> scoped_entities,
                                                    ScopeIndication scope_indication);

  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref,
                                                                   DesignEntity first_de,
                                                                   DesignEntity second_de,
                                                                   std::vector<Entity*> left_scoped_entities,
                                                                   std::vector<Entity*> right_scoped_entities,
                                                                   ScopeIndication scope_indication);

  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref,
                                                                   DesignEntity first,
                                                                   DesignEntity second);

  // Getting entities from PKB by type
  std::vector<Entity*> GetDesignEntities(DesignEntity de);

  // Getting entities for pattern matching
  std::vector<Entity*> GetPatternEntities(DesignEntity de, std::string stmtRef);

  // Returns a vector of entities of type de whose attribute attribute matches the value
  std::vector<Entity*> GetEntitiesWithAttributeValue(DesignEntity de, Attribute attribute, std::string value);

  // Returns a vector of 2-tuples of entities in which the type and attribute of the first element is defined by type_one,
  // and that of the second elements is defined by de2
  std::vector<entity_pair> GetEntitiesWithMatchingAttributes(type_attribute_pair type_one,
                                                             type_attribute_pair type_two);

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
  static std::vector<std::tuple<Entity*, Entity*>> MergeScopedEntities(std::vector<Entity*> left_entities,
                                                                       std::vector<Entity*> right_entities);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_DBMANAGER_H_
