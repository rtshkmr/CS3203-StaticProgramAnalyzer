#include "QueryParser.h"
#include "QueryValidator.h"
#include <component/QueryProcessor/types/Exceptions.h>
#include <unordered_set>
#include <unordered_map>
#include <set>

std::set<std::pair<DesignEntity, Attribute>> valid_attrRefs = {{DesignEntity::kProcedure, Attribute::kProcName},
                                                               {DesignEntity::kCall, Attribute::kProcName},
                                                               {DesignEntity::kVariable, Attribute::kVarName},
                                                               {DesignEntity::kRead, Attribute::kVarName},
                                                               {DesignEntity::kPrint, Attribute::kVarName},
                                                               {DesignEntity::kConstant, Attribute::kValue},
                                                               {DesignEntity::kStmt, Attribute::kStmtNumber},
                                                               {DesignEntity::kRead, Attribute::kStmtNumber},
                                                               {DesignEntity::kPrint, Attribute::kStmtNumber},
                                                               {DesignEntity::kCall, Attribute::kStmtNumber},
                                                               {DesignEntity::kWhile, Attribute::kStmtNumber},
                                                               {DesignEntity::kIf, Attribute::kStmtNumber},
                                                               {DesignEntity::kAssign, Attribute::kStmtNumber}};

std::unordered_set<DesignEntity> valid_lhs_UsesS = {DesignEntity::kAssign, DesignEntity::kPrint,
                                                    DesignEntity::kIf, DesignEntity::kWhile, DesignEntity::kStmt};

std::unordered_set<DesignEntity> valid_lhs_UsesP_ModifiesP = {DesignEntity::kProcedure, DesignEntity::kCall};

std::unordered_set<DesignEntity> valid_lhs_ModifiesS = {DesignEntity::kAssign, DesignEntity::kRead,
                                                        DesignEntity::kIf, DesignEntity::kWhile, DesignEntity::kStmt};

bool QueryValidator::IsValid_LhsStmt_RhsEnt(std::string l, std::string r, bool lhs_is_syn,
                                             bool rhs_is_syn, std::list<Synonym*>* synonyms, RelRef rf) {
  DesignEntity lhs = QueryParser::GetSynonymInfo(l, synonyms)->GetType();
  DesignEntity rhs = QueryParser::GetSynonymInfo(r, synonyms)->GetType();
  // if both are not synonyms, no semantic validation is needed.
  if (lhs == DesignEntity::kInvalid && rhs == DesignEntity::kInvalid) {
    return true;
  }

  std::unordered_set<DesignEntity>* valid_lhs = (rf == RelRef::kUsesS) ? & valid_lhs_UsesS : & valid_lhs_ModifiesS;

  // if both are synonyms
  if (lhs != DesignEntity::kInvalid && rhs != DesignEntity::kInvalid) {
    // lhs must be assign, print, if, while, procedure, call
    bool lhs_is_valid = (valid_lhs->find(lhs) != valid_lhs->end()) ? true : false;
    // rhs must be a variable
    bool rhs_is_valid = (rhs == DesignEntity::kVariable) ? true : false;
    return lhs_is_valid && rhs_is_valid;
  }

  // if one is synonym
  if (rhs != DesignEntity::kInvalid) {
    return (rhs == DesignEntity::kVariable) ? true : false;
  }
  if (lhs != DesignEntity::kInvalid) {
    return (valid_lhs->find(lhs) != valid_lhs->end()) ? true : false;
  }
  return true;
}

bool QueryValidator::IsValid_LhsEnt_RhsEnt(std::string l, std::string r, bool lhs_is_syn,
                                             bool rhs_is_syn, std::list<Synonym*>* synonyms, RelRef rf) {
  DesignEntity lhs = QueryParser::GetSynonymInfo(l, synonyms)->GetType();
  DesignEntity rhs = QueryParser::GetSynonymInfo(r, synonyms)->GetType();
  // if both are not synonyms, no semantic validation is needed
  if (lhs == DesignEntity::kInvalid && rhs == DesignEntity::kInvalid) {
    return true;
  }

  // if both are synonyms
  if (lhs != DesignEntity::kInvalid && rhs != DesignEntity::kInvalid) {
    // lhs must be proc or call
    bool lhs_is_valid = (valid_lhs_UsesP_ModifiesP.find(lhs) != valid_lhs_UsesP_ModifiesP.end()) ? true : false;
    // rhs must be a variable
    bool rhs_is_valid = (rhs == DesignEntity::kVariable) ? true : false;
    return lhs_is_valid && rhs_is_valid;
  }

  // if one is synonym
  if (rhs != DesignEntity::kInvalid) {
    return (rhs == DesignEntity::kVariable) ? true : false;
  }
  if (lhs != DesignEntity::kInvalid) {
    return (valid_lhs_UsesP_ModifiesP.find(lhs) != valid_lhs_UsesP_ModifiesP.end()) ? true : false;
  }
  return true;
}

bool QueryValidator::IsValid_LhsStmt_RhsStmt(std::string l, std::string r, bool lhs_is_syn,
                                             bool rhs_is_syn, std::list<Synonym*>* synonyms) {
  // Synonyms involved in relationships between statements cannot be variable or procedure.
  if (lhs_is_syn) {
    Synonym* lhs = QueryParser::GetSynonymInfo(l, synonyms);
    if (lhs->GetType() == DesignEntity::kVariable || lhs->GetType() == DesignEntity::kProcedure) {
      return false;
    }
  }
  if (rhs_is_syn) {
    Synonym* rhs = QueryParser::GetSynonymInfo(r, synonyms);
    if (rhs->GetType() == DesignEntity::kVariable || rhs->GetType() == DesignEntity::kProcedure) {
      return false;
    }
  }
  return true;
}

/**
 * Checks for semantic validity of a RelRef, by calling helper functions.
 * @param lhs is a string representing left hand side argument.
 * @param rhs is a string representing right hand side argument.
 * @param rf is the relRef enum type.
 * @param lhs_is_syn is a bool that evaluates to true if lhs is a synonym, false otherwise.
 * @param rhs_is_syn is a bool that evaluates to true if rhs is a synonym, false otherwise.
 * @param synonyms is a reference to the list of synonyms.
 * @return true if RelRef has semantically valid left and right arguments, false otherwise.
 */
bool QueryValidator::Is_Semantically_Valid_RelRef(std::string lhs, std::string rhs, RelRef rf, bool lhs_is_syn,
                                                  bool rhs_is_syn, std::list<Synonym*>* synonyms) {
  if (rf == RelRef::kUsesS || rf == RelRef::kModifiesS) {
    return QueryValidator::IsValid_LhsStmt_RhsEnt(lhs, rhs, lhs_is_syn, rhs_is_syn, synonyms, rf);
  } if (rf == RelRef::kUsesP || rf == RelRef::kModifiesP) {
    return QueryValidator::IsValid_LhsEnt_RhsEnt(lhs, rhs, lhs_is_syn, rhs_is_syn, synonyms, rf);
  } else {
    return QueryValidator::IsValid_LhsStmt_RhsStmt(lhs, rhs, lhs_is_syn, rhs_is_syn, synonyms);
  }
  return true;
}

/**
 * Checks for semantic validity of a AttrRef.
 * @param s is a pointer to the syonym in the AttrRef.
 * @param attr_name is of type Attribute and represents the AttrName in the AttrRef.
 * @return true if Attrref is semanticaly valid for the synonym type, false otherwise.
 */
bool QueryValidator::Is_Semantically_Valid_AttrRef(Synonym* s, Attribute attr_name) {
  auto candidate = std::make_pair(s->GetType(), attr_name);
  return valid_attrRefs.find(candidate) != valid_attrRefs.end() ? true : false;
}