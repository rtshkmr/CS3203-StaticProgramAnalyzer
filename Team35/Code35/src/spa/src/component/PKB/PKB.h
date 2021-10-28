#pragma once

#include <datatype/Deliverable.h>
#include <model/Statement.h>
#include <component/QueryProcessor/types/Types.h>

#include <stdio.h>
#include <iostream>

typedef std::tuple<DesignEntity, DesignEntity> type_combo;

struct type_combo_hash {
  std::size_t operator()(const type_combo& combo) const
  {
      return static_cast<std::size_t>(std::get<0>(combo))
      * static_cast<std::size_t>(DesignEntity::kInvalid)
      + static_cast<std::size_t>(std::get<1>(combo));
  }
};

struct attribute_hash {
  std::size_t operator()(const std::tuple<DesignEntity, Attribute>& combo) const
  {
    return static_cast<std::size_t>(std::get<0>(combo))
    * static_cast<std::size_t>(DesignEntity::kInvalid)
    + static_cast<std::size_t>(std::get<1>(combo));
  }
};

typedef std::tuple<Entity*, Entity*> entity_pair;

typedef std::tuple<PKBRelRefs, std::string, std::string> relationship;

class PKB {
 public:
  void PopulateDataStructures(Deliverable d);
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);
  std::vector<entity_pair> GetRelationshipByTypes(PKBRelRefs, DesignEntity, DesignEntity);
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs, DesignEntity, DesignEntity);
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs, DesignEntity);
  std::vector<Entity*> GetDesignEntities(DesignEntity de);
  std::vector<Entity*> GetPatternEntities(DesignEntity de, std::string var_or_stmt);
  std::vector<Entity*> GetEntitiesWithAttributeValue(DesignEntity design_entity, Attribute attribute, std::string value);
  std::vector<entity_pair> GetEntitiesWithMatchingAttributes(DesignEntity type_one, DesignEntity type_two);
  bool HasRelationship(PKBRelRefs);
  bool HasRelationship(PKBRelRefs, DesignEntity, DesignEntity);
  bool HasRelationship(PKBRelRefs, std::string);
  bool HasRelationship(PKBRelRefs, std::string, std::string);
  static std::string GetNameFromEntity(Entity* entity);
  static DesignEntity GetDesignEntityFromEntity(Entity* entity);
  static Attribute GetAttributeFromEntity(Entity* entity);

  // Constructor
  PKB() = default;

 private:
  std::unordered_map<DesignEntity, std::vector<Entity*>> type_to_entity_map_;

  std::unordered_map<
    DesignEntity,
    std::unordered_map<
      std::string,
      std::vector<Entity*>
    >
  > pattern_maps_;

  std::unordered_map<
    PKBRelRefs,
    std::unordered_map<
      std::string,
      std::vector<Entity*>
    >
  > relationship_table_;

  std::unordered_map<
    PKBRelRefs,
    std::unordered_map<
      type_combo,
      std::vector<entity_pair>,
      type_combo_hash
    >
  > relationship_by_types_table_;

  std::unordered_map<
    PKBRelRefs,
    std::unordered_map<
      type_combo,
      std::vector<Entity*>,
      type_combo_hash
    >
  > first_param_by_types_table_;

  std::set<relationship> relationship_set_;

  std::unordered_map<
    std::tuple<DesignEntity, Attribute>,
    std::unordered_map<
      std::string,
      std::vector<Entity*>
    >,
    attribute_hash
  > attribute_to_entity_map_;


  std::unordered_map<
    std::string,
    std::unordered_set<Entity*>
  > attribute_string_to_entity_map_;

  std::unordered_map<
    type_combo,
    std::vector<entity_pair>,
    type_combo_hash
  > entities_with_matching_attributes_map_;

  template <typename T>
  void PopulateEntities(DesignEntity design_entity, T& entity_list);

  template <typename X, typename Y>
  void PopulateRelationship(std::unordered_map<X*, std::list<Y*>*>* hash, PKBRelRefs ref);

  void ProcessEntitiesWithMatchingAttributes();

  std::vector<DesignEntity> GetApplicableTypes(DesignEntity de);

  void PopulateContainerUse(std::unordered_map<Container*, std::list<Variable*>*> container_use_hash_);
  void PopulateContainerUsedBy(std::unordered_map<Variable*, std::list<Container*>*> container_used_by_hash_);
  void PopulateContainerModifies(std::unordered_map<Container*, std::list<Variable*>*> container_modifies_hash_);
  void PopulateContainerModifiedBy(std::unordered_map<Variable*, std::list<Container*>*> container_modified_by_hash_);

  void PopulateUses();
  void PopulateUsedBy();
  void PopulateModifies();
  void PopulateModifiedBy();
};
