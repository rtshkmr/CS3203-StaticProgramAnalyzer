#include "PKB.h"
#include <stdio.h>
#include <iostream>
#include <util/Logger.h>

constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

/**
 * Returns a list of strings representing elements of the specified DesignEntity type
 * @param de is an enum of the types of DesignEntity (e.g. read, print, assign, etc.)
 * @return list of strings in which each string is the identifier of an element
 */
std::list<std::string> PKB::GetDesignEntity(DesignEntity de) {
  std::list<std::string> result = std::list<std::string>();
  switch (de) {
    case DesignEntity::kStmt:return stmt_table_;
    case DesignEntity::kRead:return read_table_;
    case DesignEntity::kPrint:return print_table_;
    case DesignEntity::kCall:return call_table_;
    case DesignEntity::kWhile:return while_table_;
    case DesignEntity::kIf:return if_table_;
    case DesignEntity::kAssign:return assign_table_;
    case DesignEntity::kVariable:return var_table_;
    case DesignEntity::kConstant:return const_table_;
    case DesignEntity::kProcedure:return proc_table_;
    case DesignEntity::kInvalid:return {};
    default:return {};
  }
}

/**
 * Returns boolean of whether there exists any follows relationship
 * @param
 * @return True if there is at least one follows relationship, otherwise False
 */
bool PKB::HasFollows() {
  return !follows_map_.empty();
};

/**
 * Returns boolean of whether there exists any Previous relationship
 * @param
 * @return True if there is at least one previous relationship, otherwise False
 */
bool PKB::HasPrevious() {
  return !previous_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent relationship
 * @param
 * @return True if there is at least one parent relationship, otherwise False
 */
bool PKB::HasParent() {
  return !parent_map_.empty();
}

/**
 * Returns boolean of whether there exists any child relationship
 * @param
 * @return True if there is at least one child relationship, otherwise False
 */
bool PKB::HasChild() {
  return !child_map_.empty();
}

/**
 * Returns boolean of whether there exists any follows* relationship
 * @param
 * @return True if there is at least one follows* relationship, otherwise False
 */
bool PKB::HasFollowsT() {
  return !follows_t_map_.empty();
};

/**
 * Returns boolean of whether there exists any previous* relationship
 * @param
 * @return True if there is at least one previous* relationship, otherwise False
 */
bool PKB::HasPreviousT() {
  return !previous_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent* relationship
 * @param
 * @return True if there is at least one parent* relationship, otherwise False
 */
bool PKB::HasParentT() {
  return !parent_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any child* relationship
 * @param
 * @return True if there is at least one child* relationship, otherwise False
 */
bool PKB::HasChildT() {
  return !child_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any uses relationship
 * @param
 * @return True if there is at least one uses relationship, otherwise False
 */
bool PKB::HasUses() {
  // Must check if both container and statement maps are empty
  return !use_s_map_.empty() || !use_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any used by relationship
 * @param
 * @return True if there is at least one used by relationship, otherwise False
 */
bool PKB::HasUsedBy() {
  // Must check if both container and statement maps are empty
  return !used_by_s_map_.empty() || !used_by_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modifies relationship
 * @param
 * @return True if there is at least one modifies relationship, otherwise False
 */
bool PKB::HasModifies() {
  // Must check if both container and statement maps are empty
  return !modifies_s_map_.empty() || !modifies_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modified by relationship
 * @param
 * @return True if there is at least one modified by relationship, otherwise False
 */
bool PKB::HasModifiedBy() {
  // Must check if both container and statement maps are empty
  return !modified_by_s_map_.empty() || !modified_by_c_map_.empty();
}

/**
 * Populates the respective entity lists and relationship hashmaps
 * @param d is a Deliverable object initialised in PSubsystem
 * @return
 */
void PKB::PopulateDataStructures(Deliverable d) {
  L("... PKB will be populated by Deliverable object from SourceProcessor\n");

  PopulateProcList(d.proc_list_);
  PopulateVarList(d.var_list_);
  PopulateConstList(d.const_list_);
  PopulateIfList(d.if_list_);
  PopulateWhileList(d.while_list_);
  PopulateAssignList(d.assign_list_);
  PopulateCallList(d.call_list_);
  PopulatePrintList(d.print_list_);
  PopulateReadList(d.read_list_);
  PopulateStmtList(d.stmt_list_);

  PopulateFollowsMap(d.follow_hash_);
  PopulatePreviousMap(d.followed_by_hash_);

  PopulateParentMap(d.parent_to_child_hash_);
  PopulateChildMap(d.child_to_parent_hash_);

  PopulateFollowsTMap(d.follows_T_hash_);
  PopulatePreviousTMap(d.followed_by_T_hash_);
  PopulateParentTMap(d.parent_to_child_T_hash_);
  PopulateChildTMap(d.child_to_parent_T_hash_);

  PopulateUseSMap(d.use_hash_);
  PopulateUsedBySMap(d.used_by_hash_);
  PopulateUseCMap(d.container_use_hash_);
  PopulateUsedByCMap(d.container_used_by_hash_);
  PopulateModifiesSMap(d.modifies_hash_);
  PopulateModifiedBySMap(d.modified_by_hash_);
  PopulateModifiesCMap(d.container_modifies_hash_);
  PopulateModifiedByCMap(d.container_modified_by_hash_);

  L("[DONE] PKB POPULATED WITHOUT ERROR\n");
}

/**
 * Returns a list of tuples representing elements that fulfill the follows relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetFollows(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto follows_iter = follows_map_.find(stmt);
  if (follows_iter == follows_map_.end()) {
    return ret_list;
  } else {
    std::tuple<DesignEntity, std::string> follows = follows_iter->second;
    ret_list.push_back(follows);
    return ret_list;
  }
}

/**
 * Returns a list of tuples representing elements that fulfill the previous relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetPrevious(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto previous_iter = previous_map_.find(stmt);
  if (previous_iter == previous_map_.end()) {
    return ret_list;
  } else {
    std::tuple<DesignEntity, std::string> previous = previous_iter->second;
    ret_list.push_back(previous);
    return ret_list;
  }
}

/**
 * Returns a list of tuples representing elements that fulfill the parent relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetParent(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto parent_iter = parent_map_.find(stmt);
  if (parent_iter != parent_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* children = parent_iter->second;
    for (auto s: * children) {
      ret_list.push_back(* s);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the child relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetChild(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto child_iter = child_map_.find(stmt);
  if (child_iter == child_map_.end()) {
    return ret_list;
  } else {
    std::tuple<DesignEntity, std::string> child = child_iter->second;
    ret_list.push_back(child);
    return ret_list;
  }
}

/**
 * Returns a list of tuples representing elements that fulfill the follows* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetFollowsT(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto follows_iter = follows_t_map_.find(stmt);
  if (follows_iter != follows_t_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* before = follows_iter->second;
    for (auto s: * before) {
      ret_list.push_back(* s);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the previous* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetPreviousT(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto previous_iter = previous_t_map_.find(stmt);
  if (previous_iter != previous_t_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* after = previous_iter->second;
    for (auto s: * after) {
      ret_list.push_back(* s);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the parent* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetParentT(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto parent_iter = parent_t_map_.find(stmt);
  if (parent_iter != parent_t_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* children = parent_iter->second;
    for (auto s: * children) {
      ret_list.push_back(* s);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the child* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetChildT(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  auto child_iter = child_t_map_.find(stmt);
  if (child_iter != child_t_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* parents = child_iter->second;
    for (auto s: * parents) {
      ret_list.push_back(* s);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the uses relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUses(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  // For statement map
  auto use_s_iter = use_s_map_.find(stmt);
  if (use_s_iter != use_s_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* use_s = use_s_iter->second;
    for (auto v: * use_s) {
      ret_list.push_back(* v);
    }
  }
  // For container map
  auto use_c_iter = use_c_map_.find(stmt);
  if (use_c_iter != use_c_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* use_c = use_c_iter->second;
    for (auto v: * use_c) {
      ret_list.push_back(* v);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the used by relationship with the specified statement
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsedBy(std::string var) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  // For statement map
  auto used_by_s_iter = used_by_s_map_.find(var);
  if (used_by_s_iter != used_by_s_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* used_by_s = used_by_s_iter->second;
    for (auto s: * used_by_s) {
      ret_list.push_back(* s);
    }
  }
  // For container map
  auto used_by_c_iter = used_by_c_map_.find(var);
  if (used_by_c_iter != used_by_c_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* used_by_c = used_by_c_iter->second;
    for (auto c: * used_by_c) {
      ret_list.push_back(* c);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modifies relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifies(std::string stmt) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  // For statement map
  auto modifies_s_iter = modifies_s_map_.find(stmt);
  if (modifies_s_iter != modifies_s_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* modifies_s = modifies_s_iter->second;
    for (auto v: * modifies_s) {
      ret_list.push_back(* v);
    }
  }
  // For container map
  auto modifies_c_iter = modifies_c_map_.find(stmt);
  if (modifies_c_iter != modifies_c_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* modifies_c = modifies_c_iter->second;
    for (auto v: * modifies_c) {
      ret_list.push_back(* v);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modified by relationship with the specified statement
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiedBy(std::string var) {
  std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
  // For statement map
  auto modified_by_s_iter = modified_by_s_map_.find(var);
  if (modified_by_s_iter != modified_by_s_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* modified_by_s = modified_by_s_iter->second;
    for (auto s: * modified_by_s) {
      ret_list.push_back(* s);
    }
  }
  // For container map
  auto modified_by_c_iter = modified_by_c_map_.find(var);
  if (modified_by_c_iter != modified_by_c_map_.end()) {
    std::list<std::tuple<DesignEntity, std::string>*>* modified_by_c = modified_by_c_iter->second;
    for (auto c: * modified_by_c) {
      ret_list.push_back(* c);
    }
  }
  return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the uses relationship with the specified procedure
 * @param procedure is a string representing the procedure
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsesP(std::string procedure) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto use_c_iter = use_c_map_.find(procedure);
    if (use_c_iter != use_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* use_c = use_c_iter->second;
        for (auto v: * use_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of procedures representing elements that fulfill the used by relationship with the specified variable
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and procedure name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsedByP(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto used_by_c_iter = used_by_c_map_.find(var);
    if (used_by_c_iter != used_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* used_by_c = used_by_c_iter->second;
        for (auto c: * used_by_c) {
          if (std::get<0>(*c) == DesignEntity::kProcedure) {
            ret_list.push_back(*c);
          }
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modifies relationship with the specified procedure
 * @param procedure is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiesP(std::string procedure) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto modifies_c_iter = modifies_c_map_.find(procedure);
    if (modifies_c_iter != modifies_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modifies_c = modifies_c_iter->second;
        for (auto v: * modifies_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modified by relationship with the specified variable
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and procedure name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiedByP(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto modified_by_c_iter = modified_by_c_map_.find(var);
    if (modified_by_c_iter != modified_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modified_by_c = modified_by_c_iter->second;
        for (auto c: * modified_by_c) {
          if (std::get<0>(*c) == DesignEntity::kProcedure) {
            ret_list.push_back(*c);
          }
        }
    }
    return ret_list;
}




/**
 * From an assign statement, Get its AssignEntity object
 * @param stmt_ref is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return vector containing one AssignEntity object
 */
std::vector<AssignEntity> PKB::GetPatternByAssign(std::string stmt_ref) {
  return assign_expr_map_[stmt_ref];
}

/**
 * From a variable name, Get all AssignEntity object with the variable on the LHS
 * @param var_name is a string representing the variable name (e.g. "give", "me", "A", ...)
 * @return vector containing the AssignEntity objects associated with var_name
 */
std::vector<AssignEntity> PKB::GetPatternByVariable(std::string var_name) {
  return assign_expr_map_[var_name];
}

/**
 * Populates the PKB proc_table_ and adds type info to type_map_
 * @param proc_list is a list of Procedure pointers
 * @return
 */
void PKB::PopulateProcList(const std::list<Procedure*>& proc_list) {
  proc_table_ = std::list<std::string>();
  for (auto const& i: proc_list) {
    auto* proc_name = const_cast<ProcedureName*>(i->GetName());
    proc_table_.push_back(proc_name->getName());
    type_map_[proc_name->getName()] = DesignEntity::kProcedure;
  }
}

/**
 * Populates the PKB var_table_ and adds type info to type_map_
 * @param var_list is a list of Variable pointers
 * @return
 */
void PKB::PopulateVarList(const std::list<Variable*>& var_list) {
  var_table_ = std::list<std::string>();
  for (auto const& i: var_list) {
    auto* var_name = const_cast<VariableName*>(i->GetName());
    var_table_.push_back(var_name->getName());
    type_map_[var_name->getName()] = DesignEntity::kVariable;
  }
}

/**
 * Populates the PKB const_table_ and adds type info to type_map_
 * @param const_list is a list of ConstantValue pointers
 * @return
 */
void PKB::PopulateConstList(const std::list<ConstantValue*>& const_list) {
  const_table_ = std::list<std::string>();
  for (auto const& i: const_list) {
    int const_value = i->Get();
    const_table_.push_back(std::to_string(const_value));
    type_map_[std::to_string(const_value)] = DesignEntity::kConstant;
  }
}

/**
 * Populates the PKB stmt_table_
 * @param stmt_list is a list of Statement pointers
 * @return
 */
void PKB::PopulateStmtList(const std::list<Statement*>& stmt_list) {
  stmt_table_ = std::list<std::string>();
  for (auto const& i: stmt_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    stmt_table_.push_back(std::to_string(s_number->GetNum()));
  }
}

/**
 * Populates the PKB if_table_ and adds type info to type_map_
 * @param if_list is a list of IfEntity pointers
 * @return
 */
void PKB::PopulateIfList(const std::list<IfEntity*>& if_list) {
  if_table_ = std::list<std::string>();
  for (auto const& i: if_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    if_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kIf;
  }
}

/**
 * Populates the PKB while_table_ and adds type info to type_map_
 * @param while_list is a list of WhileEntity pointers
 * @return
 */
void PKB::PopulateWhileList(const std::list<WhileEntity*>& while_list) {
  while_table_ = std::list<std::string>();
  for (auto const& i: while_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    while_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kWhile;
  }
}

/**
 * Populates the PKB assign_table_, adds type info to type_map_, and populates assign_expr_map_
 * @param assign_list is a list of AssignEntity pointers
 * @return
 */
void PKB::PopulateAssignList(const std::list<AssignEntity*>& assign_list) {
  assign_table_ = std::list<std::string>();
  for (auto const& i: assign_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    assign_table_.push_back(std::to_string(s_number->GetNum()));

    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kAssign;

    // Populate assign_expr_map_ with AssignEntity

    AssignEntity entity = * const_cast<AssignEntity*>(i);
    std::vector<AssignEntity> stmt_map_vector{entity};
    std::vector<AssignEntity> var_map_vector{entity};
    assign_expr_map_[std::to_string(s_number->GetNum())] = stmt_map_vector;

    VariableName var_name = * entity.GetVariable()->GetName();
    if (assign_expr_map_.find(var_name.getName()) != assign_expr_map_.end()) {
      assign_expr_map_[var_name.getName()].push_back(entity);
    } else {
      assign_expr_map_[var_name.getName()] = var_map_vector;
    }
  }
}

/**
 * Populates the PKB call_table_ and adds type info to type_map_
 * @param call_list is a list of CallEntity pointers
 * @return
 */
void PKB::PopulateCallList(const std::list<CallEntity*>& call_list) {
  call_table_ = std::list<std::string>();
  for (auto const& i: call_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    call_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kCall;
  }
}

/**
 * Populates the PKB print_table_ and adds type info to type_map_
 * @param print_list is a list of PrintEntity pointers
 * @return
 */
void PKB::PopulatePrintList(const std::list<PrintEntity*>& print_list) {
  print_table_ = std::list<std::string>();
  for (auto const& i: print_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    print_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kPrint;
  }
}

/**
 * Populates the PKB read_table_ and adds type info to type_map_
 * @param read_list is a list of ReadEntity pointers
 * @return
 */
void PKB::PopulateReadList(const std::list<ReadEntity*>& read_list) {
  read_table_ = std::list<std::string>();
  for (auto const& i: read_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    read_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kRead;
  }
}

/**
 * Populates the PKB follows_map_
 * @param follow_hash is a hashmap representing follows relationships between statements
 * @return
 */
void PKB::PopulateFollowsMap(const std::unordered_map<Statement*, Statement*>& follow_hash) {
  for (std::pair<Statement*, Statement*> kv: follow_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    follows_map_[k_string] = result;
  }
}

/**
 * Populates the PKB previous_map_
 * @param followed_by_hash is a hashmap representing followed by relationships between statements
 * @return
 */
void PKB::PopulatePreviousMap(const std::unordered_map<Statement*, Statement*>& followed_by_hash) {
  for (std::pair<Statement*, Statement*> kv: followed_by_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    previous_map_[k_string] = result;
  }
}

/**
 * Populates the PKB parent_map_
 * @param parent_to_child_hash is a hashmap representing parent relationships between statements
 * @return
 */
void PKB::PopulateParentMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: parent_to_child_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* children = kv.second;

    std::string stmt_ref;

    for (Statement* child: * children) {
      auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
      stmt_ref = std::to_string(childNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    parent_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB child_map_
 * @param child_to_parent_hash_ is a hashmap representing child relationships between statements
 * @return
 */
void PKB::PopulateChildMap(const std::unordered_map<Statement*, Statement*>& child_to_parent_hash_) {
  for (std::pair<Statement*, Statement*> kv: child_to_parent_hash_) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    child_map_[k_string] = result;
  }
}

/**
 * Populates the PKB follows_t_map_
 * @param follow_T_hash is a hashmap representing follows* relationships between statements
 * @return
 */
void PKB::PopulateFollowsTMap(std::unordered_map<Statement*, std::list<Statement*>*> follows_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: follows_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* followsList = kv.second;

    std::string stmt_ref;

    for (Statement* follows: * followsList) {
      auto* follows_number = const_cast<StatementNumber*>(follows->GetStatementNumber());
      stmt_ref = std::to_string(follows_number->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    follows_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB previous_t_map_
 * @param previous_T_hash is a hashmap representing previous* relationships between statements
 * @return
 */
void PKB::PopulatePreviousTMap(std::unordered_map<Statement*, std::list<Statement*>*> previous_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: previous_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* previousList = kv.second;

    std::string stmt_ref;

    for (Statement* previous: * previousList) {
      auto* prev_number = const_cast<StatementNumber*>(previous->GetStatementNumber());
      stmt_ref = std::to_string(prev_number->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    previous_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB parent_t_map_
 * @param parent_to_child_T_hash is a hashmap representing parent* relationships between statements
 * @return
 */
void PKB::PopulateParentTMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: parent_to_child_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* children = kv.second;

    std::string stmt_ref;

    for (Statement* child: * children) {
      auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
      stmt_ref = std::to_string(childNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    parent_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB child_t_map_
 * @param child_to_parent_T_hash is a hashmap representing child* relationships between statements
 * @return
 */
void PKB::PopulateChildTMap(std::unordered_map<Statement*, std::list<Statement*>*> child_to_parent_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: child_to_parent_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* parents = kv.second;

    std::string stmt_ref;

    for (Statement* parent: * parents) {
      auto* parentNumber = const_cast<StatementNumber*>(parent->GetStatementNumber());
      stmt_ref = std::to_string(parentNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    child_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB use_s_map_
 * @param use_hash is a hashmap representing use relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateUseSMap(std::unordered_map<Statement*, std::list<Variable*>*> use_hash) {
  for (std::pair<Statement*, std::list<Variable*>*> kv: use_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    use_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB used_by_s_map_
 * @param used_by_hash is a hashmap representing used by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateUsedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> used_by_hash) {
  for (std::pair<Variable*, std::list<Statement*>*> kv: used_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* using_statements = kv.second;

    for (Statement* stmt: * using_statements) {
      auto* stmt_num = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      std::string stmt_ref = std::to_string(stmt_num->GetNum());

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }
    used_by_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB use_c_map_
 * @param c_use_hash is a hashmap representing use relationships between container statements and variables
 * @return
 */
void PKB::PopulateUseCMap(std::unordered_map<Container*, std::list<Variable*>*> c_use_hash) {
  for (std::pair<Container*, std::list<Variable*>*> kv: c_use_hash) {

      std::string k_string;
      if (dynamic_cast<Statement*>(kv.first) != nullptr) {
          Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
          auto* k_number = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          k_string = std::to_string(k_number->GetNum());
      } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
          Procedure* proc = dynamic_cast<Procedure*>(kv.first);
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          k_string = k_number->getName();
      }

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    use_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB used_by_c_map_
 * @param c_used_by_hash is a hashmap representing used by relationships between variables and container statements
 * @return
 */
void PKB::PopulateUsedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_used_by_hash) {
  for (std::pair<Variable*, std::list<Container*>*> kv: c_used_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Container*>* using_statements = kv.second;

    for (Container* stmt: * using_statements) {
      std::string ref;
      if (Statement* c_stmt = dynamic_cast<Statement*>(stmt)) {
          auto* stmt_num = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          ref = std::to_string(stmt_num->GetNum());
      } else if (Procedure* proc = dynamic_cast<Procedure*>(stmt)) {
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          ref = k_number->getName();
      }

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[ref], ref);
      result_ptr->push_back(entity_ptr);
    }

    used_by_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modifies_s_map_
 * @param modifies_hash is a hashmap representing modifies relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateModifiesSMap(std::unordered_map<Statement*, std::list<Variable*>*> modifies_hash) {
  for (std::pair<Statement*, std::list<Variable*>*> kv: modifies_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    modifies_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modified_by_s_map_
 * @param modified_by_hash is a hashmap representing modified by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateModifiedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> modified_by_hash) {
  for (std::pair<Variable*, std::list<Statement*>*> kv: modified_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* using_statements = kv.second;

    for (Statement* stmt: * using_statements) {
      auto* stmt_num = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      std::string stmt_ref = std::to_string(stmt_num->GetNum());

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }
    modified_by_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modifies_c_map_
 * @param c_modifies_hash is a hashmap representing modifies relationships between container statements and variables
 * @return
 */
void PKB::PopulateModifiesCMap(std::unordered_map<Container*, std::list<Variable*>*> c_modifies_hash) {
  for (std::pair<Container*, std::list<Variable*>*> kv: c_modifies_hash) {
      std::string k_string;
      if (dynamic_cast<Statement*>(kv.first) != nullptr) {
          Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
          auto* k_number = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          k_string = std::to_string(k_number->GetNum());
      } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
          Procedure* proc = dynamic_cast<Procedure*>(kv.first);
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          k_string = k_number->getName();
      }
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    modifies_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modified_by_c_map_
 * @param c_modified_by_hash is a hashmap representing modified by relationships between variables and container statements
 * @return
 */
void PKB::PopulateModifiedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_modified_by_hash) {
  for (std::pair<Variable*, std::list<Container*>*> kv: c_modified_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Container*>* using_statements = kv.second;

    for (Container* stmt: * using_statements) {
        std::string ref;
        if (Statement* c_stmt = dynamic_cast<Statement*>(stmt)) {
            auto* stmt_num = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
            ref = std::to_string(stmt_num->GetNum());
        } else if (Procedure* proc = dynamic_cast<Procedure*>(stmt)) {
            auto* k_number = const_cast<ProcedureName*>(proc->GetName());
            ref = k_number->getName();
        }

        std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[ref], ref);
      result_ptr->push_back(entity_ptr);
    }

    modified_by_c_map_[k_string] = result_ptr;
  }
}