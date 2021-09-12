/**
 *  This file contains the implementation for syntax validation for a single token, based on the
 *  tag of the token. To prevent unnecessary bi-directional relationships, we shall not modify flags
 *  within PSubsystem from within this class.
 */
#include "SyntaxValidator.h"
#include <cassert>
#include <datatype/RegexPatterns.h>

using std::string;
using std::vector;

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
  bool output = tokens.capacity() == 8
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
  return tokens.capacity() == 2 && tokens.at(1).GetTokenTag() == TokenTag::kOpenBrace;
}
bool SyntaxValidator::ValidateWhileKeyword(const vector<Token>& tokens) {
  bool output = tokens.capacity() == 8
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
/**
 * Counts the number of tokens in a vector that matches a given target TokenTag
 * @param tokens tokens to count in
 * @param target_tag the target tag to meet
 * @return
 */
int SyntaxValidator::CountTokens(std::vector<Token> tokens, TokenTag target_tag) {
  int count = std::count_if(tokens.begin(), tokens.end(), [&target_tag](Token t) {
    return t.GetTokenTag() == target_tag;
  });
  return count;
}

/**
 * Counts the number of tokens in a vector that matches a given target TokenTag and string
 * @param tokens tokens to count in
 * @param target_tag the target tag to meet
 * @param target_string the string target to meet
 * @return
 */
int SyntaxValidator::CountTokens(std::vector<Token> tokens, TokenTag target_tag, std::string target_string) {
  int count = std::count_if(tokens.begin(), tokens.end(), [&target_tag, &target_string](Token t) {
    return t.GetTokenTag() == target_tag
        && t.GetTokenString() == target_string;
  });
  return count;
}

/**
 *  Returns an iterator for the tokens vector for the token that matches the regex for the desired patterns, this
 *  is to help identify where to recurse into.
 * @param tokens
 * @param desired_pattern
 * @param left_idx
 * @param right_idx
 * @return
 */
// forward iterator use:
auto SyntaxValidator::GetTokenMatchForwardIterator(const vector<Token>& tokens,
                                                   const std::regex& desired_pattern,
                                                   int left_idx,
                                                   int right_idx) {
  auto forward_iterator = std::find_if(tokens.begin() + left_idx,
                                       tokens.begin()
                                           + right_idx, // todo: check if the end is exclusive range or inclusive
                                       [&desired_pattern](Token elem) {
                                         string current = elem.GetTokenString();
                                         return std::regex_match(current, desired_pattern);
                                       });
  return forward_iterator;
}
// reverse iterator use:
/**
 * Takes in a range in the form of left and right idx which refers to the actual vector of tokens and within this,
 * finds the first token FROM THE END OF THE VECTOR that matches a desired pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx
 * @param right_boundary_idx
 * @return  the reverse iterator representing this first token
 */
auto SyntaxValidator::GetTokenMatchReverseIterator(const vector<Token>& tokens,
                                                   const std::regex& desired_pattern,
                                                   int left_boundary_idx,
                                                   int right_boundary_idx) {
  // from the given actual indices, get respective reverse iterators for the range
  auto rBeginning = tokens.crbegin() + ((tokens.size() - 1) - right_boundary_idx);
  auto rEnding = tokens.crend() - left_boundary_idx;
  // nb: find_if checks within a half-open range but we want inclusive behaviour:
  auto reverse_iterator = std::find_if(rBeginning,
                                       rEnding - 1, // todo: check if will throw out of bounds error;
                                       [&desired_pattern](Token elem) {
                                         string current = elem.GetTokenString();
                                         bool matches_target_pattern = std::regex_match(current, desired_pattern);
                                         return matches_target_pattern;
                                       });
  return reverse_iterator;
}

/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the last token that matches
 * the desired regex pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int SyntaxValidator::GetLastMatchingTokenIdx(const vector<Token>& tokens,
                                             const std::regex& desired_pattern,
                                             int left_boundary_idx,
                                             int right_boundary_idx) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchReverseIterator(tokens,
                                                                      desired_pattern,
                                                                      left_boundary_idx,
                                                                      right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}

/**
 *  A factor is either single variable name token, or an integer. If not, it's an expression surrounded by brackets.
 * @param statement_tokens  the reference statement tokens to inspect
 * @param left_boundary_idx  the left boundary of inspection  (inclusive range)
 * @param right_boundary_idx  the right boundary of inspection (inclusive range)
 * @return  true if specified range of tokens in statement_tokens makes up a Factor or not
 */
bool SyntaxValidator::IsFactor(const std::vector<Token>& statement_tokens,
                               int left_boundary_idx,
                               int right_boundary_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  // if there's only 1 token in range and that is a name or an integer
  if ((right_boundary_idx == left_boundary_idx)
      && (first_token_tag == TokenTag::kName || first_token_tag == TokenTag::kInteger)) {
    return true;
  } else if (first_token_tag
      == TokenTag::kOpenBracket) { // todo: check if the allowing of redundant brackets makes this code unreachable
    if (last_token_tag == TokenTag::kCloseBracket) {
      bool is_expression = IsExpr(statement_tokens,
                                  left_boundary_idx + 1,
                                  right_boundary_idx - 1);
      bool is_valid_factor_in_redundant_brackets =
          IsFactor(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
      return is_expression || is_valid_factor_in_redundant_brackets;
    } else {
      return false; // uneven bracketing
    }
  } else {
    return false;
  }
}

/**
 *  A Term is either single factor or it's combination of another sub-term and a factor, delimited by a specific set
 *  of delimiters (* or / or %).
 *  Assumption: the order doesn't matter, i.e. we don't care about  if <term><delim><factor> or <factor><delim><term>

 * @param statement_tokens  the reference statement tokens to inspect
 * @param left_boundary_idx  the left boundary of inspection  (inclusive range)
 * @param right_boundary_idx  the right boundary of inspection (inclusive range)
 * @return  true if specified range of tokens in statement_tokens makes up a Term
 */
bool SyntaxValidator::IsTerm(const vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
    return IsTerm(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
  }
  // case 1: whole range makes up a factor:
  if (IsFactor(statement_tokens, left_boundary_idx, right_boundary_idx)) {
    return true;
  }
  // case 2: <term><operator><factor>
  // find delimiter, if found and in range, the left to it is should be a term and right of it should be a factor
  int delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                          RegexPatterns::GetTermDelimiterPattern(),
                                          left_boundary_idx,
                                          right_boundary_idx);
  bool delim_idx_in_range = left_boundary_idx <= delim_idx && right_boundary_idx >= delim_idx;
  if (delim_idx_in_range) {
    if (delim_idx == left_boundary_idx || delim_idx == right_boundary_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    // checks right part first to fail early and prevent recursive loops
    bool right_part_is_factor = IsFactor(statement_tokens, delim_idx + 1, right_boundary_idx);
    if (!right_part_is_factor) {
      return false;
    }
    bool left_part_is_term = IsTerm(statement_tokens, left_boundary_idx, delim_idx - 1);
    return left_part_is_term;
  } else {
    return false;
  }
}
/**
 *  An Expression is either single term or it's combination of another sub-expression and a term, delimited by a specific set
 *  of delimiters (+ or / or -).
 *  Assumption: the order doesn't matter, i.e. we don't care about  if <term><delim><factor> or <factor><delim><term>

 * @param statement_tokens  the reference statement tokens to inspect
 * @param left_boundary_idx  the left boundary of inspection  (inclusive range)
 * @param right_boundary_idx  the right boundary of inspection (inclusive range)
 * @return  true if specified range of tokens in statement_tokens makes up an Expression
 */
bool SyntaxValidator::IsExpr(const vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx) {
  // case 0: accept and recurse if there are redundant set of brackets around current inspection range
  const Token& first_token = statement_tokens.at(left_boundary_idx);
  const Token& last_token = statement_tokens.at(right_boundary_idx);
  if (first_token.GetTokenTag() == TokenTag::kOpenBracket && last_token.GetTokenTag() == TokenTag::kCloseBracket) {
    return IsExpr(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
  }

  // case 1, it's a term, recurse early
  if (IsTerm(statement_tokens, left_boundary_idx, right_boundary_idx)) {
    return true;
  }
  // case 2: <expr><delim><term>
  auto delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                           RegexPatterns::GetExprDelimiterPattern(),
                                           left_boundary_idx,
                                           right_boundary_idx);
  bool delim_idx_in_range = left_boundary_idx <= delim_idx && right_boundary_idx >= delim_idx;
  if (delim_idx_in_range) {
    if (delim_idx == left_boundary_idx || delim_idx == right_boundary_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    bool right_part_is_term = IsTerm(statement_tokens, delim_idx + 1, right_boundary_idx);
    if (!right_part_is_term) {
      return false;
    }
    bool left_part_is_expr = IsExpr(statement_tokens, left_boundary_idx, delim_idx - 1);
    return left_part_is_expr;
  } else {
    return false;
  }
}

/**
 *  A relative factor is anything to the left or right of a boolean comparison operator. Note that a relative factor will be
 *  surrounded on one end by a boolean comparison operator and the other by an open/close bracket, however, the inspection
 *  range shall not have any of these tokens.
 *
 * @param statement_tokens
 * @param left_idx
 * @param right_idx
 * @return
 */
bool SyntaxValidator::IsRelFactor(const vector<Token>& statement_tokens, int left_idx, int right_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
    return IsRelFactor(statement_tokens, left_idx + 1, right_idx - 1);
  }
  bool is_factor = IsFactor(statement_tokens, left_idx, right_idx);// accepts redundant bracketing of expr e.g. ((x+1))
  bool is_expr = IsExpr(statement_tokens, left_idx, right_idx);  // not sure if redundant OR clause
  return is_factor || is_expr;
}

bool SyntaxValidator::IsRelExpr(const vector<Token>& statement_tokens, int left_idx, int right_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  // case 0: redundant brackets
  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
    return IsRelExpr(statement_tokens, left_idx + 1, right_idx - 1);
  }

  // case 1: <rel_expr> < binary comparison operator> < rel expr>
  int delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                          RegexPatterns::GetBinaryComparisonPattern(),
                                          left_idx,
                                          right_idx);
  bool delim_idx_in_range = left_idx <= delim_idx && right_idx >= delim_idx;
  if (delim_idx_in_range) {
    // todo: can deprecate this by adding to if predicate
    if (delim_idx == left_idx || delim_idx == right_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    bool right_part_is_rel_factor = IsRelFactor(statement_tokens, delim_idx + 1, right_idx);
    if (!right_part_is_rel_factor) {
      return false;
    }
    bool left_part_is_rel_factor = IsRelFactor(statement_tokens, left_idx, delim_idx - 1);
    return left_part_is_rel_factor;
  }
  return false;
}
bool SyntaxValidator::IsCondExpr(const vector<Token>& statement_tokens, int left_idx, int right_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_idx).GetTokenTag();
  // todo: not sure how to handle redundant bracket for because will wrongly remove outer brackets for (cond_expr) <bool> (cond_expr) because removing the outer ones
//           will give unequal bracketing
//  // case 0: redundant brackets
//  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
//    return IsCondExpr(statement_tokens, left_idx + 1, right_idx - 1);
//  }

  // case 1 it's a relative expr:
  if (IsRelExpr(statement_tokens, left_idx, right_idx)) {
    return true;
  }
  Token first_token = statement_tokens.at(left_idx);
  Token second_token = statement_tokens.at(left_idx + 1);
  // case 2: unary "not" operator, note that needs to be surrounded with an open bracket:
  // QQ: !x is invalid but !(x) is valid right
  bool first_token_is_unary_operator = std::regex_match(first_token.GetTokenString(),
                                                        RegexPatterns::GetUnaryBooleanOperatorPattern());
  if (first_token_is_unary_operator && second_token.GetTokenTag() == TokenTag::kOpenBracket) {
    return IsCondExpr(statement_tokens, left_idx + 1, right_idx); // keep the Open Bracket
  }
  // case 3: it's (<cond_expr>) <binary bool operator> (<cond_expr>)
  int delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                          RegexPatterns::GetBinaryBooleanOperatorPattern(),
                                          left_idx,
                                          right_idx);
  bool delim_idx_in_range = left_idx <= delim_idx && right_idx >= delim_idx;
  if (delim_idx_in_range) {
    // todo: can deprecate this by adding to if predicate
    if (delim_idx == left_idx || delim_idx == right_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    bool right_part_is_cond_expr = IsCondExpr(statement_tokens, delim_idx + 1, right_idx);
    if (!right_part_is_cond_expr) {
      return false;
    }
    bool left_part_is_cond_expr = IsCondExpr(statement_tokens, left_idx, delim_idx - 1);
    return left_part_is_cond_expr;
  }
  return false;
}



