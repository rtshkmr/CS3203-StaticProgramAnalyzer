#include "Types.h"
#include <unordered_map>

static std::unordered_map<std::string, RelRef> _relref_map = {{"ModifiesP", RelRef::kModifiesP},
                                                              {"ModifiesS", RelRef::kModifiesS},
                                                              {"UsesP", RelRef::kUsesP},
                                                              {"UsesS", RelRef::kUsesS},
                                                              {"Calls", RelRef::kCalls},
                                                              {"Calls*", RelRef::kCallsT},
                                                              {"Parent", RelRef::kParent},
                                                              {"Parent*", RelRef::kParentT},
                                                              {"Follows", RelRef::kFollows},
                                                              {"Follows*", RelRef::kFollowsT},
                                                              {"Next", RelRef::kNext},
                                                              {"Next*", RelRef::kNextT},
                                                              {"Affects", RelRef::kAffects},
                                                              {"Affects*", RelRef::kAffectsT}};

RelRef GetRelRef(std::string reference) {
  std::unordered_map<std::string, RelRef>::const_iterator iter = _relref_map.find(reference);
  if (iter != _relref_map.end()) {
    return iter->second;
  }
  return RelRef::kInvalid;
}

static std::unordered_map<std::string, Attribute> _attribute_map = {{"procName", Attribute::kProcName},
                                                                    {"varName", Attribute::kProcName},
                                                                    {"value", Attribute::kProcName},
                                                                    {"stmt#", Attribute::kStmtNumber}};
Attribute GetAttribute(std::string attr_string) {
  std::unordered_map<std::string, Attribute>::const_iterator iter = _attribute_map.find(attr_string);
  if (iter != _attribute_map.end()) {
    return iter->second;
  }
  return Attribute::kInvalid;
}

DesignEntity GetDesignEntity(std::string reference) {
  if (reference == "stmt") {
    return DesignEntity::kStmt;
  } else if (reference == "read") {
    return DesignEntity::kRead;
  } else if (reference == "print") {
    return DesignEntity::kPrint;
  } else if (reference == "call") {
    return DesignEntity::kCall;
  } else if (reference == "while") {
    return DesignEntity::kWhile;
  } else if (reference == "if") {
    return DesignEntity::kIf;
  } else if (reference == "assign") {
    return DesignEntity::kAssign;
  } else if (reference == "variable") {
    return DesignEntity::kVariable;
  } else if (reference == "constant") {
    return DesignEntity::kConstant;
  } else if (reference == "procedure") {
    return DesignEntity::kProcedure;
  }

  return DesignEntity::kInvalid;
}

void Group::AddSynToTargetSyns(Synonym* s) {
  target_synonyms.push_back(s);
}

bool Synonym::operator==(const Synonym& other) const{
  return this->name == other.name && this->type == other.type;
}

void Group::AddClauseToVector(Clause* clause) {
  clauses.push_back(clause);
}

std::vector<Clause*> Group::GetClauses() {
  return clauses;
}

bool Group::ContainsTargetSynonym() {
  return has_target_synonym;
}

std::vector<Synonym*> Group::GetTargetSynonyms() {
  return target_synonyms;
}

void Group::UpdateHasTargetSynonymAttr() {
  has_target_synonym = target_synonyms.size() > 0 ? true : false;
}
