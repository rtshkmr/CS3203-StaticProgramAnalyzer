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
  static bool IsValid_LhsStmt_RhsStmt(std::string l, std::string r, bool lhs_is_syn,
                                      bool rhs_is_syn, std::list<Synonym*>* synonyms);
 public:
  static bool Is_Semantically_Valid_RelRef(std::string lhs, std::string rhs, RelRef rf,
                                           bool lhs_is_syn, bool rhs_is_syn, std::list<Synonym*>* synonyms);
  static bool Is_Semantically_Valid_AttrRef(Synonym* s, Attribute attr_name);
};

#endif //AUTOTESTER_QUERYSEMANTICVALIDATOR_H
