/**
 *  SyntaxValidator checks whether the tokenized statement follows the Concrete Grammar Syntax.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_

#include <datatype/DataType.h>
#include <vector>

class SyntaxValidator {
 private:
  static bool VerifyNameTokensAreDelimited(const std::vector<Token>& statement_tokens);

 public:
  SyntaxValidator();
  static bool ValidateSyntax(std::vector<Token> statement_tokens);
  static bool ValidateProcedureSyntax(const std::vector<Token>& tokens);
  static bool ValidateMacroFunctionSyntax(const std::vector<Token>& tokens);
  static bool ValidateIfStatementSyntax(const std::vector<Token>& tokens);
  static bool ValidateElseKeyword(const std::vector<Token>& tokens);
  static bool ValidateWhileKeyword(const std::vector<Token>& tokens);
  static bool ValidateAssignmentSentenceSyntax(const std::vector<Token>& statement_tokens);
  static bool ValidateCloseBrace(const std::vector<Token>& tokens);
  static bool IsFactor(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsTerm(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsRelFactor(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsRelExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool IsCondExpr(const std::vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx);
  static bool StatementPassesCommonBlacklistRules(const std::vector<Token>& statement_tokens);
  static int FindSplitPoint(const std::vector<Token>& statement_tokens,
                            int left_boundary_idx,
                            int right_boundary_idx,
                            const std::regex& desired_pattern);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_SYNTAXVALIDATOR_H_
