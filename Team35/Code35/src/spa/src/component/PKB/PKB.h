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

class PKB {
 public:
  // Population of PKB
  void PopulateDataStructures(Deliverable d);

  // Getting relationships from PKB
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string entity);
  // todo: implement GetRelationshipByTypes
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs, DesignEntity, DesignEntity);

  // Getting entities from PKB by type
  std::vector<Entity*> GetDesignEntities(DesignEntity de);

  // Getting type from entity string
  DesignEntity GetTypeFromEntityString(std::string entity_string);

  // Getting type from entity
  DesignEntity GetTypeFromEntity(Entity* entity);

  // Getting entities for pattern matching
  std::vector<AssignEntity*> GetAssignEntityByStmtRef(std::string stmtRef);
  std::vector<AssignEntity*> GetAssignEntityByVariable(std::string variable);
  std::vector<WhileEntity*> GetWhileEntityByStmtRef(std::string stmtRef);
  std::vector<WhileEntity*> GetWhileEntityByVariable(std::string variable);
  std::vector<IfEntity*> GetIfEntityByStmtRef(std::string stmtRef);
  std::vector<IfEntity*> GetIfEntityByVariable(std::string variable);

  // Check if relationship exists
  bool HasRelationship(PKBRelRefs);
  // todo: implement HasRelationship with entity specification
  bool HasRelationship(PKBRelRefs, DesignEntity, DesignEntity);

  // Constructor
  PKB() = default;








  // TO BE DEPRECATED FROM HERE
  std::vector<AssignEntity> GetPatternByAssign(std::string stmtRef);
  std::vector<AssignEntity> GetPatternByVariable(std::string varName);

  std::list<std::string> GetDesignEntity(DesignEntity de);
  std::list<std::tuple<DesignEntity, std::string>> GetFollows(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetPrevious(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetParent(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetChild(std::string stmt);

  std::list<std::tuple<DesignEntity, std::string>> GetUses(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetUsedBy(std::string var);
  std::list<std::tuple<DesignEntity, std::string>> GetModifies(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetModifiedBy(std::string var);

  std::list<std::tuple<DesignEntity, std::string>> GetUsesP(std::string procedure);
  std::list<std::tuple<DesignEntity, std::string>> GetUsedByP(std::string var);
  std::list<std::tuple<DesignEntity, std::string>> GetModifiesP(std::string procedure);
  std::list<std::tuple<DesignEntity, std::string>> GetModifiedByP(std::string var);

  std::list<std::tuple<DesignEntity, std::string>> GetFollowsT(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetPreviousT(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetParentT(std::string stmt);
  std::list<std::tuple<DesignEntity, std::string>> GetChildT(std::string stmt);

  bool HasFollows();
  bool HasPrevious();
  bool HasParent();
  bool HasChild();
  bool HasFollowsT();
  bool HasPreviousT();
  bool HasParentT();
  bool HasChildT();
  bool HasUses();
  bool HasUsedBy();
  bool HasModifies();
  bool HasModifiedBy();
  // TO HERE








 private:


  std::unordered_map<DesignEntity, std::vector<std::tuple<DesignEntity, DesignEntity>>> first_param_map_;
  std::unordered_map<DesignEntity, std::vector<std::tuple<DesignEntity, DesignEntity>>> second_param_map_;

  std::unordered_map<DesignEntity, std::vector<Entity*>> type_to_entity_map_;
  std::unordered_map<std::string, DesignEntity> entity_string_to_type_map_;
  std::unordered_map<Entity*, DesignEntity> entity_object_to_type_map_;

  std::unordered_map<std::string, std::vector<AssignEntity*>> assign_map_;
  std::unordered_map<std::string, std::vector<WhileEntity*>> while_map_;
  std::unordered_map<std::string, std::vector<IfEntity*>> if_map_;

  std::unordered_map<
    PKBRelRefs,
    std::unordered_map<
      std::string,
      std::vector<Entity*>
    >
  > relationship_table_;

//  todo: create relationship by type table, populate it and enable retrieval from it
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
