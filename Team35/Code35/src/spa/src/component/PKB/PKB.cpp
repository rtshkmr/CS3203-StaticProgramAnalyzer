#include "PKB.h"
#include <stdio.h>
#include <iostream>
#include <util/Logger.h>

constexpr auto L = [](auto msg){
    LOG(spa_logger << Logger::Prettify(msg));
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
bool PKB::hasFollows() {
    return !follows_map_.empty();
};

/**
 * Returns boolean of whether there exists any Previous relationship
 * @param
 * @return True if there is at least one previous relationship, otherwise False
 */
bool PKB::hasPrevious() {
    return !previous_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent relationship
 * @param
 * @return True if there is at least one parent relationship, otherwise False
 */
bool PKB::hasParent() {
    return !parent_map_.empty();
}

/**
 * Returns boolean of whether there exists any child relationship
 * @param
 * @return True if there is at least one child relationship, otherwise False
 */
bool PKB::hasChild() {
    return !child_map_.empty();
}

/**
 * Returns boolean of whether there exists any follows* relationship
 * @param
 * @return True if there is at least one follows* relationship, otherwise False
 */
bool PKB::hasFollowsT() {
    return !follows_t_map_.empty();
};

/**
 * Returns boolean of whether there exists any previous* relationship
 * @param
 * @return True if there is at least one previous* relationship, otherwise False
 */
bool PKB::hasPreviousT() {
    return !previous_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent* relationship
 * @param
 * @return True if there is at least one parent* relationship, otherwise False
 */
bool PKB::hasParentT() {
    return !parent_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any child* relationship
 * @param
 * @return True if there is at least one child* relationship, otherwise False
 */
bool PKB::hasChildT() {
    return !child_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any uses relationship
 * @param
 * @return True if there is at least one uses relationship, otherwise False
 */
bool PKB::hasUses() {
    // Must check if both container and statement maps are empty
    return !use_s_map_.empty() || !use_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any used by relationship
 * @param
 * @return True if there is at least one used by relationship, otherwise False
 */
bool PKB::hasUsedBy() {
    // Must check if both container and statement maps are empty
    return !used_by_s_map_.empty() || !used_by_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modifies relationship
 * @param
 * @return True if there is at least one modifies relationship, otherwise False
 */
bool PKB::hasModifies() {
    // Must check if both container and statement maps are empty
    return !modifies_s_map_.empty() || !modifies_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modified by relationship
 * @param
 * @return True if there is at least one modified by relationship, otherwise False
 */
bool PKB::hasModifiedBy() {
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
    auto parent_iter = parent_t_map_.find(stmt);
    if (parent_iter != parent_t_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* children = parent_iter->second;
        for (auto s : *children) {
            ret_list.push_back(*s);
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
        for (auto s : *before) {
            ret_list.push_back(*s);
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
        for (auto s : *after) {
            ret_list.push_back(*s);
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
        for (auto s : *children) {
            ret_list.push_back(*s);
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
        for (auto s : *parents) {
            ret_list.push_back(*s);
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
        for (auto v : *use_s) {
            ret_list.push_back(*v);
        }
    }
    // For container map
    auto use_c_iter = use_c_map_.find(stmt);
    if (use_c_iter != use_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* use_c = use_c_iter->second;
        for (auto v : *use_c) {
            ret_list.push_back(*v);
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
        for (auto s : *used_by_s) {
            ret_list.push_back(*s);
        }
    }
    // For container map
    auto used_by_c_iter = used_by_c_map_.find(var);
    if (used_by_c_iter != used_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* used_by_c = used_by_c_iter->second;
        for (auto c : *used_by_c) {
            ret_list.push_back(*c);
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
        for (auto v : *modifies_s) {
            ret_list.push_back(*v);
        }
    }
    // For container map
    auto modifies_c_iter = modifies_c_map_.find(stmt);
    if (modifies_c_iter != modifies_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modifies_c = modifies_c_iter->second;
        for (auto v : *modifies_c) {
            ret_list.push_back(*v);
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
        for (auto s : *modified_by_s) {
            ret_list.push_back(*s);
        }
    }
    // For container map
    auto modified_by_c_iter = modified_by_c_map_.find(var);
    if (modified_by_c_iter != modified_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modified_by_c = modified_by_c_iter->second;
        for (auto c : *modified_by_c) {
            ret_list.push_back(*c);
        }
    }
    return ret_list;
}

/**
 * From an assign statement, get its AssignEntity object
 * @param stmtRef is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return vector containing one AssignEntity object
 */
std::vector<AssignEntity> PKB::GetPatternByAssign(std::string stmtRef) {
    return assign_expr_map_[stmtRef];
}

/**
 * From a variable name, get all AssignEntity object with the variable on the LHS
 * @param varName is a string representing the variable name (e.g. "give", "me", "A", ...)
 * @return vector containing the AssignEntity objects associated with varName
 */
std::vector<AssignEntity> PKB::GetPatternByVariable(std::string varName) {
    return assign_expr_map_[varName];
}

/**
 * Populates the PKB proc_table_ and adds type info to type_map_
 * @param proc_list is a list of Procedure pointers
 * @return
 */
void PKB::PopulateProcList(const std::list<Procedure*>& proc_list) {
    proc_table_ = std::list<std::string>();
    for (auto const& i : proc_list) {
        auto* pName = const_cast<ProcedureName*>(i->GetName());
        proc_table_.push_back(pName->getName());
        type_map_[pName->getName()] = DesignEntity::kProcedure;
    }
}

/**
 * Populates the PKB var_table_ and adds type info to type_map_
 * @param var_list is a list of Variable pointers
 * @return
 */
void PKB::PopulateVarList(const std::list<Variable*>& var_list) {
    var_table_ = std::list<std::string>();
    for (auto const& i : var_list) {
        auto* vName = const_cast<VariableName*>(i->GetName());
        var_table_.push_back(vName->getName());
        type_map_[vName->getName()] = DesignEntity::kVariable;
    }
}

/**
 * Populates the PKB const_table_ and adds type info to type_map_
 * @param const_list is a list of ConstantValue pointers
 * @return
 */
void PKB::PopulateConstList(const std::list<ConstantValue*>& const_list) {
    const_table_ = std::list<std::string>();
    for (auto const& i : const_list) {
        int cValue = i->get();
        const_table_.push_back(std::to_string(cValue));
        type_map_[std::to_string(cValue)] = DesignEntity::kConstant;
    }
}

/**
 * Populates the PKB stmt_table_
 * @param stmt_list is a list of Statement pointers
 * @return
 */
void PKB::PopulateStmtList(const std::list<Statement*>& stmt_list) {
    stmt_table_ = std::list<std::string>();
    for (auto const& i : stmt_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        stmt_table_.push_back(std::to_string(sNumber->GetNum()));
    }
}

/**
 * Populates the PKB if_table_ and adds type info to type_map_
 * @param if_list is a list of IfEntity pointers
 * @return
 */
void PKB::PopulateIfList(const std::list<IfEntity*>& if_list) {
    if_table_ = std::list<std::string>();
    for (auto const& i : if_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        if_table_.push_back(std::to_string(sNumber->GetNum()));
        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kIf;
    }
}

/**
 * Populates the PKB while_table_ and adds type info to type_map_
 * @param while_list is a list of WhileEntity pointers
 * @return
 */
void PKB::PopulateWhileList(const std::list<WhileEntity*>& while_list) {
    while_table_ = std::list<std::string>();
    for (auto const& i : while_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        while_table_.push_back(std::to_string(sNumber->GetNum()));
        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kWhile;
    }
}

/**
 * Populates the PKB assign_table_, adds type info to type_map_, and populates assign_expr_map_
 * @param assign_list is a list of AssignEntity pointers
 * @return
 */
void PKB::PopulateAssignList(const std::list<AssignEntity*>& assign_list) {
    assign_table_ = std::list<std::string>();
    for (auto const& i : assign_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        assign_table_.push_back(std::to_string(sNumber->GetNum()));

        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kAssign;

        // Populate assign_expr_map_ with AssignEntity

        AssignEntity entity = *const_cast<AssignEntity*>(i);
        std::vector<AssignEntity> stmtMapVect{entity};
        std::vector<AssignEntity> varMapVect{entity};
        assign_expr_map_[std::to_string(sNumber->GetNum())] = stmtMapVect;

        VariableName varName = *entity.GetVariable()->GetName();
        if (assign_expr_map_.find(varName.getName()) != assign_expr_map_.end()) {
            assign_expr_map_[varName.getName()].push_back(entity);
        } else {
            assign_expr_map_[varName.getName()] = varMapVect;
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
    for (auto const& i : call_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        call_table_.push_back(std::to_string(sNumber->GetNum()));
        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kCall;
    }
}

/**
 * Populates the PKB print_table_ and adds type info to type_map_
 * @param print_list is a list of PrintEntity pointers
 * @return
 */
void PKB::PopulatePrintList(const std::list<PrintEntity*>& print_list) {
    print_table_ = std::list<std::string>();
    for (auto const& i : print_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        print_table_.push_back(std::to_string(sNumber->GetNum()));
        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kPrint;
    }
}

/**
 * Populates the PKB read_table_ and adds type info to type_map_
 * @param read_list is a list of ReadEntity pointers
 * @return
 */
void PKB::PopulateReadList(const std::list<ReadEntity*>& read_list) {
    read_table_ = std::list<std::string>();
    for (auto const& i : read_list) {
        auto* sNumber = const_cast<StatementNumber*>(i->GetStatementNumber());
        read_table_.push_back(std::to_string(sNumber->GetNum()));
        type_map_[std::to_string(sNumber->GetNum())] = DesignEntity::kRead;
    }
}

/**
 * Populates the PKB follows_map_
 * @param follow_hash is a hashmap representing follows relationships between statements
 * @return
 */
void PKB::PopulateFollowsMap(const std::unordered_map<Statement*, Statement*>& follow_hash) {
    for (std::pair<Statement*, Statement*> kv : follow_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto* vNumber = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->GetNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        follows_map_[kString] = result;
    }
}

/**
 * Populates the PKB previous_map_
 * @param followed_by_hash is a hashmap representing followed by relationships between statements
 * @return
 */
void PKB::PopulatePreviousMap(const std::unordered_map<Statement*, Statement*>& followed_by_hash) {
    for (std::pair<Statement*, Statement*> kv : followed_by_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto* vNumber = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->GetNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        previous_map_[kString] = result;
    }
}

/**
 * Populates the PKB parent_map_
 * @param parent_to_child_hash is a hashmap representing parent relationships between statements
 * @return
 */
void PKB::PopulateParentMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_hash) {
    for (std::pair<Statement*, std::list<Statement*>*> kv : parent_to_child_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* children = kv.second;

        std::string stmtRef;

        for (Statement* child : *children) {
            auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
            stmtRef = std::to_string(childNumber->GetNum());
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        parent_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB child_map_
 * @param child_to_parent_hash_ is a hashmap representing child relationships between statements
 * @return
 */
void PKB::PopulateChildMap(const std::unordered_map<Statement*, Statement*>& child_to_parent_hash_) {
    for (std::pair<Statement*, Statement*> kv : child_to_parent_hash_) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto* vNumber = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->GetNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        child_map_[kString] = result;
    }
}

/**
 * Populates the PKB follows_t_map_
 * @param follow_T_hash is a hashmap representing follows* relationships between statements
 * @return
 */
void PKB::PopulateFollowsTMap(std::unordered_map<Statement*, std::list<Statement*>*> follows_T_hash) {
    for (std::pair<Statement*, std::list<Statement*>*> kv : follows_T_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* followsList = kv.second;

        std::string stmtRef;

        for (Statement* follows : *followsList) {
            auto* followsNumber = const_cast<StatementNumber*>(follows->GetStatementNumber());
            stmtRef = std::to_string(followsNumber->GetNum());
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        follows_t_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB previous_t_map_
 * @param previous_T_hash is a hashmap representing previous* relationships between statements
 * @return
 */
void PKB::PopulatePreviousTMap(std::unordered_map<Statement*, std::list<Statement*>*> previous_T_hash) {
    for (std::pair<Statement*, std::list<Statement*>*> kv : previous_T_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* previousList = kv.second;

        std::string stmtRef;

        for (Statement* previous : *previousList) {
            auto* prevNumber = const_cast<StatementNumber*>(previous->GetStatementNumber());
            stmtRef = std::to_string(prevNumber->GetNum());
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        previous_t_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB parent_t_map_
 * @param parent_to_child_T_hash is a hashmap representing parent* relationships between statements
 * @return
 */
void PKB::PopulateParentTMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_T_hash) {
    for (std::pair<Statement*, std::list<Statement*>*> kv : parent_to_child_T_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* children = kv.second;

        std::string stmtRef;

        for (Statement* child : *children) {
            auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
            stmtRef = std::to_string(childNumber->GetNum());
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        parent_t_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB child_t_map_
 * @param child_to_parent_T_hash is a hashmap representing child* relationships between statements
 * @return
 */
void PKB::PopulateChildTMap(std::unordered_map<Statement*, std::list<Statement*>*> child_to_parent_T_hash) {
    for (std::pair<Statement*, std::list<Statement*>*> kv : child_to_parent_T_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* parents = kv.second;

        std::string stmtRef;

        for (Statement* parent : *parents) {
            auto* parentNumber = const_cast<StatementNumber*>(parent->GetStatementNumber());
            stmtRef = std::to_string(parentNumber->GetNum());
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        child_t_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB use_s_map_
 * @param use_hash is a hashmap representing use relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateUseSMap(std::unordered_map<Statement*, std::list<Variable*>*> use_hash) {
    for (std::pair<Statement*, std::list<Variable*>*> kv : use_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Variable*>* used_variables = kv.second;

        std::string varString;

        for (Variable* var : *used_variables) {
            VariableName* varName = const_cast<VariableName*>(var->GetName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, varString);
            result_ptr->push_back(entity_ptr);
        }

        use_s_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB used_by_s_map_
 * @param used_by_hash is a hashmap representing used by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateUsedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> used_by_hash) {
    for (std::pair<Variable*, std::list<Statement*>*> kv : used_by_hash) {

        VariableName* varName = const_cast<VariableName*>(kv.first->GetName());
        std::string kString = varName->getName();

        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* using_statements = kv.second;

        for (Statement* stmt : *using_statements) {
            auto* stmtNum = const_cast<StatementNumber*>(stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->GetNum());

            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }
        used_by_s_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB use_c_map_
 * @param c_use_hash is a hashmap representing use relationships between container statements and variables
 * @return
 */
void PKB::PopulateUseCMap(std::unordered_map<Container*, std::list<Variable*>*> c_use_hash) {
    for (std::pair<Container*, std::list<Variable*>*> kv : c_use_hash) {
        Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
        auto* kNumber = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());

        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Variable*>* used_variables = kv.second;

        std::string varString;

        for (Variable* var : *used_variables) {
            VariableName* varName = const_cast<VariableName*>(var->GetName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, varString);
            result_ptr->push_back(entity_ptr);
        }

        use_c_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB used_by_c_map_
 * @param c_used_by_hash is a hashmap representing used by relationships between variables and container statements
 * @return
 */
void PKB::PopulateUsedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_used_by_hash) {
    for (std::pair<Variable*, std::list<Container*>*> kv : c_used_by_hash) {

        VariableName* varName = const_cast<VariableName*>(kv.first->GetName());
        std::string kString = varName->getName();

        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Container*>* using_statements = kv.second;

        for (Container* stmt : *using_statements) {
            Statement* c_stmt = dynamic_cast<Statement*>(stmt);
            auto* stmtNum = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->GetNum());

            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        used_by_c_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB modifies_s_map_
 * @param modifies_hash is a hashmap representing modifies relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateModifiesSMap(std::unordered_map<Statement*, std::list<Variable*>*> modifies_hash) {
    for (std::pair<Statement*, std::list<Variable*>*> kv : modifies_hash) {
        auto* kNumber = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Variable*>* used_variables = kv.second;

        std::string varString;

        for (Variable* var : *used_variables) {
            VariableName* varName = const_cast<VariableName*>(var->GetName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, varString);
            result_ptr->push_back(entity_ptr);
        }

        modifies_s_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB modified_by_s_map_
 * @param modified_by_hash is a hashmap representing modified by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateModifiedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> modified_by_hash) {
    for (std::pair<Variable*, std::list<Statement*>*> kv : modified_by_hash) {

        VariableName* varName = const_cast<VariableName*>(kv.first->GetName());
        std::string kString = varName->getName();

        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Statement*>* using_statements = kv.second;

        for (Statement* stmt : *using_statements) {
            auto* stmtNum = const_cast<StatementNumber*>(stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->GetNum());

            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }
        modified_by_s_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB modifies_c_map_
 * @param c_modifies_hash is a hashmap representing modifies relationships between container statements and variables
 * @return
 */
void PKB::PopulateModifiesCMap(std::unordered_map<Container*, std::list<Variable*>*> c_modifies_hash) {
//    for (std::pair<Container *, std::list<Variable *> *> kv : c_modifies_hash) {
    for (std::pair<Container*, std::list<Variable*>*> kv : c_modifies_hash) {
        Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
        auto* kNumber = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
        std::string kString = std::to_string(kNumber->GetNum());
        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Variable*>* used_variables = kv.second;

        std::string varString;

        for (Variable* var : *used_variables) {
            VariableName* varName = const_cast<VariableName*>(var->GetName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, varString);
            result_ptr->push_back(entity_ptr);
        }

        modifies_c_map_[kString] = result_ptr;
    }
}

/**
 * Populates the PKB modified_by_c_map_
 * @param c_modified_by_hash is a hashmap representing modified by relationships between variables and container statements
 * @return
 */
void PKB::PopulateModifiedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_modified_by_hash) {
    for (std::pair<Variable*, std::list<Container*>*> kv : c_modified_by_hash) {

        VariableName* varName = const_cast<VariableName*>(kv.first->GetName());
        std::string kString = varName->getName();

        auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

        std::list<Container*>* using_statements = kv.second;

        for (Container* stmt : *using_statements) {
            Statement* c_stmt = dynamic_cast<Statement*>(stmt);
            auto* stmtNum = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->GetNum());

            std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmtRef], stmtRef);
            result_ptr->push_back(entity_ptr);
        }

        modified_by_c_map_[kString] = result_ptr;
    }
}