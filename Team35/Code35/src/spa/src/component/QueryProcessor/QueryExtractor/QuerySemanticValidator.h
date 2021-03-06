#ifndef AUTOTESTER_QUERYSEMANTICVALIDATOR_H
#define AUTOTESTER_QUERYSEMANTICVALIDATOR_H

#include <component/QueryProcessor/types/Types.h>
#include <string>
#include <list>

/**
 * Contains helper functions to perform syntactic/semantic analysis of queries.
 * Note that some form of validations are also performed in QueryParser.
 */
class QuerySemanticValidator {
 private:
  static bool IsValid_LhsStmt_RhsStmt(const std::string& l, const std::string& r, bool lhs_is_syn,
                                      bool rhs_is_syn, std::list<Synonym*>* synonyms);
  static std::string GetAttrCompareType(const std::string& return_type, Synonym* syn, Attribute attr_type);
 public:
  static bool Is_Semantically_Valid_RelRef(const std::string& lhs, const std::string& rhs, RelRef rf,
                                           bool lhs_is_syn, bool rhs_is_syn, std::list<Synonym*>* synonyms);
  static bool Is_Semantically_Valid_AttrRef(Synonym* s, Attribute attr_name);
  static bool Is_Semantically_Valid_AttrCompare(Synonym* lhs_syn, Attribute lhs_attr, const std::string& lhs_return_type,
                                                Synonym* rhs_syn, Attribute rhs_attr, const std::string& rhs_return_type);
};

#endif //AUTOTESTER_QUERYSEMANTICVALIDATOR_H
