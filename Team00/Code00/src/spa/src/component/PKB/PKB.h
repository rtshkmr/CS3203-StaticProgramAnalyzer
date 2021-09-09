#pragma once

#include <datatype/Deliverable.h>
#include <model/Statement.h>
#include <component/QueryProcessor/types/Types.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <unordered_map>

using namespace std;


class PKB {
 public:
  void PopulateDataStructures(Deliverable d);

  list<string> GetDesignEntity(DesignEntity de);
  list<tuple<DesignEntity,string>> GetFollows(string stmt);
  //    list<tuple<DesignEntity,string>> GetPrevious(string stmt);
  list<tuple<DesignEntity,string>> GetParent(string stmt);
  //    list<tuple<DesignEntity,string>> GetChild(string stmt);
  PKB() = default;
 private:

  list<string> proc_table_;
  list<string> var_table_;
  list<string> const_table_;
  list<string> stmt_table_;
  list<string> if_table_;
  list<string> while_table_;
  list<string> assign_table_;
  list<string> call_table_;
  list<string> print_table_;
  list<string> read_table_;

  unordered_map<string, DesignEntity> type_map_;

  unordered_map<string, tuple<DesignEntity,string>> follows_map_;
  //    unordered_map<string, tuple<DesignEntity,string>> previous_map_;
  unordered_map<string, list<tuple<DesignEntity,string>>> parent_map_;
  //    unordered_map<string, tuple<DesignEntity,string>> child_map_;

  //    unordered_map<string, list<tuple<DesignEntity,string>>> use_map_;
  //    unordered_map<string, list<tuple<DesignEntity,string>>> modifies_map_;

  void PopulateProcList(const list<Procedure*>& proc_list);
  void PopulateVarList(const list<Variable*>& var_list);
  void PopulateConstList(const list<ConstantValue*>& const_list);
  void PopulateStmtList(const list<Statement*>& stmt_list);
  void PopulateIfList(const list<IfEntity*>& if_list);
  void PopulateWhileList(const list<WhileEntity*>& while_list);
  void PopulateAssignList(const list<AssignEntity*>& assign_list);
  void PopulateCallList(const list<CallEntity*>& call_list);
  void PopulatePrintList(const list<PrintEntity*>& print_list);
  void PopulateReadList(const list<ReadEntity*>& read_list);

  void PopulateFollowsMap(const unordered_map<Statement*, Statement*>& follow_hash);
  //    void PopulateFollowedByMap(const unordered_map<Statement*, Statement*>& follow_hash);

  void PopulateParentMap(const unordered_map<Statement*, list<Statement*>>& parent_hash);
  //    void PopulateChildMap(const unordered_map<Statement*, Statement*>& parent_hash);

  //    void PopulateUseMap(const unordered_map<Statement*, Entity*>& use_hash);
  //    void PopulateUsedByMap(const unordered_map<Statement*, Entity*>& use_hash);
  //    void PopulateModifiesMap(const unordered_map<Statement*, Entity*>& modifies_hash);
  //    void PopulateModifiedByMap(const unordered_map<Statement*, Entity*>& modifies_hash);
};
