#include "Types.h"

RelRef GetRelRef(std::string reference) {
  if (reference == "ModifiesP") {
    return RelRef::kModifiesP;
  } else if (reference == "ModifiesS") {
    return RelRef::kModifiesS;
  } else if (reference == "UsesP") {
    return RelRef::kUsesP;
  } else if (reference == "UsesS") {
    return RelRef::kUsesS;
  } else if (reference == "Calls") {
    return RelRef::kCalls;
  } else if (reference == "Calls*") {
    return RelRef::kCallsT;
  } else if (reference == "Parent") {
    return RelRef::kParent;
  } else if (reference == "Parent*") {
    return RelRef::kParentT;
  } else if (reference == "Follows") {
    return RelRef::kFollows;
  } else if (reference == "Follows*") {
    return RelRef::kFollowsT;
  } else if (reference == "Next") {
    return RelRef::kNext;
  } else if (reference == "Next*") {
    return RelRef::kNextT;
  } else if (reference == "Affects") {
    return RelRef::kAffects;
  } else if (reference == "Affects*") {
    return RelRef::kAffectsT;
  }
  return RelRef::kInvalid;
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

bool Synonym::operator==(const Synonym& other) const{
  return this->name == other.name && this->type == other.type;
}

bool Group::AddClauseToVector(Clause* clause) {
  auto initialSize = clauses.size();
  clauses.push_back(clause);
  return (clauses.size() == initialSize + 1);
}

std::vector<Clause*> Group::GetClauses() {
  return clauses;
}

bool Group::ContainsTargetSynonym() {
  return has_target_synonym;
}

std::vector<Synonym> Group::GetTargetSynonyms() {
  return target_synonyms;
}
