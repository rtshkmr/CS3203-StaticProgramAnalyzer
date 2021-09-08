/**
 *  This file contains the implementation for syntax validation for a single token, based on the
 *  tag of the token. To prevent unnecessary bi-directional relationships, we shall not modify flags
 *  within PSubsystem from within this class.
 */
#include "SyntaxValidator.h"
#include <cassert>

SyntaxValidator::SyntaxValidator() = default;

/**
 * Validates semantic syntax within a single statement by referring to the token tags and adhering to various semantic
 * rules.
 *
 * @param tokens
 * @return
 */
bool SyntaxValidator::ValidateSemanticSyntax(vector<Token> tokens) {
  // check starting token to determine what kind of keyword it is and handle subcases in separate private functions:
  Token first_token = tokens.at(0);
  if (Token::IsKeywordToken(first_token)) {
    switch (first_token.GetTokenTag()) {
      case TokenTag::kProcedureKeyword:return ValidateProcedureSemantics(tokens);
      case TokenTag::kReadKeyword: // might need a better name to refer to a group of these keywords:
      case TokenTag::kPrintKeyword:
      case TokenTag::kCallKeyword:return ValidateMacroFunctionSemantics(tokens);
      case TokenTag::kIfKeyword:return ValidateIfStatementSemantics(tokens);
      case TokenTag::kElseKeyword:return ValidateElseKeyword(tokens);
      case TokenTag::kWhileKeyword:return ValidateWhileKeyword(tokens);
        // QQ: will if and then appear in the same line always or can they separate? will put it as true by default first
      case TokenTag::kThenKeyword:return ValidateThenKeyword(tokens);
      default: {
        assert(false);
      }
    }
  } else if (first_token.GetTokenTag() == TokenTag::kName) { // it's an assignment operator
    return ValidateAssignmentSemantics(tokens);
  } else if (first_token.GetTokenTag() == TokenTag::kCloseBrace) {
    return ValidateCloseBrace(tokens);
  } else { // any other case
    return false;
  }
}

bool SyntaxValidator::ValidateProcedureSemantics(const vector<Token>& tokens) {
  // QQ: is this part (fixed number of tokens for every type of statement a valid guarantee? Feels like it's
  //     very hardcode-y though :(
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kOpenBrace;
}
// for read, print and call keywords
bool SyntaxValidator::ValidateMacroFunctionSemantics(const vector<Token>& tokens) {
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kSemicolon;
}
bool SyntaxValidator::ValidateIfStatementSemantics(const vector<Token>& tokens) {
  // todo: 1. this is a very rudimentary implementation.
  //       2. Future implementation needs to support recursive check of conditional expressions, possibly need to do
  //       something like : identify open and close brace to identify scope, then check recursively within each scope.
  //       Doing that would allow us to handle nested conditional_expressions.
  //       possible future choice: separate into IsCondExpr and ValidateCondExpr

  // comms: check if entity factory can handle nested cond_expr
  bool output =  tokens.capacity() == 8
      && tokens.at(1).GetTokenTag() == TokenTag::kOpenBracket
      && (tokens.at(2).GetTokenTag() == TokenTag::kName || tokens.at(2).GetTokenTag() == TokenTag::kInteger)
      && tokens.at(3).GetTokenTag() == TokenTag::kBinaryComparisonOperator
      && (tokens.at(4).GetTokenTag() == TokenTag::kName || tokens.at(4).GetTokenTag() == TokenTag::kInteger)
      && tokens.at(5).GetTokenTag() == TokenTag::kCloseBracket
      && tokens.at(6).GetTokenTag() == TokenTag::kThenKeyword
      && tokens.at(7).GetTokenTag() == TokenTag::kOpenBrace;
  return output;
}

bool SyntaxValidator::ValidateThenKeyword(const vector<Token>& tokens) {
  return true;
}
bool SyntaxValidator::ValidateElseKeyword(const vector<Token>& tokens) {
  return tokens.capacity()==2 && tokens.at(1).GetTokenTag()==TokenTag::kOpenBrace;
}
bool SyntaxValidator::ValidateWhileKeyword(const vector<Token>& tokens) {
  bool output =  tokens.capacity() == 8
      && tokens.at(1).GetTokenTag() == TokenTag::kOpenBracket
      && (tokens.at(2).GetTokenTag() == TokenTag::kName || tokens.at(2).GetTokenTag() == TokenTag::kInteger)
      && tokens.at(3).GetTokenTag() == TokenTag::kBinaryComparisonOperator
      && (tokens.at(4).GetTokenTag() == TokenTag::kName || tokens.at(4).GetTokenTag() == TokenTag::kInteger)
      && tokens.at(5).GetTokenTag() == TokenTag::kCloseBracket
      && tokens.at(6).GetTokenTag() == TokenTag::kThenKeyword
      && tokens.at(7).GetTokenTag() == TokenTag::kOpenBrace;
  return output;
}
bool SyntaxValidator::ValidateAssignmentSemantics(const vector<Token>& tokens) {
  return false;
}
bool SyntaxValidator::ValidateCloseBrace(const vector<Token>& tokens) { // redundant function in case there are edge cases
  return true;
}
