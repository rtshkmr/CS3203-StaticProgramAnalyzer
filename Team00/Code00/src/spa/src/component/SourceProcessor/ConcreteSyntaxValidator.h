/**
 *  ConcreteSyntaxValidator checks whether the tokenized statement follows the Concrete Grammar Syntax.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_

#include <datatype/DataType.h>
#include <vector>

using namespace std;

class ConcreteSyntaxValidator {
 public:
  ConcreteSyntaxValidator();
  bool ValidateConcreteSyntax(vector<Token> tokens);

 private:
  bool ValidateToken(Token token);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_
