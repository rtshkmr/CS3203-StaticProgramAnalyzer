#include "QueryParser.h"
#include "QuerySemanticValidator.h"
#include <unordered_set>
#include <set>

// note: kProgLine does not have attrs based on PQL grammar.
std::set<std::pair<DesignEntity, Attribute>> valid_attrRefs = {
        {DesignEntity::kProcedure, Attribute::kProcName},
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
        {DesignEntity::kAssign, Attribute::kStmtNumber},
        {DesignEntity::kConstant, Attribute::kValue},
};

// note: here, kInvalid refers to the absense of a synonym
std::set<std::tuple<RelRef, DesignEntity, DesignEntity>> valid_relref_args = {
        // 1 synonym
        {RelRef::kCalls, DesignEntity::kProcedure, DesignEntity::kInvalid},
        {RelRef::kCalls, DesignEntity::kInvalid, DesignEntity::kProcedure},
        {RelRef::kCallsT, DesignEntity::kProcedure, DesignEntity::kInvalid},
        {RelRef::kCallsT, DesignEntity::kInvalid, DesignEntity::kProcedure},
        {RelRef::kUsesS, DesignEntity::kAssign, DesignEntity::kInvalid},
        {RelRef::kUsesS, DesignEntity::kPrint, DesignEntity::kInvalid},
        {RelRef::kUsesS, DesignEntity::kIf, DesignEntity::kInvalid},
        {RelRef::kUsesS, DesignEntity::kWhile, DesignEntity::kInvalid},
        {RelRef::kUsesS, DesignEntity::kStmt, DesignEntity::kInvalid},
        {RelRef::kUsesS, DesignEntity::kInvalid, DesignEntity::kVariable},
        {RelRef::kUsesP, DesignEntity::kProcedure, DesignEntity::kInvalid},
        {RelRef::kUsesP, DesignEntity::kCall, DesignEntity::kInvalid},
        {RelRef::kUsesP, DesignEntity::kInvalid, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kAssign, DesignEntity::kInvalid},
        {RelRef::kModifiesS, DesignEntity::kRead, DesignEntity::kInvalid},
        {RelRef::kModifiesS, DesignEntity::kIf, DesignEntity::kInvalid},
        {RelRef::kModifiesS, DesignEntity::kWhile, DesignEntity::kInvalid},
        {RelRef::kModifiesS, DesignEntity::kStmt, DesignEntity::kInvalid},
        {RelRef::kModifiesS, DesignEntity::kInvalid, DesignEntity::kVariable},
        {RelRef::kModifiesP, DesignEntity::kProcedure, DesignEntity::kInvalid},
        {RelRef::kModifiesP, DesignEntity::kCall, DesignEntity::kInvalid},
        {RelRef::kModifiesP, DesignEntity::kInvalid, DesignEntity::kVariable},
        {RelRef::kAffects, DesignEntity::kStmt, DesignEntity::kInvalid},
        {RelRef::kAffects, DesignEntity::kAssign, DesignEntity::kInvalid},
        {RelRef::kAffects, DesignEntity::kInvalid, DesignEntity::kStmt},
        {RelRef::kAffects, DesignEntity::kInvalid, DesignEntity::kAssign},
        {RelRef::kAffectsT, DesignEntity::kStmt, DesignEntity::kInvalid},
        {RelRef::kAffectsT, DesignEntity::kAssign, DesignEntity::kInvalid},
        {RelRef::kAffectsT, DesignEntity::kInvalid, DesignEntity::kStmt},
        {RelRef::kAffectsT, DesignEntity::kInvalid, DesignEntity::kAssign},
        // 2 synonyms
        {RelRef::kCalls, DesignEntity::kProcedure, DesignEntity::kProcedure},
        {RelRef::kCallsT, DesignEntity::kProcedure, DesignEntity::kProcedure},
        {RelRef::kUsesS, DesignEntity::kAssign, DesignEntity::kVariable},
        {RelRef::kUsesS, DesignEntity::kPrint, DesignEntity::kVariable},
        {RelRef::kUsesS, DesignEntity::kIf, DesignEntity::kVariable},
        {RelRef::kUsesS, DesignEntity::kWhile, DesignEntity::kVariable},
        {RelRef::kUsesS, DesignEntity::kStmt, DesignEntity::kVariable},
        {RelRef::kUsesP, DesignEntity::kProcedure, DesignEntity::kVariable},
        {RelRef::kUsesP, DesignEntity::kCall, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kAssign, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kRead, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kIf, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kWhile, DesignEntity::kVariable},
        {RelRef::kModifiesS, DesignEntity::kStmt, DesignEntity::kVariable},
        {RelRef::kModifiesP, DesignEntity::kProcedure, DesignEntity::kVariable},
        {RelRef::kModifiesP, DesignEntity::kCall, DesignEntity::kVariable},
        {RelRef::kAffects, DesignEntity::kStmt, DesignEntity::kStmt},
        {RelRef::kAffects, DesignEntity::kStmt, DesignEntity::kAssign},
        {RelRef::kAffects, DesignEntity::kAssign, DesignEntity::kStmt},
        {RelRef::kAffects, DesignEntity::kAssign, DesignEntity::kAssign},
        {RelRef::kAffectsT, DesignEntity::kStmt, DesignEntity::kStmt},
        {RelRef::kAffectsT, DesignEntity::kStmt, DesignEntity::kAssign},
        {RelRef::kAffectsT, DesignEntity::kAssign, DesignEntity::kStmt},
        {RelRef::kAffectsT, DesignEntity::kAssign, DesignEntity::kAssign},
};

bool QuerySemanticValidator::IsValid_LhsStmt_RhsStmt(std::string l, std::string r, bool lhs_is_syn,
                                             bool rhs_is_syn, std::list<Synonym*>* synonyms) {
  // Synonyms involved in relationships between statements cannot be variable, procedure, or constant.
  std::unordered_set<DesignEntity> stmt_blacklist = {DesignEntity::kVariable,
                                                     DesignEntity::kProcedure,
                                                     DesignEntity::kConstant};
  if (lhs_is_syn) {
    Synonym* lhs = QueryParser::GetSynonymInfo(l, synonyms);
    if (stmt_blacklist.find(lhs->GetType()) != stmt_blacklist.end()) {
      return false;
    }
  }
  if (rhs_is_syn) {
    Synonym* rhs = QueryParser::GetSynonymInfo(r, synonyms);
    if (stmt_blacklist.find(rhs->GetType()) != stmt_blacklist.end()) {
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
bool QuerySemanticValidator::Is_Semantically_Valid_RelRef(std::string l, std::string r, RelRef rf, bool lhs_is_syn,
                                                  bool rhs_is_syn, std::list<Synonym*>* synonyms) {
  // if neither lhs nor rhs is a synonym, no semantic validation is needed
  DesignEntity lhs = QueryParser::GetSynonymInfo(l, synonyms)->GetType();
  DesignEntity rhs = QueryParser::GetSynonymInfo(r, synonyms)->GetType();
  if (lhs == DesignEntity::kInvalid && rhs == DesignEntity::kInvalid) {
    return true;
  }

  // RelRefs with little permutations of arguments will use whitelist table.
  std::unordered_set<RelRef> whitelisted = {RelRef::kCalls, RelRef::kCallsT, RelRef::kAffects, RelRef::kAffectsT,
                                                    RelRef::kUsesS, RelRef::kUsesP, RelRef::kModifiesS, RelRef::kModifiesP};
  if (whitelisted.find(rf) != whitelisted.end()) {
    // for the purposes of pql grammar, prog_line has the same behavior as stmt
    if (lhs == DesignEntity::kProgLine) lhs = DesignEntity::kStmt;
    if (rhs == DesignEntity::kProgLine) rhs = DesignEntity::kStmt;
    auto t = std::make_tuple(rf, lhs, rhs);
    return valid_relref_args.find(t) != valid_relref_args.end();
  }

  // RelRefs with many permutations of arguments will use a blacklist approach instead.
  return QuerySemanticValidator::IsValid_LhsStmt_RhsStmt(l, r, lhs_is_syn, rhs_is_syn, synonyms);
}

/**
 * Checks for semantic validity of a AttrRef.
 * @param s is a pointer to the syonym in the AttrRef.
 * @param attr_name is of type Attribute and represents the AttrName in the AttrRef.
 * @return true if Attrref is semanticaly valid for the synonym type, false otherwise.
 */
bool QuerySemanticValidator::Is_Semantically_Valid_AttrRef(Synonym* s, Attribute attr_name) {
  auto candidate = std::make_pair(s->GetType(), attr_name);
  return valid_attrRefs.find(candidate) != valid_attrRefs.end() ? true : false;
}
