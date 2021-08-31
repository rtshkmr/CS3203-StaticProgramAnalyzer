/**
 *  ConcreteSyntaxValidator checks whether the tokenized statement follows the Concrete Grammar Syntax.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_

class ConcreteSyntaxValidator {
  public:
  ConcreteSyntaxValidator();
  static bool validateConcreteSyntax(vector<Token> tokens);

  private:
  static bool validateToken(Token token)
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_CONCRETESYNTAXVALIDATOR_H_
