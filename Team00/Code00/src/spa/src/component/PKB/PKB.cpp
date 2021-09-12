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

    //    PopulateUseMap(d.use_hash_);
    //    PopulateUsedByMap(d.use_hash_);
    //
    //    PopulateModifiesMap(d.modifies_hash_);
    //    PopulateModifiedByMap(d.modifies_hash_);
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

//void PKB::PopulateUseMap(const unordered_map<Statement *, Entity *>& use_hash) {
//    for (std::pair<Statement *, Entity *> kv : use_hash)
//    {
//        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
//        std::string kString = std::to_string(kNumber->getNum());
//        Variable* var = dynamic_cast<Variable*>(kv.second);
//        VariableName* vName = const_cast<VariableName *>(var->getName());
//        std::string vString = vName->getName();
//        std::tuple<DesignEntity,std::string> result = make_tuple(DesignEntity::kVariable, vString);
//        use_map_[kString] = result;
//    }
//}

//void PKB::PopulateModifiesMap(const unordered_map<Statement *, Entity *>& modifies_hash) {
//    for (std::pair<Statement *, Entity *> kv : modifies_hash)
//    {
//        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
//        std::string kString = std::to_string(kNumber->getNum());
//        Variable* var = dynamic_cast<Variable*>(kv.second);
//        VariableName* vName = const_cast<VariableName *>(var->getName());
//        std::string vString = vName->getName();
//        std::tuple<DesignEntity,std::string> result = make_tuple(DesignEntity::kVariable, vString);
//        modifies_map_[kString] = result;
//    }
//}

//void PKB::PopulateFollowedByMap(const unordered_map<Statement *, Statement *>& follow_hash) {
//
//}
//void PKB::PopulateChildMap(const unordered_map<Statement *, Statement *>& parent_hash) {
//
//}
//void PKB::PopulateUsedByMap(const unordered_map<Statement *, Entity *>& use_hash) {
//
//}
//void PKB::PopulateModifiedByMap(const unordered_map<Statement *, Entity *>& modifies_hash) {
//
//}

