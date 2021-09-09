/**
 *  SyntaxValidator checks whether the tokenized statement follows the Concrete Grammar Syntax.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_

#include <datatype/DataType.h>
#include <vector>

class SyntaxValidator {

 public:
  SyntaxValidator();
  static bool ValidateSemanticSyntax(std::vector<Token> tokens);
  static bool ValidateProcedureSemantics(const std::vector<Token>& tokens);
  static bool ValidateMacroFunctionSemantics(const std::vector<Token>& tokens);
  static bool ValidateIfStatementSemantics(const std::vector<Token>& tokens);
  static bool ValidateThenKeyword(const std::vector<Token>& tokens);
  static bool ValidateElseKeyword(const std::vector<Token>& tokens);
  static bool ValidateWhileKeyword(const std::vector<Token>& tokens);
  static bool ValidateAssignmentSemantics(const std::vector<Token>& tokens);
  static bool ValidateCloseBrace(const std::vector<Token>& tokens);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
