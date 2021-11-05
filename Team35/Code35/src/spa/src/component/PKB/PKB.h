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

typedef std::tuple<DesignEntity, Attribute> type_attribute_pair;

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

  // Returns a stmt number to vector of related Entities map.
  std::unordered_map<std::string, std::vector<Entity*>> GetRelationshipMap(PKBRelRefs ref);

  // Returns a vector of all entities in a relationship with the specified entity
  // E.g. GetRelationship(kFollows, 1) returns a vector with one Entity with statement number 2
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);

  // Returns a vector of 2-tuples of entities in which the first element of the tuple is specified
  // by entity and the second element is any element for which the ref relationship holds with entity
  std::vector<entity_pair> GetRelationshipByFirst(PKBRelRefs ref, std::string entity);

  // Returns a vector of 2-tuples of entities in which the second element of the tuple is specified
  // by entity and the first element is any element for which the ref relationship holds with entity
  std::vector<entity_pair> GetRelationshipBySecond(PKBRelRefs ref, std::string entity);

  // Returns a vector of 2-tuples of entities in which the first element of the tuple is of type de1,
  // the second elements is of type de2, and the ref relationship holds between them
  std::vector<entity_pair> GetRelationshipByTypes(PKBRelRefs ref, DesignEntity de1, DesignEntity de2);

  // Returns a vector of entities of type de1 such that the ref relationship holds between
  // the specified entity and another entity of type de2
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de1, DesignEntity de2);

  // Returns a vector of entities of type dee such that the ref relationship holds between
  // the specified entity and another entity of any type
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de);

  // Returns a vector of entities of type de
  std::vector<Entity*> GetDesignEntities(DesignEntity de);

  // Returns a vector of While, If or Assign entities (determined by type de) such that
  // the entity is associated with var_or_stmt. If var_or_stmt is an integer string,
  // returns a vector with one Entity with the specified statement number.
  // Otherwise, return either AssignEntities that modify the specified variable or WhileEntities/IfEntities
  // that have the specified variable in their condition.
  std::vector<Entity*> GetPatternEntities(DesignEntity de, std::string var_or_stmt);

  // Returns a vector of entities of type de whose attribute attribute matches the value
  std::vector<Entity*> GetEntitiesWithAttributeValue(DesignEntity de, Attribute attribute, std::string value);

  // Returns a vector of 2-tuples of entities in which the type and attribute of the first element is defined by type_one,
  // and that of the second elements is defined by de2
  std::vector<entity_pair> GetEntitiesWithMatchingAttributes(type_attribute_pair type_one, type_attribute_pair type_two);

  // Returns true if the relationship exists, false otherwise
  bool HasRelationship(PKBRelRefs);

  // Returns true if the relationship exists between two entities of the specified types, false otherwise
  bool HasRelationship(PKBRelRefs, DesignEntity, DesignEntity);

  // Returns true if the relationship exists with the first entity specified by the string, false otherwise
  bool HasRelationship(PKBRelRefs, std::string);

  // Returns true if the relationship exists between the two entities specified by strings, false otherwise
  bool HasRelationship(PKBRelRefs, std::string, std::string);

  // Returns the name of any entity in string format
  // (statements - statement number, variables - variable name, procedures - procedure name, constant - value)
  static std::string GetNameFromEntity(Entity* entity);

  // Returns the type of any entity in DesignEntity format
  static DesignEntity GetDesignEntityFromEntity(Entity* entity);

  // Returns a string corresponding to a specific attribute of an entity
  static std::string GetAttributeFromEntity(Entity* entity, Attribute);

  // Returns a vector of Attribute types that an entity possesses
  static std::vector<Attribute> GetAttributeTypes(Entity* entity);

  // Returns an unordered_map of Attribute to attribute strings
  static std::unordered_map<Attribute, std::string> GetAttributesFromEntity(Entity*);

  // Constructor
  PKB() = default;

  template <typename X, typename Y>
  void PopulateRelationship(std::unordered_map<X*, std::list<Y*>*>* hash, PKBRelRefs ref);

  std::unordered_map<Entity*, std::list<Entity*>*>* ConvertStringToEntityMapping(const std::unordered_map<std::string,
                                                                                 std::vector<Entity*>>& pkb_map);

  Program* GetProgram();

 private:
  Program* program_;

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
      std::string,
      std::vector<entity_pair>
    >
  > relationship_by_first_entity_table_;

  std::unordered_map<
    PKBRelRefs,
    std::unordered_map<
      std::string,
      std::vector<entity_pair>
    >
  > relationship_by_second_entity_table_;

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
    std::set<Entity*>
  > attribute_string_to_entity_map_;

  std::unordered_map<
    Entity*,
    std::unordered_map<
      std::string,
      Attribute
    >
  > entity_to_attribute_type_map_;

  std::unordered_map<
    type_attribute_pair,
    std::unordered_map<
      type_attribute_pair,
      std::vector<entity_pair>,
      attribute_hash
    >,
    attribute_hash
  > entities_with_matching_attributes_map_;

  template <typename T>
  void PopulateEntities(DesignEntity design_entity, T& entity_list);

  void ProcessEntitiesWithMatchingAttributes();

  std::vector<DesignEntity> GetApplicableTypes(DesignEntity de);
};


/**
 * Populates the respective hashmap and tables in the pkb for the ref according to the hashmap provided.
 * Population will only be carried out once for each ref.
 * @tparam X Type of key of hash.
 * @tparam Y Type of element in the value list of hash.
 * @param hash Hashmap used to provide information to populate the pkb.
 * @param ref Type of relationship.
 */
template <typename X, typename Y>
void PKB::PopulateRelationship(std::unordered_map<X*, std::list<Y*>*>* hash, PKBRelRefs ref) {
  for (std::pair<X*, std::list<Y*>*> kv: *hash) {
    Entity* first_entity = dynamic_cast<Entity*>(kv.first);
    std::string k_string = GetNameFromEntity(first_entity);
    DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
    std::vector<DesignEntity> first_types = GetApplicableTypes(first_type);
    std::vector<DesignEntity> second_types;

    for (Y* e : *kv.second) {
      Entity* entity = dynamic_cast<Entity*>(e);
      relationship_set_.insert({ref, k_string, GetNameFromEntity(entity)});
      relationship_table_[ref][k_string].push_back(entity);
      relationship_by_first_entity_table_[ref][k_string].push_back({first_entity, entity});
      relationship_by_second_entity_table_[ref][GetNameFromEntity(entity)].push_back({first_entity, entity});

      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      second_types = GetApplicableTypes(second_type);

      for (DesignEntity type1 : first_types) {
        for (DesignEntity type2 : second_types) {
          relationship_by_types_table_[ref][{type1, type2}].push_back({first_entity, entity});
          std::vector<Entity*> first_params = first_param_by_types_table_[ref][{type1, type2}];
          if (std::find(first_params.begin(), first_params.end(), first_entity) == first_params.end()) {
            first_param_by_types_table_[ref][{type1, type2}].push_back(first_entity);
          }
        }
      }
    }
  }
}
