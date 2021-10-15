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

typedef std::tuple<PKBRelRefs, std::string, std::string> relationship;

class PKB {
 public:
  // Population of PKB
  void PopulateDataStructures(Deliverable d);

  // Getting relationships from PKB
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);

  std::vector<Entity*> GetRelationshipByType(PKBRelRefs, DesignEntity);

  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs, DesignEntity, DesignEntity);

  // Getting entities from PKB by type
  std::vector<Entity*> GetDesignEntities(DesignEntity de);

  // Getting entities for pattern matching
  std::vector<Entity*> GetAssignEntityByStmtRef(std::string stmtRef);
  std::vector<Entity*> GetAssignEntityByVariable(std::string variable);
  std::vector<Entity*> GetWhileEntityByStmtRef(std::string stmtRef);
  std::vector<Entity*> GetWhileEntityByVariable(std::string variable);
  std::vector<Entity*> GetIfEntityByStmtRef(std::string stmtRef);
  std::vector<Entity*> GetIfEntityByVariable(std::string variable);

  // Check if relationship exists
  bool HasRelationship(PKBRelRefs);
  bool HasRelationship(PKBRelRefs, DesignEntity, DesignEntity);
  bool HasRelationship(PKBRelRefs, std::string, std::string);

  DesignEntity EntityToDesignEntity(Entity* entity);

  // Constructor
  PKB() = default;

 private:

  std::unordered_map<DesignEntity, std::vector<std::tuple<DesignEntity, DesignEntity>>> first_param_map_;
  std::unordered_map<DesignEntity, std::vector<std::tuple<DesignEntity, DesignEntity>>> second_param_map_;

  std::unordered_map<DesignEntity, std::vector<Entity*>> type_to_entity_map_;
  std::unordered_map<std::string, DesignEntity> entity_string_to_type_map_;
  std::unordered_map<Entity*, DesignEntity> entity_object_to_type_map_;

  std::unordered_map<std::string, std::vector<Entity*>> assign_map_;
  std::unordered_map<std::string, std::vector<Entity*>> while_map_;
  std::unordered_map<std::string, std::vector<Entity*>> if_map_;

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
      std::vector<
        std::tuple<Entity*, Entity*>
      >,
      type_combo_hash
    >
  > relationship_by_type_table_;

  std::set<DesignEntity> stmt_design_entities_;
  std::set<relationship> relationship_set_;

  void InitializeDataStructures();

  void PopulateProcEntities(const std::list<Procedure*>& proc_list);
  void PopulateVarEntities(const std::list<Variable*>& var_list);
  void PopulateConstEntities(const std::list<ConstantValue*>& const_list);
  void PopulateStmtEntities(const std::list<Statement*>& stmt_list);
  void PopulateIfEntities(const std::list<IfEntity*>& if_list);
  void PopulateWhileEntities(const std::list<WhileEntity*>& while_list);
  void PopulateAssignEntities(const std::list<AssignEntity*>& assign_list);
  void PopulateCallEntities(const std::list<CallEntity*>& call_list);
  void PopulatePrintEntities(const std::list<PrintEntity*>& print_list);
  void PopulateReadEntities(const std::list<ReadEntity*>& read_list);

  void PopulateRelationship(std::unordered_map<Entity*, std::list<Entity*>*>* hash, PKBRelRefs ref);

  void PopulateFollows(std::unordered_map<Statement*, Statement*>& follow_hash);
  void PopulateFollowedBy(std::unordered_map<Statement*, Statement*>& followed_by_hash);
  void PopulateChild(std::unordered_map<Statement*, Statement*>& child_to_parent_hash);
  void PopulateContainerUse(std::unordered_map<Container*, std::list<Variable*>*> container_use_hash_);
  void PopulateContainerUsedBy(std::unordered_map<Variable*, std::list<Container*>*> container_used_by_hash_);
  void PopulateContainerModifies(std::unordered_map<Container*, std::list<Variable*>*> container_modifies_hash_);
  void PopulateContainerModifiedBy(std::unordered_map<Variable*, std::list<Container*>*> container_modified_by_hash_);

  void PopulateUses();
  void PopulateUsedBy();
  void PopulateModifies();
  void PopulateModifiedBy();

  std::string GetNameFromEntity(Entity* entity);
};
