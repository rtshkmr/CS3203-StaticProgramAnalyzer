#include <stdio.h>
#include <iostream>
//#include <string>
//#include <vector>
//#include <component/QueryProcessor/types/Types.h>

using namespace std;

#include "PKB.h"

list<string> PKB::GetDesignEntity(DesignEntity de) {
  list<string> result = list<string>();
  switch(de) {
    case DesignEntity::kStmt:
      return stmt_table_;
      break;
      case DesignEntity::kRead:
        return read_table_;
        break;
        case DesignEntity::kPrint:
          return print_table_;
          break;
          case DesignEntity::kCall:
            return call_table_;
            break;
            case DesignEntity::kWhile:
              return while_table_;
              break;
              case DesignEntity::kIf:
                return if_table_;
                break;
                case DesignEntity::kAssign:
                  return assign_table_;
                  break;
                  case DesignEntity::kVariable:
                    return var_table_;
                    break;
                    case DesignEntity::kConstant:
                      return const_table_;
                      break;
                      case DesignEntity::kProcedure:
                        return proc_table_;
                        break;
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
  //    PopulateFollowedByMap(d.follow_hash_);

  PopulateParentMap(d.parent_hash_);
  //    PopulateChildMap(d.parent_hash_);

  //    PopulateUseMap(d.use_hash_);
  //    PopulateUsedByMap(d.use_hash_);
  //
  //    PopulateModifiesMap(d.modifies_hash_);
  //    PopulateModifiedByMap(d.modifies_hash_);
}

list<tuple<DesignEntity,string>> PKB::GetFollows(string stmt) {
  list<tuple<DesignEntity,string>> ret_list = list<tuple<DesignEntity,string>>();
  auto follows_iter = follows_map_.find(stmt);
  if ( follows_iter == follows_map_.end() ) {
    return ret_list;
  }
  else {
    tuple<DesignEntity,string> follows = follows_iter->second;
    ret_list.push_back(follows);
    return ret_list;
  }
}

//list<tuple<DesignEntity,string>> PKB::GetPrevious(string stmt) {
//    list<tuple<DesignEntity,string>> ret_list = list<tuple<DesignEntity,string>>();
//    auto previous_iter = previous_map_.find(stmt);
//    tuple<DesignEntity,string> previous = previous_iter->second;
//    ret_list.push_back(previous);
//    return ret_list;
//}

list<tuple<DesignEntity,string>> PKB::GetParent(string stmt) {
  list<tuple<DesignEntity,string>> ret_list = list<tuple<DesignEntity,string>>();
  auto parent_iter = parent_map_.find(stmt);
  if ( parent_iter == parent_map_.end() ) {
    return ret_list;
  }
  else {
    list<tuple<DesignEntity,string>> parent = parent_iter->second;
    return parent;
  }
}

//list<tuple<DesignEntity,string>> PKB::GetChild(string stmt) {
//    list<tuple<DesignEntity,string>> ret_list = list<tuple<DesignEntity,string>>();
//    auto child_iter = child_map_.find(stmt);
//    tuple<DesignEntity,string> child = child_iter->second;
//    ret_list.push_back(child);
//    return ret_list;
//}

void PKB::PopulateProcList(const list<Procedure *>& proc_list) {
  proc_table_ = list<string>();
  for (auto const& i : proc_list) {
    auto *pName = const_cast<ProcedureName *>(i->getName());
    proc_table_.push_back(pName->getName());
    type_map_[pName->getName()] = DesignEntity::kProcedure;
  }
}

void PKB::PopulateVarList(const list<Variable *>& var_list) {
  var_table_ = list<string>();
  for (auto const& i : var_list) {
    auto *vName = const_cast<VariableName *>(i->getName());
    var_table_.push_back(vName->getName());
    type_map_[vName->getName()] = DesignEntity::kVariable;
  }
}

void PKB::PopulateConstList(const list<ConstantValue *>& const_list) {
  const_table_ = list<string>();
  for (auto const& i : const_list) {
    int cValue = i->get();
    const_table_.push_back(to_string(cValue));
    type_map_[to_string(cValue)] = DesignEntity::kConstant;
  }
}

void PKB::PopulateStmtList(const list<Statement *>& stmt_list) {
  stmt_table_ = list<string>();
  for (auto const& i : stmt_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    stmt_table_.push_back(to_string(sNumber->getNum()));
  }
}

void PKB::PopulateIfList(const list<IfEntity *>& if_list) {
  if_table_ = list<string>();
  for (auto const& i : if_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    if_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kIf;
  }
}

void PKB::PopulateWhileList(const list<WhileEntity *>& while_list) {
  while_table_ = list<string>();
  for (auto const& i : while_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    while_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kWhile;
  }
}

void PKB::PopulateAssignList(const list<AssignEntity *>& assign_list) {
  assign_table_ = list<string>();
  for (auto const& i : assign_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    assign_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kAssign;
  }
}

void PKB::PopulateCallList(const list<CallEntity *>& call_list) {
  call_table_ = list<string>();
  for (auto const& i : call_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    call_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kCall;
  }
}

void PKB::PopulatePrintList(const list<PrintEntity *>& print_list) {
  print_table_ = list<string>();
  for (auto const& i : print_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    print_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kPrint;
  }
}

void PKB::PopulateReadList(const list<ReadEntity *>& read_list) {
  read_table_ = list<string>();
  for (auto const& i : read_list) {
    auto *sNumber = const_cast<StatementNumber *>(i->GetStatementNumber());
    read_table_.push_back(to_string(sNumber->getNum()));
    type_map_[to_string(sNumber->getNum())] = DesignEntity::kRead;
  }
}

void PKB::PopulateFollowsMap(const unordered_map<Statement *, Statement *>& follow_hash) {
  for (pair<Statement *, Statement *> kv : follow_hash)
  {
    auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
    string kString = to_string(kNumber->getNum());
    auto *vNumber = const_cast<StatementNumber *>(kv.second->GetStatementNumber());
    string vString = to_string(vNumber->getNum());
    DesignEntity vType = type_map_.find(vString)->second;
    tuple<DesignEntity,string> result = make_tuple(vType, vString);
    follows_map_[kString] = result;
  }
}

void PKB::PopulateParentMap(const unordered_map<Statement *, list<Statement *>>& parent_hash) {
  for (pair<Statement *, list<Statement *>> kv : parent_hash)
  {
    auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
    string kString = to_string(kNumber->getNum());
    auto result = list<tuple<DesignEntity,string>>();

    list<Statement *> children = kv.second;
    string cString;

    for (Statement *child : children) {
      cString = to_string(child->GetStatementNumber()->getNum());
      tuple<DesignEntity,string> entity = make_tuple(type_map_[cString], cString);
      result.push_back(entity);
    }

    parent_map_[kString] = result;
  }
}

//void PKB::PopulateUseMap(const unordered_map<Statement *, Entity *>& use_hash) {
//    for (pair<Statement *, Entity *> kv : use_hash)
//    {
//        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
//        string kString = to_string(kNumber->getNum());
//        Variable* var = dynamic_cast<Variable*>(kv.second);
//        VariableName* vName = const_cast<VariableName *>(var->getName());
//        string vString = vName->getName();
//        tuple<DesignEntity,string> result = make_tuple(DesignEntity::kVariable, vString);
//        use_map_[kString] = result;
//    }
//}

//void PKB::PopulateModifiesMap(const unordered_map<Statement *, Entity *>& modifies_hash) {
//    for (pair<Statement *, Entity *> kv : modifies_hash)
//    {
//        auto *kNumber = const_cast<StatementNumber *>(kv.first->GetStatementNumber());
//        string kString = to_string(kNumber->getNum());
//        Variable* var = dynamic_cast<Variable*>(kv.second);
//        VariableName* vName = const_cast<VariableName *>(var->getName());
//        string vString = vName->getName();
//        tuple<DesignEntity,string> result = make_tuple(DesignEntity::kVariable, vString);
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

