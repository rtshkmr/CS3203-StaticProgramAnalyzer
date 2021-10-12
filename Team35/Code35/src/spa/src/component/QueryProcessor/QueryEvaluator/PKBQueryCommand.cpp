#include "PKBQueryCommand.h"

PKBQueryReceiver::PKBQueryReceiver(PKB *pkb) : pkb(pkb) {}

PKBRelRefs PKBQueryCommand::GetPKBRelRef(RelRef relation) {
  switch(relation) {
    case RelRef::kFollows:
      return PKBRelRefs::kFollows;
      case RelRef::kParentT:
        return PKBRelRefs::kParentT;
        case RelRef::kParent:
          return PKBRelRefs::kParent;
//          case RelRef::kModifiesP:
//            return PKBRelRefs::kModifiesP;
//            case RelRef::kModifiesS:
//              return PKBRelRefs::kModifiesS;
              case RelRef::kUsesS:
                return PKBRelRefs::kUsesS;
//                case RelRef::kUsesP:
//                  return PKBRelRefs::kUsesP;
                  case RelRef::kFollowsT:
                    return PKBRelRefs::kFollowsT;
                    case RelRef::kCalls:
                      return PKBRelRefs::kCalls;
//                      case RelRef::kCallsT:
//                        return PKBRelRefs::kCallsT;
//                        case RelRef::kNext:
//                          return PKBRelRefs::kNext;
//                          case RelRef::kNextT:
//                            return PKBRelRefs::kNextT;
//                            case RelRef::kAffects:
//                              return PKBRelRefs::kAffects;
//                              case RelRef::kAffectsT:
//                                return PKBRelRefs::kAffectsT;
//                                case RelRef::kWildcard:
//                                  return PKBRelRefs::kWildcard;
//                                  default:
//                                    return PKBRelRefs::kWildcard
  }
}

//Works for Two synonyms
IntermediateTable* PKBQueryReceiver::QueryPKBTwoSynonyms(PKBRelRefs rel, DesignEntity first_synonym, DesignEntity second_synonym) {
  std::vector<std::tuple<Entity *, Entity *>> output = pkb->GetRelationshipByTypes(rel, first_synonym, second_synonym);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

// Works for One Synonym or no synonym
IntermediateTable *PKBQueryReceiver::QueryPKBByValue(PKBRelRefs rel, std::string value) {
  std::vector<Entity *> output = pkb->GetRelationship(rel, value);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

// Works for wildcards
IntermediateTable *PKBQueryReceiver::QueryRelRefExistence(PKBRelRefs rel) {
  bool output = pkb->HasRelationship(rel);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

IntermediateTable *PKBQueryReceiver::QueryDesignEntity(DesignEntity design_entity) {
  IntermediateTable *table = new IntermediateTable();
  switch(design_entity) {
    case DesignEntity::kAssign:
      table->InsertData(pkb->GetDesignEntities(DesignEntity::kAssign));
      break;
      case DesignEntity::kWhile:
        table->InsertData(pkb->GetDesignEntities(DesignEntity::kWhile));
        break;
        case DesignEntity::kIf:
          table->InsertData(pkb->GetDesignEntities(DesignEntity::kIf));
          break;
          default:
            break;
  }
  return table;

}

IntermediateTable *PKBQueryReceiver::QueryPatternByValue(DesignEntity design_entity, std::string value) {
  IntermediateTable *table = new IntermediateTable();
  switch(design_entity) {
    case DesignEntity::kAssign:
      table->InsertData(pkb->GetAssignEntityByVariable(value));
      break;
      case DesignEntity::kWhile:
        table->InsertData(pkb->GetWhileEntityByVariable(value));
        break;
        case DesignEntity::kIf:
          table->InsertData(pkb->GetIfEntityByVariable(value));
          break;
          default:
            break;
  }
  return table;
}

QuerySuchThatTwoSynonymCommand::QuerySuchThatTwoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

QuerySuchThatTwoSynonymCommand::QuerySuchThatTwoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause) :
clause(clause), receiver(receiver) {};

void QuerySuchThatTwoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QuerySuchThatTwoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* such_that = dynamic_cast<SuchThat *>(clause);
  PKBRelRefs pkb_rel= GetPKBRelRef(such_that->rel_ref);
  return this->receiver->QueryPKBTwoSynonyms(pkb_rel, such_that->first_synonym->GetType(), such_that->second_synonym->GetType());
}

QuerySuchThatOneSynonymCommand::QuerySuchThatOneSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

QuerySuchThatOneSynonymCommand::QuerySuchThatOneSynonymCommand(PKBQueryReceiver *receiver, Clause *clause) :
clause(clause), receiver(receiver) {};

void QuerySuchThatOneSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QuerySuchThatOneSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* such_that = dynamic_cast<SuchThat *>(clause);
  bool is_first_param = true;
  if (such_that ->left_is_synonym) {
    is_first_param = false;
  } else {
    is_first_param = true;
  }
  std::string first = such_that->left_hand_side;
  std::string second = such_that->right_hand_side;


  switch (such_that->rel_ref) {
    case RelRef::kFollows:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kFollows, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kFollowedBy, second);
    case RelRef::kParent:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kParent, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kChild, second);
    case RelRef::kUsesS:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kUsesS, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kUsedByS, second);
    case RelRef::kModifiesS:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kModifiedByStatement, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kModifiesStatement, second);
    case RelRef::kFollowsT:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kFollowsT, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kFollowedByT, second);
    case RelRef::kParentT:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kParentT, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kChildT, second);
case RelRef::kUsesP:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kUsesC, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kUsedByC, second);
case RelRef::kModifiesP:return is_first_param ? this->receiver->QueryPKBByValue(PKBRelRefs::kModifiedByContainer, first) : this->receiver->QueryPKBByValue(PKBRelRefs::kModifiesContainer, second);
default:
  return new IntermediateTable();
  // TODO: Throw error?
  }
}

QuerySuchThatNoSynonymCommand::QuerySuchThatNoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

QuerySuchThatNoSynonymCommand::QuerySuchThatNoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause) :
clause(clause), receiver(receiver) {};

void QuerySuchThatNoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QuerySuchThatNoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* such_that = dynamic_cast<SuchThat *>(clause);
  PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref);
  return this->receiver->QueryRelRefExistence(pkb_rel);
}

QueryPatternTwoSynonymCommand::QueryPatternTwoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

QueryPatternTwoSynonymCommand::QueryPatternTwoSynonymCommand(PKBQueryReceiver *receiver, Clause *clause) :
clause(clause), receiver(receiver) {};

void QueryPatternTwoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QueryPatternTwoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* pattern = dynamic_cast<Pattern *>(clause);
  // TODO: CHANGE here for while and if support
  return this->receiver->QueryDesignEntity(DesignEntity::kAssign);
}

QueryPatternOneSynonymCommand::QueryPatternOneSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

QueryPatternOneSynonymCommand::QueryPatternOneSynonymCommand(PKBQueryReceiver *receiver, Clause *clause) :
clause(clause), receiver(receiver) {};

void QueryPatternOneSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QueryPatternOneSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* pattern = dynamic_cast<Pattern *>(clause);
  // TODO: CHANGE here for while and if support
  return this->receiver->QueryPatternByValue(DesignEntity::kAssign, pattern->left_hand_side);
}