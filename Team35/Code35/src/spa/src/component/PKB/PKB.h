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

  // Getting type from entity string
  [[deprecated]] // Because the object itself with have a type_enum
  DesignEntity GetTypeFromEntityString(std::string entity_string);

  // Getting type from entity
  [[deprecated]] // Because the object itself with have a type_enum
  DesignEntity GetTypeFromEntity(Entity* entity);

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








  // TO BE DEPRECATED FROM HERE
  [[deprecated]]
  std::vector<AssignEntity> GetPatternByAssign(std::string stmtRef);
  [[deprecated]]
  std::vector<AssignEntity> GetPatternByVariable(std::string varName);

  [[deprecated]]
  std::list<std::string> GetDesignEntity(DesignEntity de);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetFollows(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetPrevious(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetParent(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetChild(std::string stmt);

  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetUses(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetUsedBy(std::string var);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetModifies(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetModifiedBy(std::string var);

  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetUsesP(std::string procedure);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetUsedByP(std::string var);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetModifiesP(std::string procedure);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetModifiedByP(std::string var);

  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetFollowsT(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetPreviousT(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetParentT(std::string stmt);
  [[deprecated]]
  std::list<std::tuple<DesignEntity, std::string>> GetChildT(std::string stmt);

  [[deprecated]]
  bool HasFollows();
  [[deprecated]]
  bool HasPrevious();
  [[deprecated]]
  bool HasParent();
  [[deprecated]]
  bool HasChild();
  [[deprecated]]
  bool HasFollowsT();
  [[deprecated]]
  bool HasPreviousT();
  [[deprecated]]
  bool HasParentT();
  [[deprecated]]
  bool HasChildT();
  [[deprecated]]
  bool HasUses();
  [[deprecated]]
  bool HasUsedBy();
  [[deprecated]]
  bool HasModifies();
  [[deprecated]]
  bool HasModifiedBy();
  // TO HERE








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



  std::string GetNameFromEntity(Entity* entity);

  std::set<DesignEntity> stmt_design_entities_;
  std::set<relationship> relationship_set_;

  // TO BE DEPRECATED FROM HERE

  std::unordered_map<std::string, DesignEntity> type_map_;

  std::list<std::string> proc_table_;
  std::list<std::string> var_table_;
  std::list<std::string> const_table_;
  std::list<std::string> stmt_table_;
  std::list<std::string> if_table_;
  std::list<std::string> while_table_;
  std::list<std::string> assign_table_;
  std::list<std::string> call_table_;
  std::list<std::string> print_table_;
  std::list<std::string> read_table_;

  std::unordered_map<std::string, std::tuple<DesignEntity, std::string>> follows_map_;
  std::unordered_map<std::string, std::tuple<DesignEntity, std::string>> previous_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> parent_map_;
  std::unordered_map<std::string, std::tuple<DesignEntity, std::string>> child_map_;

  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> follows_t_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> previous_t_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> parent_t_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> child_t_map_;

  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> use_s_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> used_by_s_map_;

  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> use_c_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> used_by_c_map_;

  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> modifies_s_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> modified_by_s_map_;

  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> modifies_c_map_;
  std::unordered_map<std::string, std::list<std::tuple<DesignEntity, std::string>*>*> modified_by_c_map_;

  std::unordered_map<std::string, std::vector<AssignEntity>> assign_expr_map_;

  void PopulateProcList(const std::list<Procedure*>& proc_list);
  void PopulateVarList(const std::list<Variable*>& var_list);
  void PopulateConstList(const std::list<ConstantValue*>& const_list);
  void PopulateStmtList(const std::list<Statement*>& stmt_list);
  void PopulateIfList(const std::list<IfEntity*>& if_list);
  void PopulateWhileList(const std::list<WhileEntity*>& while_list);
  void PopulateAssignList(const std::list<AssignEntity*>& assign_list);
  void PopulateCallList(const std::list<CallEntity*>& call_list);
  void PopulatePrintList(const std::list<PrintEntity*>& print_list);
  void PopulateReadList(const std::list<ReadEntity*>& read_list);

  void PopulateFollowsMap(const std::unordered_map<Statement*, Statement*>& follow_hash);
  void PopulatePreviousMap(const std::unordered_map<Statement*, Statement*>& followed_by_hash);

  void PopulateParentMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_hash);
  void PopulateChildMap(const std::unordered_map<Statement*, Statement*>& parent_of_hash);

  void PopulateFollowsTMap(std::unordered_map<Statement*, std::list<Statement*>*> follows_T_hash);
  void PopulatePreviousTMap(std::unordered_map<Statement*, std::list<Statement*>*> followed_by_T_hash);
  void PopulateParentTMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_T_hash);
  void PopulateChildTMap(std::unordered_map<Statement*, std::list<Statement*>*> child_to_parent_T_hash);

  void PopulateUseSMap(std::unordered_map<Statement*, std::list<Variable*>*> use_hash);
  void PopulateUsedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> used_by_hash);

  void PopulateUseCMap(std::unordered_map<Container*, std::list<Variable*>*> c_use_hash);
  void PopulateUsedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_used_by_hash);

  void PopulateModifiesSMap(std::unordered_map<Statement*, std::list<Variable*>*> modifies_hash);
  void PopulateModifiedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> modified_by_hash);

  void PopulateModifiesCMap(std::unordered_map<Container*, std::list<Variable*>*> c_modifies_hash);
  void PopulateModifiedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_modified_by_hash);


  // TO HERE
};
