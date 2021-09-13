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
  // todo: majority of the functions here may be made private, kept it public for testing mainly
  static bool ValidateSemanticSyntax(std::vector<Token> tokens);
  static bool ValidateProcedureSemantics(const std::vector<Token>& tokens);
  static bool ValidateMacroFunctionSemantics(const std::vector<Token>& tokens);
  static bool ValidateIfStatementSemantics(const std::vector<Token>& tokens);
  static bool ValidateThenKeyword(const std::vector<Token>& tokens);
  static bool ValidateElseKeyword(const std::vector<Token>& tokens);
  static bool ValidateWhileKeyword(const std::vector<Token>& tokens);
  static bool ValidateAssignmentSemantics(const std::vector<Token>& tokens);
  static bool ValidateCloseBrace(const std::vector<Token>& tokens);
  static int CountTokens(std::vector<Token> tokens, TokenTag target_tag);
  static int CountTokens(std::vector<Token> tokens, TokenTag target_tag, std::string target_string);
  static bool IsFactor(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsTerm(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsRelFactor(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsRelExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsCondExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static auto GetTokenMatchReverseIterator(const std::vector<Token>& tokens,
                                           const std::regex& desired_pattern,
                                           int left_boundary_idx,
                                           int right_boundary_idx);
  static auto GetTokenMatchForwardIterator(const std::vector<Token>& tokens,
                                           const std::regex& desired_pattern,
                                           int left_idx,
                                           int right_idx);
  static int GetLastMatchingTokenIdx(const std::vector<Token>& tokens,
                                     const std::regex& desired_pattern,
                                     int left_boundary_idx,
                                     int right_boundary_idx);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_