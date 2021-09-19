#include "QueryParser.h"
#include "QueryValidator.h"
#include <unordered_set>

std::unordered_set<DesignEntity> valid_lhs_UsesS = {DesignEntity::kAssign, DesignEntity::kPrint,
                                              DesignEntity::kIf, DesignEntity::kWhile, DesignEntity::kStmt,
                                              DesignEntity::kProcedure, DesignEntity::kCall};

std::unordered_set<DesignEntity> valid_lhs_ModifiesS = {DesignEntity::kAssign, DesignEntity::kRead,
                                              DesignEntity::kIf, DesignEntity::kWhile, DesignEntity::kStmt,
                                              DesignEntity::kProcedure, DesignEntity::kCall};

bool QueryValidator::IsValid_LhsStmt_RhsProc(std::string l, std::string r, bool lhs_is_syn,
                                    bool rhs_is_syn, std::list<Synonym>* synonyms, RelRef rf) {
  DesignEntity lhs = QueryParser::GetSynonymInfo(l, synonyms).GetType();
  DesignEntity rhs = QueryParser::GetSynonymInfo(r, synonyms).GetType();
  // if both are not synonyms
  if (lhs == DesignEntity::kInvalid && rhs == DesignEntity::kInvalid) {
    return false;
  }

  std::unordered_set<DesignEntity>* valid_lhs = (rf == RelRef::kUsesS) ? &valid_lhs_UsesS : &valid_lhs_ModifiesS;

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

bool QueryValidator::IsValid_LhsStmt_RhsStmt(std::string l, std::string r, bool lhs_is_syn,
                                    bool rhs_is_syn, std::list<Synonym>* synonyms) {
  // Synonyms involved in relationships between statements cannot be variable or procedure.
  if (lhs_is_syn) {
    Synonym lhs = QueryParser::GetSynonymInfo(l, synonyms);
    if (lhs.GetType() == DesignEntity::kVariable || lhs.GetType() == DesignEntity::kProcedure) {
      return false;
    }
  }
  if (rhs_is_syn) {
    Synonym rhs = QueryParser::GetSynonymInfo(r, synonyms);
    if (rhs.GetType() == DesignEntity::kVariable || rhs.GetType() == DesignEntity::kProcedure) {
      return false;
    }
  }
  return true;
}

bool QueryValidator::Is_Semantically_Valid_RelRef(std::string lhs, std::string rhs, RelRef rf, bool lhs_is_syn,
                                                         bool rhs_is_syn, std::list<Synonym>* synonyms) {
  if (rf == RelRef::kUsesS || rf == RelRef::kModifiesS) {
    return QueryValidator::IsValid_LhsStmt_RhsProc(lhs, rhs, lhs_is_syn, rhs_is_syn, synonyms, rf);
  } else {
    return QueryValidator::IsValid_LhsStmt_RhsStmt(lhs, rhs, lhs_is_syn, rhs_is_syn, synonyms);
  }
  return true;
}