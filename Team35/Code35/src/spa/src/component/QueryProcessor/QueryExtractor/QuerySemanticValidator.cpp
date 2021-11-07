#include "QueryParser.h"
#include "QuerySemanticValidator.h"
#include <exception/SpaException.h>
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
};

// first DesignEntity is for lhs, second is for rhs of relref
// note: here, kInvalid refers to the absence of a synonym
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

bool QuerySemanticValidator::IsValid_LhsStmt_RhsStmt(const std::string& l, std::string r, bool lhs_is_syn,
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
bool QuerySemanticValidator::Is_Semantically_Valid_RelRef(const std::string& l, std::string r, RelRef rf, bool lhs_is_syn,
                                                          bool rhs_is_syn, std::list<Synonym*>* synonyms) {
  // if neither lhs nor rhs is a synonym, no semantic validation is needed
// note that the same kInvalid enum is used differently by the front and the backend code. 
  DesignEntity lhs = DesignEntity::kInvalid;
  DesignEntity rhs = DesignEntity::kInvalid;
  if (lhs_is_syn) {
    lhs = QueryParser::GetSynonymInfo(l, synonyms)->GetType();
  }
  if (rhs_is_syn) {
    rhs = QueryParser::GetSynonymInfo(r, synonyms)->GetType();
  }
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
  return valid_attrRefs.find(candidate) != valid_attrRefs.end();
}

/**
 * Checks for semantic validity of the left and right arguments in an AttrCompare in with-clause.
 * @param lhs_syn is a pointer to the left hand side synonym in the AttrRef, which could be a nullptr.
 * @param lhs_attr is of type Attribute and represents the Attribute type of the left hand side argument.
 * @param lhs_return_type is a string that is either "INTEGER", "STRING", or "SYNONYM" based on left argument type.
 * @param rhs_syn is a pointer to the right hand side synonym in the AttrRef, which could be a nullptr.
 * @param rhs_attr is of type Attribute and represents the Attribute type of the right hand side argument.
 * @param rhs_return_type is a string that is either "INTEGER", "STRING", or "SYNONYM" based on right argument type.
 * @return true if AttrCompare is semantically valid, false otherwise.
 */
bool QuerySemanticValidator::Is_Semantically_Valid_AttrCompare(Synonym* lhs_syn, Attribute lhs_attr,
                                                               const std::string& lhs_return_type, Synonym* rhs_syn,
                                                               Attribute rhs_attr, const std::string& rhs_return_type) {
  // edge cases: meaningless but valid query
  if (lhs_return_type == "INTEGER" && rhs_return_type == "INTEGER") return true;
  if (lhs_return_type == "STRING" && rhs_return_type == "STRING") return true;

  bool lhs_is_syn = lhs_return_type == "SYNONYM";
  bool rhs_is_syn = rhs_return_type == "SYNONYM";
  bool lhs_has_attr = lhs_attr != Attribute::kInvalid;
  bool rhs_has_attr = rhs_attr != Attribute::kInvalid;
  bool lhs_is_pl = lhs_is_syn && lhs_syn->GetType() == DesignEntity::kProgLine;
  bool rhs_is_pl = rhs_is_syn && lhs_syn->GetType() == DesignEntity::kProgLine;

  // conceptually, at least one lhs or rhs of with-cl must involve an attribute (or a prog_line with no attributes);
  bool one_side_is_prog_line = lhs_is_pl || rhs_is_pl;
  bool one_side_has_attr = lhs_has_attr || rhs_has_attr;
  if (!(one_side_is_prog_line || one_side_has_attr)) return false;

  // for all synonyms (on either side), it must always have an attribute (or a prog_line with no attribute).
  bool is_lhs_syn_illegal = lhs_is_syn && !lhs_has_attr && !lhs_is_pl;
  bool is_rhs_syn_illegal = rhs_is_syn && !rhs_has_attr && !rhs_is_pl;
  if (is_lhs_syn_illegal || is_rhs_syn_illegal) return false;

  // check that the "type" on both lhs and rhs are the same (ie NAME or INTEGER)
  std::string lhs_type = QuerySemanticValidator::GetAttrCompareType(lhs_return_type, lhs_syn, lhs_attr);
  std::string rhs_type = QuerySemanticValidator::GetAttrCompareType(rhs_return_type, rhs_syn, rhs_attr);
  return lhs_type == rhs_type;
}

std::string QuerySemanticValidator::GetAttrCompareType(const std::string& return_type, Synonym* syn, Attribute attr_type) {
  std::string type1 = "NAME";
  std::string type2 = "INTEGER";
  // argument in AttrCompare was a string
  if (return_type == "STRING") return type1;
  // argument in AttrCompare was a integer
  if (return_type == "INTEGER") return type2;
  // argument in AttrCompare was a synonym (with its attribute).
  if (return_type == "SYNONYM") {
    DesignEntity de = syn->GetType();
    if (de == DesignEntity::kProgLine) return type2;
    if (attr_type == Attribute::kStmtNumber || attr_type == Attribute::kValue) return type2;
    if (attr_type == Attribute::kProcName || attr_type == Attribute::kVarName) return type1;
  }

  throw PQLValidationException("Received unexpected argument in with clause.");
}
