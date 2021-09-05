/**
 *  SyntaxValidator checks whether the tokenized statement follows the Concrete Grammar Syntax.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_

#include <datatype/DataType.h>
#include <vector>

using namespace std;

class SyntaxValidator {

 public:
  SyntaxValidator();
  static bool ValidateSemanticSyntax(vector<Token> tokens);
  static bool ValidateProcedureSemantics(const vector<Token>& tokens);
  static bool ValidateMacroFunctionSemantics(const vector<Token>& tokens);
  static bool ValidateIfStatementSemantics(const vector<Token>& tokens);
  static bool ValidateThenKeyword(const vector<Token>& tokens);
  static bool ValidateElseKeyword(const vector<Token>& tokens);
  static bool ValidateWhileKeyword(const vector<Token>& tokens);
  static bool ValidateAssignmentSemantics(const vector<Token>& tokens);
  static bool ValidateCloseBrace(const vector<Token>& tokens);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
