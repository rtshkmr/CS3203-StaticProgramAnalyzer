#include <stdio.h>
#include <iostream>

#include "PKB.h"

std::list<std::string> PKB::GetDesignEntity(DesignEntity de) {
    std::list<std::string> result = std::list<std::string>();
    switch (de) {
        case DesignEntity::kStmt:return stmt_table_;
            break;
        case DesignEntity::kRead:return read_table_;
            break;
        case DesignEntity::kPrint:return print_table_;
            break;
        case DesignEntity::kCall:return call_table_;
            break;
        case DesignEntity::kWhile:return while_table_;
            break;
        case DesignEntity::kIf:return if_table_;
            break;
        case DesignEntity::kAssign:return assign_table_;
            break;
        case DesignEntity::kVariable:return var_table_;
            break;
        case DesignEntity::kConstant:return const_table_;
            break;
        case DesignEntity::kProcedure:return proc_table_;
            break;
        case DesignEntity::kInvalid:return {};
    }
}

void PKB::PopulateDataStructures(Deliverable d) {

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

    PopulateParentMap(d.parent_hash_);
    PopulateChildMap(d.parent_of_hash_);

    PopulateUseSMap(d.use_hash_);
    PopulateUsedBySMap(d.used_by_hash_);
    PopulateUseCMap(d.container_use_hash_);
    PopulateUsedByCMap(d.container_used_by_hash_);
    PopulateModifiesSMap(d.modifies_hash_);
    PopulateModifiedBySMap(d.modified_by_hash_);
    PopulateModifiesCMap(d.container_modifies_hash_);
    PopulateModifiedByCMap(d.container_modified_by_hash_);
}

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

std::list<std::tuple<DesignEntity,std::string>> PKB::GetPrevious(std::string stmt) {
    std::list<std::tuple<DesignEntity,std::string>> ret_list = std::list<std::tuple<DesignEntity,std::string>>();
    auto previous_iter = previous_map_.find(stmt);
    std::tuple<DesignEntity,std::string> previous = previous_iter->second;
    ret_list.push_back(previous);
    return ret_list;
}

std::list<std::tuple<DesignEntity, std::string>> PKB::GetParent(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto parent_iter = parent_map_.find(stmt);
    if (parent_iter == parent_map_.end()) {
        return ret_list;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> parent = parent_iter->second;
        return parent;
    }
}

std::list<std::tuple<DesignEntity,std::string>> PKB::GetChild(std::string stmt) {
    std::list<std::tuple<DesignEntity,std::string>> ret_list = std::list<std::tuple<DesignEntity,std::string>>();
    auto child_iter = child_map_.find(stmt);
    std::tuple<DesignEntity,std::string> child = child_iter->second;
    ret_list.push_back(child);
    return ret_list;
}

std::list<std::tuple<DesignEntity, std::string>> PKB::GetUses(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto use_s_iter = use_s_map_.find(stmt);
    if (use_s_iter == use_s_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> use_s = use_s_iter->second;
        ret_list.insert(ret_list.end(), use_s.begin(), use_s.end());
    }
    auto use_c_iter = use_c_map_.find(stmt);
    if (use_c_iter == use_c_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> use_c = use_c_iter->second;
        ret_list.insert(ret_list.end(), use_c.begin(), use_c.end());
    }
    return ret_list;
}

std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsedBy(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto used_by_s_iter = used_by_s_map_.find(stmt);
    if (used_by_s_iter == used_by_s_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> used_by_s = used_by_s_iter->second;
        ret_list.insert(ret_list.end(), used_by_s.begin(), used_by_s.end());
    }
    auto used_by_c_iter = used_by_c_map_.find(stmt);
    if (used_by_c_iter == used_by_c_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> used_by_c = used_by_c_iter->second;
        ret_list.insert(ret_list.end(), used_by_c.begin(), used_by_c.end());
    }
    return ret_list;
}

std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifies(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto modifies_s_iter = modifies_s_map_.find(stmt);
    if (modifies_s_iter == modifies_s_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> modifies_s = modifies_s_iter->second;
        ret_list.insert(ret_list.end(), modifies_s.begin(), modifies_s.end());
    }
    auto modifies_c_iter = modifies_c_map_.find(stmt);
    if (modifies_c_iter == modifies_c_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> modifies_c = modifies_c_iter->second;
        ret_list.insert(ret_list.end(), modifies_c.begin(), modifies_c.end());
    }
    return ret_list;
}

std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiedBy(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto modified_by_s_iter = modified_by_s_map_.find(stmt);
    if (modified_by_s_iter == modified_by_s_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> modified_by_s = modified_by_s_iter->second;
        ret_list.insert(ret_list.end(), modified_by_s.begin(), modified_by_s.end());
    }
    auto modified_by_c_iter = modified_by_c_map_.find(stmt);
    if (modified_by_c_iter == modified_by_c_map_.end()) {
        ;
    } else {
        std::list<std::tuple<DesignEntity, std::string>> modified_by_c = modified_by_c_iter->second;
        ret_list.insert(ret_list.end(), modified_by_c.begin(), modified_by_c.end());
    }
    return ret_list;
}

void PKB::PopulateProcList(const std::list<Procedure *> &proc_list) {
    proc_table_ = std::list<std::string>();
    for (auto const &i : proc_list) {
        auto *pName = const_cast<ProcedureName *>(i->getName());
        proc_table_.push_back(pName->getName());
        type_map_[pName->getName()] = DesignEntity::kProcedure;
    }
}

void PKB::PopulateVarList(const std::list<Variable *> &var_list) {
    var_table_ = std::list<std::string>();
    for (auto const &i : var_list) {
        auto *vName = const_cast<VariableName *>(i->getName());
        var_table_.push_back(vName->getName());
        type_map_[vName->getName()] = DesignEntity::kVariable;
    }
}

void PKB::PopulateConstList(const std::list<ConstantValue *> &const_list) {
    const_table_ = std::list<std::string>();
    for (auto const &i : const_list) {
        int cValue = i->get();
        const_table_.push_back(std::to_string(cValue));
        type_map_[std::to_string(cValue)] = DesignEntity::kConstant;
    }
}

void PKB::PopulateStmtList(const std::list<Statement *> &stmt_list) {
    stmt_table_ = std::list<std::string>();
    for (auto const &i : stmt_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        stmt_table_.push_back(std::to_string(sNumber->getNum()));
    }
}

void PKB::PopulateIfList(const std::list<IfEntity *> &if_list) {
    if_table_ = std::list<std::string>();
    for (auto const &i : if_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        if_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kIf;
    }
}

void PKB::PopulateWhileList(const std::list<WhileEntity *> &while_list) {
    while_table_ = std::list<std::string>();
    for (auto const &i : while_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        while_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kWhile;
    }
}

void PKB::PopulateAssignList(const std::list<AssignEntity *> &assign_list) {
    assign_table_ = std::list<std::string>();
    for (auto const &i : assign_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        assign_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kAssign;
    }
}

void PKB::PopulateCallList(const std::list<CallEntity *> &call_list) {
    call_table_ = std::list<std::string>();
    for (auto const &i : call_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        call_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kCall;
    }
}

void PKB::PopulatePrintList(const std::list<PrintEntity *> &print_list) {
    print_table_ = std::list<std::string>();
    for (auto const &i : print_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        print_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kPrint;
    }
}

void PKB::PopulateReadList(const std::list<ReadEntity *> &read_list) {
    read_table_ = std::list<std::string>();
    for (auto const &i : read_list) {
        auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
        read_table_.push_back(std::to_string(sNumber->getNum()));
        type_map_[std::to_string(sNumber->getNum())] = DesignEntity::kRead;
    }
}

void PKB::PopulateFollowsMap(const std::unordered_map<Statement *, Statement *> &follow_hash) {
    for (std::pair<Statement *, Statement *> kv : follow_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto *vNumber = const_cast<StatementNumber *>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->getNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        follows_map_[kString] = result;
    }
}

void PKB::PopulatePreviousMap(const std::unordered_map<Statement *, Statement *> &followed_by_hash) {
    for (std::pair<Statement *, Statement *> kv : followed_by_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto *vNumber = const_cast<StatementNumber *>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->getNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        previous_map_[kString] = result;
    }
}

void PKB::PopulateParentMap(std::unordered_map<Statement *, std::list<Statement *> *> parent_hash) {
    for (std::pair<Statement *, std::list<Statement *> *> kv : parent_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Statement *> *children = kv.second;

        std::string cString;

        for (Statement *child : *children) {
            cString = std::to_string(child->GetStatementNumber()->getNum());
            std::tuple<DesignEntity, std::string> entity = make_tuple(type_map_[cString], cString);
            result.push_back(entity);
        }

        parent_map_[kString] = result;
    }
}

void PKB::PopulateChildMap(const std::unordered_map<Statement *, Statement *> &parent_of_hash) {
    for (std::pair<Statement *, Statement *> kv : parent_of_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto *vNumber = const_cast<StatementNumber *>(kv.second->GetStatementNumber());
        std::string vString = std::to_string(vNumber->getNum());
        DesignEntity vType = type_map_.find(vString)->second;
        std::tuple<DesignEntity, std::string> result = make_tuple(vType, vString);
        child_map_[kString] = result;
    }
}

void PKB::PopulateUseSMap(std::unordered_map<Statement *, std::list<Variable *> *> use_hash) {
    for (std::pair<Statement *, std::list<Variable *> *> kv : use_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Variable *> *used_variables = kv.second;

        std::string varString;

        for (Variable *var : *used_variables) {
            VariableName *varName = const_cast<VariableName*>(var->getName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string> entity = make_tuple(DesignEntity::kVariable, varString);
            result.push_back(entity);
        }

        use_s_map_[kString] = result;
    }
}

void PKB::PopulateUsedBySMap(std::unordered_map<Variable *, std::list<Statement *> *> used_by_hash) {
    for (std::pair<Variable *, std::list<Statement *> *> kv : used_by_hash) {

        VariableName *varName = const_cast<VariableName*>(kv.first->getName());
        std::string kString = varName->getName();

        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Statement *> *using_statements = kv.second;

        for (Statement *stmt : *using_statements) {
            auto *stmtNum = const_cast<StatementNumber *>(stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->getNum());

            std::tuple<DesignEntity, std::string> entity = make_tuple(type_map_[stmtRef], stmtRef);
            result.push_back(entity);
        }

        used_by_s_map_[kString] = result;
    }
}

void PKB::PopulateUseCMap(std::unordered_map<Container *, std::list<Variable *> *> c_use_hash) {
    for (std::pair<Container *, std::list<Variable *> *> kv : c_use_hash) {
        Statement *c_stmt = (Statement *) kv.first;
        auto *kNumber = const_cast<StatementNumber *>(c_stmt->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Variable *> *used_variables = kv.second;

        std::string varString;

        for (Variable *var : *used_variables) {
            VariableName *varName = const_cast<VariableName*>(var->getName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string> entity = make_tuple(DesignEntity::kVariable, varString);
            result.push_back(entity);
        }

        use_c_map_[kString] = result;
    }
}

void PKB::PopulateUsedByCMap(std::unordered_map<Variable *, std::list<Container *> *> c_used_by_hash) {
    for (std::pair<Variable *, std::list<Container *> *> kv : c_used_by_hash) {

        VariableName *varName = const_cast<VariableName*>(kv.first->getName());
        std::string kString = varName->getName();

        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Container *> *using_statements = kv.second;

        for (Container *stmt : *using_statements) {
            Statement *c_stmt = (Statement *) stmt;
            auto *stmtNum = const_cast<StatementNumber *>(c_stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->getNum());

            std::tuple<DesignEntity, std::string> entity = make_tuple(type_map_[stmtRef], stmtRef);
            result.push_back(entity);
        }

        used_by_c_map_[kString] = result;
    }
}


void PKB::PopulateModifiesSMap(std::unordered_map<Statement *, std::list<Variable *> *> modifies_hash) {
    for (std::pair<Statement *, std::list<Variable *> *> kv : modifies_hash) {
        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Variable *> *used_variables = kv.second;

        std::string varString;

        for (Variable *var : *used_variables) {
            VariableName *varName = const_cast<VariableName*>(var->getName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string> entity = make_tuple(DesignEntity::kVariable, varString);
            result.push_back(entity);
        }

        modifies_s_map_[kString] = result;
    }
}

void PKB::PopulateModifiedBySMap(std::unordered_map<Variable *, std::list<Statement *> *> modified_by_hash) {
    for (std::pair<Variable *, std::list<Statement *> *> kv : modified_by_hash) {

        VariableName *varName = const_cast<VariableName*>(kv.first->getName());
        std::string kString = varName->getName();

        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Statement *> *using_statements = kv.second;

        for (Statement *stmt : *using_statements) {
            auto *stmtNum = const_cast<StatementNumber *>(stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->getNum());

            std::tuple<DesignEntity, std::string> entity = make_tuple(type_map_[stmtRef], stmtRef);
            result.push_back(entity);
        }

        modified_by_s_map_[kString] = result;
    }
}

void PKB::PopulateModifiesCMap(std::unordered_map<Container *, std::list<Variable *> *> c_modifies_hash) {
    for (std::pair<Container *, std::list<Variable *> *> kv : c_modifies_hash) {
        Statement *c_stmt = (Statement *) kv.first;
        auto *kNumber = const_cast<StatementNumber *>(c_stmt->GetStatementNumber());
        std::string kString = std::to_string(kNumber->getNum());
        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Variable *> *used_variables = kv.second;

        std::string varString;

        for (Variable *var : *used_variables) {
            VariableName *varName = const_cast<VariableName*>(var->getName());
            varString = varName->getName();
            std::tuple<DesignEntity, std::string> entity = make_tuple(DesignEntity::kVariable, varString);
            result.push_back(entity);
        }

        modifies_c_map_[kString] = result;
    }
}

void PKB::PopulateModifiedByCMap(std::unordered_map<Variable *, std::list<Container *> *> c_modified_by_hash) {
    for (std::pair<Variable *, std::list<Container *> *> kv : c_modified_by_hash) {

        VariableName *varName = const_cast<VariableName*>(kv.first->getName());
        std::string kString = varName->getName();

        auto result = std::list<std::tuple<DesignEntity, std::string>>();

        std::list<Container *> *using_statements = kv.second;

        for (Container *stmt : *using_statements) {
            Statement *c_stmt = (Statement *) stmt;
            auto *stmtNum = const_cast<StatementNumber *>(c_stmt->GetStatementNumber());
            std::string stmtRef = std::to_string(stmtNum->getNum());

            std::tuple<DesignEntity, std::string> entity = make_tuple(type_map_[stmtRef], stmtRef);
            result.push_back(entity);
        }

        modified_by_c_map_[kString] = result;
    }
}