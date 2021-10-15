#include "PKBQueryCommand.h"

PKBQueryReceiver::PKBQueryReceiver(PKB *pkb) : pkb(pkb) {}

PKBRelRefs PKBQueryCommand::GetPKBRelRef(RelRef relation, bool order_of_values_unchanged_from_clause) {
  switch(relation) {
    case RelRef::kFollows:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kFollows : PKBRelRefs::kFollowedBy;
      case RelRef::kParentT:
        return order_of_values_unchanged_from_clause ? PKBRelRefs::kParentT : PKBRelRefs::kChildT;
        case RelRef::kParent:
          return order_of_values_unchanged_from_clause ? PKBRelRefs::kParent : PKBRelRefs::kChild;
          case RelRef::kModifiesP:
            return order_of_values_unchanged_from_clause ? PKBRelRefs::kModifiesContainer : PKBRelRefs::kModifiedByContainer;
            case RelRef::kModifiesS:
              return order_of_values_unchanged_from_clause ? PKBRelRefs::kModifies : PKBRelRefs::kModifiedBy;
              case RelRef::kUsesS:
                return order_of_values_unchanged_from_clause ? PKBRelRefs::kUses : PKBRelRefs::kUsedBy;
                case RelRef::kUsesP:
                  return order_of_values_unchanged_from_clause ? PKBRelRefs::kUsesC : PKBRelRefs::kUsedByC;
                  case RelRef::kFollowsT:
                    return order_of_values_unchanged_from_clause ? PKBRelRefs::kFollowsT : PKBRelRefs::kFollowedByT;
                    case RelRef::kCalls:
                      return order_of_values_unchanged_from_clause ? PKBRelRefs::kCalls : PKBRelRefs::kCalledBy;
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

/**
 * Query SuchThat clauses which contains 2 synonyms regardless of presence in the group table.
 * Generally we would want to keep the order of synonyms as given in the clause.
 * @param rel The PKBRelRef which shows the relationship between the first_synonym and second_synonym.
 * @param first_synonym The first_synonym in the Such that clause.
 * @param second_synonym The second synonym in the such that clause.
 * @return The intermediate table with the results.
 */
IntermediateTable* PKBQueryReceiver::QueryPKBTwoSynonyms(PKBRelRefs rel, DesignEntity first_synonym, DesignEntity second_synonym) {
  std::vector<std::tuple<Entity *, Entity *>> output = pkb->GetRelationshipByTypes(rel, first_synonym, second_synonym);
  IntermediateTable *table = new IntermediateTable();

  table->InsertData(output);
  return table;
}

/**
 * Works for Such that single synonym or no synonyms.
 * Note that the PKBRelRefs has to be decided during the Command creation.
 * E.g Uses(a1, "i") should be kUsedBy instead of kUses
 * @param rel The PKBRelRef to be decided during Command creation.
 * @param value The value of the Non-synonym.
 * @return The intermediate table with the results.
 */
IntermediateTable *PKBQueryReceiver::QueryPKBByValue(PKBRelRefs rel, std::string value) {
  std::vector<Entity *> output = pkb->GetRelationship(rel, value);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

// E.g Uses(a1, _) should be kUsedBy instead of kUses
IntermediateTable *PKBQueryReceiver::QueryPKBForSynonymWithWildCard(PKBRelRefs rel, DesignEntity entity) {
  std::vector<Entity *> output = pkb->GetRelationshipByType(rel, entity);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

/**
 * This should only be called for such that wildcards.
 * Note: This might be used for with clauses.
 * @param rel The RelRef converted to PKBRelRefs of a such that clause
 * @return The intermediate table with the results.
 */
IntermediateTable *PKBQueryReceiver::QueryRelRefExistence(PKBRelRefs rel) {
  bool output = pkb->HasRelationship(rel);
  IntermediateTable *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

/**
 * Querying Pattern clause with Assign synonym in the group table and the variable clause.
 * @param design_entity The type of design entity in the pattern clause.
 * @return The intermediate table with the results.
 */
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

  if (value == "_") {
    // Returns std::vector<Entity*>
    table->InsertData(pkb->GetDesignEntities(design_entity));
    return table;
  }

  // Returns std::vector<Entity*>
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


//Returns true in the Intermediate table if the relationship contains at least some values. Used for value and wildcard.
IntermediateTable *PKBQueryReceiver::QueryPKBByValueForBoolean(PKBRelRefs rel, std::string value) {
  IntermediateTable *table = new IntermediateTable();
  std::vector<Entity *> list = pkb->GetRelationship(rel, value);
  table->InsertData(list.size() != 0);
  return table;
}

// Returns true if the relationship holds for the 2 given values (no wildcards)
// Such that uses(3, "x")
IntermediateTable *
PKBQueryReceiver::QueryPKBByValueForBoolean(PKBRelRefs rel, std::string first_value, std::string second_value) {
  IntermediateTable *table = new IntermediateTable();
  bool has_result = pkb->HasRelationship(rel, first_value, second_value);
  table->InsertData(has_result);
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
  PKBRelRefs pkb_rel= GetPKBRelRef(such_that->rel_ref, true);
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
  bool synonym_is_first_param;
  if (such_that ->left_is_synonym) {
    synonym_is_first_param = true;
  } else {
    synonym_is_first_param = false;
  }
  std::string first = such_that->left_hand_side;
  std::string second = such_that->right_hand_side;
  std::string query_value = synonym_is_first_param ? second : first;
  Synonym* query_synonym = synonym_is_first_param ? such_that->first_synonym : such_that->second_synonym;

  if (query_value == "_") {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, synonym_is_first_param);
    return this->receiver->QueryPKBForSynonymWithWildCard(pkb_rel, query_synonym->GetType());
  } else {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, !synonym_is_first_param);
    return this->receiver->QueryPKBByValue(pkb_rel, query_value);
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
  if (such_that->left_hand_side == "_" && such_that->right_hand_side == "_") {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, true);
    return this->receiver->QueryRelRefExistence(pkb_rel);
  } else if (such_that->left_hand_side == "_") {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, false);
    return this->receiver->QueryPKBByValueForBoolean(pkb_rel, such_that->right_hand_side);
  } else if (such_that->right_hand_side == "_") {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, true);
    return this->receiver->QueryPKBByValueForBoolean(pkb_rel, such_that->right_hand_side);
  } else {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, true);
    return this->receiver->QueryPKBByValueForBoolean(pkb_rel, such_that->left_hand_side, such_that->right_hand_side);
  }
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