#include "PKBQueryCommand.h"
#include <cassert>
#include <utility>

PKBQueryReceiver::PKBQueryReceiver(DBManager *db_manager, QueryEvaluatorTable *table) : db_manager(db_manager), table(table) {}
PKBQueryReceiver::PKBQueryReceiver(DBManager *db_manager) : db_manager(db_manager), table(nullptr) {}

PKBRelRefs PKBQueryCommand::GetPKBRelRef(RelRef relation, bool order_of_values_unchanged_from_clause) {
  switch(relation) {
    case RelRef::kFollows:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kFollows : PKBRelRefs::kFollowedBy;
    case RelRef::kParentT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kParentT : PKBRelRefs::kChildT;
    case RelRef::kParent:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kParent : PKBRelRefs::kChild;
    case RelRef::kModifiesP:
    case RelRef::kModifiesS:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kModifies : PKBRelRefs::kModifiedBy;
    case RelRef::kUsesS:
    case RelRef::kUsesP:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kUses : PKBRelRefs::kUsedBy;
    case RelRef::kFollowsT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kFollowsT : PKBRelRefs::kFollowedByT;
    case RelRef::kCalls:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kCalls : PKBRelRefs::kCalledBy;
    case RelRef::kCallsT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kCallsT : PKBRelRefs::kCalledByT;
    case RelRef::kNext:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kNext : PKBRelRefs::kPrevious;
    case RelRef::kNextT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kNextT : PKBRelRefs::kPreviousT;
    case RelRef::kAffects:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kAffects : PKBRelRefs::kAffectedBy;
    case RelRef::kAffectsT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kAffectsT : PKBRelRefs::kAffectedByT;
    case RelRef::kNextBip:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kNextBip : PKBRelRefs::kPrevBip;
    case RelRef::kNextBipT:
      return order_of_values_unchanged_from_clause ? PKBRelRefs::kNextBipT : PKBRelRefs::kPrevBipT;
    default:return PKBRelRefs::kInvalid;
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
IntermediateTable* PKBQueryReceiver::QueryPKBTwoSynonyms(PKBRelRefs rel, Synonym *first_synonym, Synonym *second_synonym) {

  std::vector<Entity*> first_list = table->GetColumn(first_synonym);
  std::vector<Entity*> second_list =table->GetColumn(second_synonym);
  ScopeIndication scoping = GetDoubleSynonymScoping(first_list, second_list);
  std::vector<std::tuple<Entity *, Entity *>> output = db_manager->GetRelationshipByTypes(rel,
                                                                                          first_synonym->GetType(),
                                                                                          second_synonym->GetType(),
                                                                                          first_list,
                                                                                          second_list,
                                                                                          scoping);
  auto *intermediate_table = new IntermediateTable();
  intermediate_table->InsertData(output);
  return intermediate_table;
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
  std::vector<Entity *> output = db_manager->GetRelationship(rel, std::move(value));
  auto *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

// E.g Uses(a1, _) should be kUsedBy instead of kUses
IntermediateTable *PKBQueryReceiver::QueryEntityWithWildcard(PKBRelRefs rel, DesignEntity entity) {
  std::vector<Entity *> output = db_manager->GetFirstEntityOfRelationship(rel, entity);
  auto *table = new IntermediateTable();
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
  bool output = db_manager->HasRelationship(rel);
  auto *table = new IntermediateTable();
  table->InsertData(output);
  return table;
}

/**
 * Querying Pattern clause with [Assign, If, While] synonym in the group table and the variable clause.
 * @param design_entity The type of design entity in the pattern clause.
 * @return The intermediate table with the results.
 */
IntermediateTable *PKBQueryReceiver::QueryDesignEntity(DesignEntity design_entity) {
  auto *table = new IntermediateTable();
  switch(design_entity) {
    case DesignEntity::kAssign:
      PopulateAssignDoubleSynonym(table);
      break;
    case DesignEntity::kWhile:
      PopulatePatternDoubleSynonym(table, DesignEntity::kWhile);
      break;
    case DesignEntity::kIf:
      PopulatePatternDoubleSynonym(table, DesignEntity::kIf);
      break;
    default:
      break;
  }
  return table;
}

void PKBQueryReceiver::PopulateAssignDoubleSynonym(IntermediateTable *table) {
  std::vector<Entity*> assign_entity_list = db_manager->GetDesignEntities(DesignEntity::kAssign);
  table->InsertData(assign_entity_list);
  std::vector<std::tuple<Entity*, Entity*>> entity_pair_list;
  for (auto assign : assign_entity_list) {
    auto assign_entity = dynamic_cast<AssignEntity*>(assign);
    auto curr_pair = std::make_tuple(assign, assign_entity->GetVariableObj());
    entity_pair_list.emplace_back(curr_pair);
  }
  table->InsertData(entity_pair_list);
}

void PKBQueryReceiver::PopulatePatternDoubleSynonym(IntermediateTable *table, DesignEntity design_entity) {
  std::vector<std::tuple<Entity*, Entity*>> entity_pair_list;
  if (design_entity == DesignEntity::kWhile) {
    std::vector<Entity*> entity_list = db_manager->GetDesignEntities(DesignEntity::kWhile);
    table->InsertData(entity_list);
    for (auto entity : entity_list) {
      auto while_entity = dynamic_cast<WhileEntity*>(entity);
      for (auto variable : while_entity->GetControlVariables()) {
        entity_pair_list.emplace_back(entity, variable);
      }
    }
  } else {
    assert(design_entity == DesignEntity::kIf);
    std::vector<Entity*> entity_list = db_manager->GetDesignEntities(DesignEntity::kIf);
    table->InsertData(entity_list);
    for (auto entity : entity_list) {
      auto if_entity = dynamic_cast<IfEntity*>(entity);
      for (auto variable : if_entity->GetControlVariables()) {
        entity_pair_list.emplace_back(entity, variable);
      }
    }
  }
}

IntermediateTable *PKBQueryReceiver::QueryPatternByValue(DesignEntity design_entity, const std::string& value) {
  auto *table = new IntermediateTable();

  if (value == "_") {
    // Returns std::vector<Entity*>
    table->InsertData(db_manager->GetDesignEntities(design_entity));
    return table;
  }

  // Returns std::vector<Entity*>
  switch(design_entity) {
    case DesignEntity::kAssign:
      table->InsertData(db_manager->GetPatternEntities(DesignEntity::kAssign, value));
      break;
    case DesignEntity::kWhile:
      table->InsertData(db_manager->GetPatternEntities(DesignEntity::kWhile, value));
      break;
    case DesignEntity::kIf:
      table->InsertData(db_manager->GetPatternEntities(DesignEntity::kIf, value));
      break;
    default:
      break;
  }
  return table;
}


//Returns true in the Intermediate table if the relationship contains at least some values. Used for value and wildcard.
IntermediateTable *PKBQueryReceiver::QueryPKBByValueForBoolean(PKBRelRefs rel, std::string value) {
  auto *table = new IntermediateTable();
  std::vector<Entity *> list = db_manager->GetRelationship(rel, std::move(value));
  table->InsertData(!list.empty());
  return table;
}

// Returns true if the relationship holds for the 2 given values (no wildcards)
// Such that uses(3, "x")
IntermediateTable *
PKBQueryReceiver::QueryPKBByValueForBoolean(PKBRelRefs rel, std::string first_value, std::string second_value) {
  auto *table = new IntermediateTable();
  bool has_result = db_manager->HasRelationship(rel, std::move(first_value), std::move(second_value));
  table->InsertData(has_result);
  return table;
}

IntermediateTable *
PKBQueryReceiver::QueryAttributeMatch(type_attribute_pair first_attr_pair, type_attribute_pair second_attr_pair) {
  auto *table = new IntermediateTable();
  std::vector<std::tuple<Entity*, Entity*>> result = db_manager->
          GetEntitiesWithMatchingAttributes(std::move(first_attr_pair), second_attr_pair);
  table->InsertData(result);
  return table;
}

IntermediateTable *
PKBQueryReceiver::QueryEntityAttributeMatch(DesignEntity design_entity, Attribute attribute, std::string value) {
  auto *table = new IntermediateTable();
  std::vector<Entity*> result = db_manager->GetEntitiesWithAttributeValue(design_entity, attribute, std::move(value));
  table->InsertData(result);
  return table;
}

ScopeIndication
PKBQueryReceiver::GetDoubleSynonymScoping(const std::vector<Entity *>& first_entity_list,
                                          const std::vector<Entity *>& second_entity_list) {
  if (!first_entity_list.empty() && !second_entity_list.empty()) {
    return ScopeIndication::kAllScope;
  } else if (!first_entity_list.empty()) {
    return ScopeIndication::kLeftScope;
  } else if (!second_entity_list.empty()) {
    return ScopeIndication::kRightScope;
  } else {
    return ScopeIndication::kNoScope;
  }
}

QuerySuchThatTwoSynonymCommand::QuerySuchThatTwoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QuerySuchThatTwoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver_to_set) {
  delete this->receiver;
  this->receiver = receiver_to_set;
}

IntermediateTable * QuerySuchThatTwoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* such_that = dynamic_cast<SuchThat *>(clause);
  PKBRelRefs pkb_rel= GetPKBRelRef(such_that->rel_ref, true);
  return this->receiver->QueryPKBTwoSynonyms(pkb_rel, such_that->first_synonym, such_that->second_synonym);
}


QuerySuchThatOneSynonymCommand::QuerySuchThatOneSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QuerySuchThatOneSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QuerySuchThatOneSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* such_that = dynamic_cast<SuchThat *>(clause);
  bool synonym_is_first_param = such_that->left_is_synonym;

  std::string first = such_that->left_hand_side;
  std::string second = such_that->right_hand_side;
  std::string query_value = synonym_is_first_param ? second : first;
  Synonym* query_synonym = synonym_is_first_param ? such_that->first_synonym : such_that->second_synonym;

  if (query_value == "_") {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, synonym_is_first_param);
    return this->receiver->QueryEntityWithWildcard(pkb_rel, query_synonym->GetType());
  } else {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, !synonym_is_first_param);
    return this->receiver->QueryPKBByValue(pkb_rel, query_value);
  }

}

QuerySuchThatNoSynonymCommand::QuerySuchThatNoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

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
    return this->receiver->QueryPKBByValueForBoolean(pkb_rel, such_that->left_hand_side);
  } else {
    PKBRelRefs pkb_rel = GetPKBRelRef(such_that->rel_ref, true);
    return this->receiver->QueryPKBByValueForBoolean(pkb_rel, such_that->left_hand_side, such_that->right_hand_side);
  }
}

QueryPatternTwoSynonymCommand::QueryPatternTwoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QueryPatternTwoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QueryPatternTwoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* pattern = dynamic_cast<Pattern *>(clause);
  DesignEntity pattern_design_entity = pattern->first_synonym->GetType();
  return this->receiver->QueryDesignEntity(pattern_design_entity);
}

QueryPatternOneSynonymCommand::QueryPatternOneSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QueryPatternOneSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QueryPatternOneSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* pattern = dynamic_cast<Pattern *>(clause);
  DesignEntity pattern_design_entity = pattern->first_synonym->GetType();
  return this->receiver->QueryPatternByValue(pattern_design_entity, pattern->left_hand_side);
}

QueryWithTwoSynonymCommand::QueryWithTwoSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QueryWithTwoSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable * QueryWithTwoSynonymCommand::ExecuteQuery(Clause *clause) {
  auto* with = dynamic_cast<With *>(clause);
  auto first_attr_pair = std::make_tuple(with->GetFirstSynonymType(), with->left_attribute);
  auto second_attr_pair = std::make_tuple(with->GetSecondSynonymType(), with->right_attribute);
  return this->receiver->QueryAttributeMatch(first_attr_pair, second_attr_pair);
}

QueryWithOneSynonymCommand::QueryWithOneSynonymCommand(Clause *clause) : clause(clause), receiver(nullptr) {}

void QueryWithOneSynonymCommand::SetReceiver(PKBQueryReceiver *receiver) {
  delete this->receiver;
  this->receiver = receiver;
}

IntermediateTable *QueryWithOneSynonymCommand::ExecuteQuery(Clause *input_clause) {
  auto* with_clause = dynamic_cast<With*>(input_clause);
  bool synonym_is_first_param = with_clause->left_is_synonym;
  DesignEntity design_entity_to_check = synonym_is_first_param ? with_clause->GetFirstSynonymType()
          : with_clause->GetSecondSynonymType();
  Attribute attribute = synonym_is_first_param ? with_clause->left_attribute : with_clause->right_attribute;
  std::string value = synonym_is_first_param ? with_clause->right_hand_side : with_clause->left_hand_side;

  return this->receiver->QueryEntityAttributeMatch(design_entity_to_check, attribute, value);
}
