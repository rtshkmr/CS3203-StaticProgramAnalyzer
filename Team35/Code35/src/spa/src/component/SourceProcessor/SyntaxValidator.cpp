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
 * Validates syntax within a single statement by referring to the token tags and adhering to various types of statements
 * rules.
 *
 * @param statement_tokens
 * @return
 */
bool SyntaxValidator::ValidateSyntax(vector<Token> statement_tokens) {
  bool passes_generic_blacklist_rules = StatementPassesCommonBlacklistRules(statement_tokens);
  if (!passes_generic_blacklist_rules) {
    return false;
  }

  // check starting token to determine what kind of keyword it is and handle subcases in separate private functions:
  Token first_token = statement_tokens.at(0);
  if (Token::IsKeywordToken(first_token)) {
    switch (first_token.GetTokenTag()) {
      case TokenTag::kProcedureKeyword:return ValidateProcedureSyntax(statement_tokens);
      case TokenTag::kReadKeyword: // might need a better name to refer to a group of these keywords:
      case TokenTag::kPrintKeyword:
      case TokenTag::kCallKeyword:return ValidateMacroFunctionSyntax(statement_tokens);
      case TokenTag::kIfKeyword:return ValidateIfStatementSyntax(statement_tokens);
      case TokenTag::kElseKeyword:return ValidateElseKeyword(statement_tokens);
      case TokenTag::kWhileKeyword:return ValidateWhileKeyword(statement_tokens);
      default: {
        assert(false);
      }
    }
  } else if (first_token.GetTokenTag() == TokenTag::kName) { // it's an assignment statement
    return ValidateAssignmentSentenceSyntax(statement_tokens);
  } else if (first_token.GetTokenTag() == TokenTag::kCloseBrace) {
    return ValidateCloseBrace(statement_tokens);
  } else { // any other case returns false
    return false;
  }
}

bool SyntaxValidator::ValidateProcedureSyntax(const vector<Token>& tokens) {
  // QQ: is this part (fixed number of tokens for every type of statement a valid guarantee? Feels like it's
  //     very hardcode-y though :(
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kOpenBrace;
}
// for read, print and call keywords
bool SyntaxValidator::ValidateMacroFunctionSyntax(const vector<Token>& tokens) {
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kSemicolon;
}
bool SyntaxValidator::ValidateIfStatementSyntax(const vector<Token>& tokens) {
  bool contains_then_keyword = CountTokens(tokens, TokenTag::kThenKeyword) == 1;
  bool guarantee = tokens.at(0).GetTokenTag() == TokenTag::kIfKeyword;
  assert(guarantee);
  if (!contains_then_keyword) {
    return false;
  }
  // todo: probably no need to use counting, need to see what the finder fucntion returns if it doesn't exist
  int then_keyword_idx = GetFirstMatchingTokenIdx(tokens, TokenTag::kThenKeyword);
  int first_open_bracket = GetFirstMatchingTokenIdx(tokens, TokenTag::kOpenBracket);
  int last_close_bracket = GetLastMatchingTokenIdx(tokens, TokenTag::kCloseBracket);
  bool valid_cond_expression = then_keyword_idx > 4 // min number of tokens required
      && tokens.at(then_keyword_idx - 1).GetTokenTag() == TokenTag::kCloseBracket // cond_expr surrounded by brackets
      && tokens.at(1).GetTokenTag() == TokenTag::kOpenBracket
      && IsCondExpr(tokens, first_open_bracket + 1, last_close_bracket - 1); // guranteed in bound
  return valid_cond_expression;
}

bool SyntaxValidator::ValidateElseKeyword(const vector<Token>& tokens) {
  return tokens.capacity() == 2 && tokens.at(1).GetTokenTag() == TokenTag::kOpenBrace;
}
bool SyntaxValidator::ValidateWhileKeyword(const vector<Token>& tokens) {
  bool guarantee = tokens.at(0).GetTokenTag() == TokenTag::kWhileKeyword;
  assert(guarantee);
  int first_open_bracket_idx = GetFirstMatchingTokenIdx(tokens, TokenTag::kOpenBracket);
  int last_close_bracket_idx = GetLastMatchingTokenIdx(tokens, TokenTag::kCloseBracket);
  bool valid_cond_expression = tokens.size() > 4
      && first_open_bracket_idx + 1 <= last_close_bracket_idx - 1
      && first_open_bracket_idx == 1
      && IsCondExpr(tokens, first_open_bracket_idx + 1, last_close_bracket_idx - 1);
  return valid_cond_expression;
}

/**
 * Validates whether the statement is an assignment statement
 * @param statement_tokens
 * @return
 */
bool SyntaxValidator::ValidateAssignmentSentenceSyntax(const vector<Token>& statement_tokens) {
  bool terminates_with_semicolon =
      statement_tokens.at(statement_tokens.size() - 1).GetTokenTag() == TokenTag::kSemicolon;
  assert(terminates_with_semicolon == true); // because the generic blacklist would have run by this time
  bool has_only_one_assignment_operator = CountTokens(statement_tokens, TokenTag::kAssignmentOperator);
  if (!has_only_one_assignment_operator) {
    return false;
  }
  // validate the tokens to the right of the assignment operator as an expression;
  int assignment_operator_idx = GetFirstMatchingTokenIdx(statement_tokens, TokenTag::kAssignmentOperator);
  int right_boundary = statement_tokens.size() - 2; // left of the semicolon
  if (right_boundary <= assignment_operator_idx) {
    return false; // boundary invariant check
  }
  return IsExpr(statement_tokens, assignment_operator_idx + 1, right_boundary);
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
                                           + right_idx, // todo: check if the end is exclusive range or inclusive, it's half open, the ending is excluded
                                       [&desired_pattern](Token elem) {
                                         string current = elem.GetTokenString();
                                         bool matches_target_pattern = std::regex_match(current, desired_pattern);
                                         return matches_target_pattern;
                                       });
  return forward_iterator;
}
auto SyntaxValidator::GetTokenMatchForwardIterator(const vector<Token>& tokens,
                                                   TokenTag target_token_tag,
                                                   int left_idx,
                                                   int right_idx) {
  auto forward_iterator = std::find_if(tokens.begin() + left_idx,
                                       tokens.begin() + right_idx,
      // todo: check if the end is exclusive range or inclusive, it's half open, the ending is excluded
                                       [&target_token_tag](Token elem) {
                                         TokenTag current_tag = elem.GetTokenTag();
                                         bool matches_target_token = current_tag == target_token_tag;
                                         return matches_target_token;
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
auto SyntaxValidator::GetTokenMatchReverseIterator(const vector<Token>& tokens,
                                                   TokenTag target_token_tag,
                                                   int left_boundary_idx,
                                                   int right_boundary_idx) {
  // from the given actual indices, get respective reverse iterators for the range
  auto rBeginning = tokens.crbegin() + ((tokens.size() - 1) - right_boundary_idx);
  auto rEnding = tokens.crend() - left_boundary_idx;
  // nb: find_if checks within a half-open range but we want inclusive behaviour:
  auto reverse_iterator = std::find_if(rBeginning,
                                       rEnding - 1, // todo: check if will throw out of bounds error;
                                       [&target_token_tag](Token elem) {
                                         string current = elem.GetTokenString();
                                         TokenTag current_tag = elem.GetTokenTag();
                                         bool matches_target_token = current_tag == target_token_tag;
                                         return matches_target_token;
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
 * For a given vector of tokens, returns the index of the last token that matches
 * the desired regex pattern in the entire vector.
 * @param tokens
 * @param desired_pattern
 * @return
 */
int SyntaxValidator::GetLastMatchingTokenIdx(const vector<Token>& tokens,
                                             const std::regex& desired_pattern) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchReverseIterator(tokens,
                                                                      desired_pattern,
                                                                      0,
                                                                      tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}

/**
 * For a given vector of tokens, returns the index of the last token that matches
 * the desired regex pattern in the entire vector.
 * @param tokens
 * @param desired_pattern
 * @return
 */
int SyntaxValidator::GetLastMatchingTokenIdx(const vector<Token>& tokens,
                                             TokenTag target_token_tag) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchReverseIterator(tokens,
                                                                      target_token_tag,
                                                                      0,
                                                                      tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}

/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the last token that matches
 * the target token tag.
 * @param tokens
 * @param target_token_tag
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int SyntaxValidator::GetLastMatchingTokenIdx(const vector<Token>& tokens,
                                             TokenTag target_token_tag,
                                             int left_boundary_idx,
                                             int right_boundary_idx) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchReverseIterator(tokens,
                                                                      target_token_tag,
                                                                      left_boundary_idx,
                                                                      right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}

/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the first token that matches
 * the desired regex pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int SyntaxValidator::GetFirstMatchingTokenIdx(const vector<Token>& tokens,
                                              const std::regex& desired_pattern,
                                              int left_boundary_idx,
                                              int right_boundary_idx) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchForwardIterator(tokens,
                                                                      desired_pattern,
                                                                      left_boundary_idx,
                                                                      right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator); // todo: check delim_idx
  return delim_idx;
}

/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the first token that matches
 * the desired target token tag.
 * @param tokens
 * @param token_tag
 * @param left_boundary_idx
 * @param right_boundary_idx
 * @return
 */
int SyntaxValidator::GetFirstMatchingTokenIdx(const vector<Token>& tokens,
                                              TokenTag token_tag,
                                              int left_boundary_idx,
                                              int right_boundary_idx) {
  auto delim_iterator = SyntaxValidator::GetTokenMatchForwardIterator(tokens,
                                                                      token_tag,
                                                                      left_boundary_idx,
                                                                      right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator); // todo: check delim_idx
  return delim_idx;
}

/**
 * Returns first matching token based on the desired regex pattern within the entire vector of tokens
 * @param tokens
 * @param desired_pattern
 * @return
 */
int SyntaxValidator::GetFirstMatchingTokenIdx(const vector<Token>& tokens,
                                              const std::regex& desired_pattern) {
  assert(!tokens.empty());
  auto delim_iterator = SyntaxValidator::GetTokenMatchForwardIterator(tokens,
                                                                      desired_pattern,
                                                                      0,
                                                                      tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator); // todo: check delim_idx
  return delim_idx;
}
/**
 * Returns first matching token based on the desired regex pattern within the entire vector of tokens
 * @param tokens
 * @param target_token_tag
 * @return
 */
int SyntaxValidator::GetFirstMatchingTokenIdx(const vector<Token>& tokens,
                                              TokenTag target_token_tag) {
  assert(!tokens.empty());
  auto delim_iterator = SyntaxValidator::GetTokenMatchForwardIterator(tokens,
                                                                      target_token_tag,
                                                                      0,
                                                                      tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator); // todo: check delim_idx
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
  TokenTag last_token_tag = statement_tokens.at(right_boundary_idx).GetTokenTag();
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
  TokenTag last_token_tag = statement_tokens.at(right_boundary_idx).GetTokenTag();
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
 * @param left_boundary_idx
 * @param right_boundary_idx
 * @return
 */
bool SyntaxValidator::IsRelFactor(const vector<Token>& statement_tokens,
                                  int left_boundary_idx,
                                  int right_boundary_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(right_boundary_idx).GetTokenTag();
  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
    return IsRelFactor(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
  }
  bool is_factor = IsFactor(statement_tokens,
                            left_boundary_idx,
                            right_boundary_idx);// accepts redundant bracketing of expr e.g. ((x+1))
  bool is_expr = IsExpr(statement_tokens, left_boundary_idx, right_boundary_idx);  // not sure if redundant OR clause
  return is_factor || is_expr;
}

bool SyntaxValidator::IsRelExpr(const vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(right_boundary_idx).GetTokenTag();
  // case 0: redundant brackets
  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
    return IsRelExpr(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
  }

  // case 1: <rel_expr> < binary comparison operator> < rel expr>
  int delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                          RegexPatterns::GetBinaryComparisonPattern(),
                                          left_boundary_idx,
                                          right_boundary_idx);
  bool delim_idx_in_range = left_boundary_idx <= delim_idx && right_boundary_idx >= delim_idx;
  if (delim_idx_in_range) {
    // todo: can deprecate this by adding to if predicate
    if (delim_idx == left_boundary_idx || delim_idx == right_boundary_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    bool right_part_is_rel_factor = IsRelFactor(statement_tokens, delim_idx + 1, right_boundary_idx);
    if (!right_part_is_rel_factor) {
      return false;
    }
    bool left_part_is_rel_factor = IsRelFactor(statement_tokens, left_boundary_idx, delim_idx - 1);
    return left_part_is_rel_factor;
  }
  return false;
}
bool SyntaxValidator::IsCondExpr(const vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx) {
  TokenTag first_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  TokenTag last_token_tag = statement_tokens.at(left_boundary_idx).GetTokenTag();
  // todo: not sure how to handle redundant bracket for because will wrongly remove outer brackets for (cond_expr) <bool> (cond_expr) because removing the outer ones
//           will give unequal bracketing
//  // case 0: redundant brackets
//  if (first_token_tag == TokenTag::kOpenBracket && last_token_tag == TokenTag::kCloseBracket) {
//    return IsCondExpr(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
//  }

  // case 1 it's a relative expr:
  if (IsRelExpr(statement_tokens, left_boundary_idx, right_boundary_idx)) {
    return true;
  }
  Token first_token = statement_tokens.at(left_boundary_idx);
  Token second_token = statement_tokens.at(left_boundary_idx + 1);
  // case 2: unary "not" operator, note that needs to be surrounded with an open bracket:
  // QQ: !x is invalid but !(x) is valid right
  bool first_token_is_unary_operator = std::regex_match(first_token.GetTokenString(),
                                                        RegexPatterns::GetUnaryBooleanOperatorPattern());
  if (first_token_is_unary_operator && second_token.GetTokenTag() == TokenTag::kOpenBracket) {
    return IsCondExpr(statement_tokens, left_boundary_idx + 1, right_boundary_idx); // keep the Open Bracket
  }
  // case 3: it's (<cond_expr>) <binary bool operator> (<cond_expr>)
  int delim_idx = GetLastMatchingTokenIdx(statement_tokens,
                                          RegexPatterns::GetBinaryBooleanOperatorPattern(),
                                          left_boundary_idx,
                                          right_boundary_idx);
  bool delim_idx_in_range = left_boundary_idx <= delim_idx && right_boundary_idx >= delim_idx;
  if (delim_idx_in_range) {
    // todo: can deprecate this by adding to if predicate
    if (delim_idx == left_boundary_idx || delim_idx == right_boundary_idx) {
      // if first or last token is a delim (prevents out of range access in recursive calls)
      return false;
    }
    bool right_part_is_cond_expr = IsCondExpr(statement_tokens, delim_idx + 1, right_boundary_idx);
    if (!right_part_is_cond_expr) {
      return false;
    }
    bool left_part_is_cond_expr = IsCondExpr(statement_tokens, left_boundary_idx, delim_idx - 1);
    return left_part_is_cond_expr;
  }
  return false;
}

/**
 * This runs the statement against a blacklist rule:
 * 1. Statement either ends with these possible tokens:
 *          ";", ==> read,print,call statement  | assignment statement
 *          "{" (opening of a new container), if | while | procedure
 *          "}" only token, closing
 * 2. If there are brackets, it means it's an if or while statement. In such a case, we'd need an even number of
 * brackets.
 *
 * Other blacklist rules should be applied depending on the type of statement it is actually.
 *
 * @param statement_tokens tokens that represent the entire sentence
 * @return
 */
bool SyntaxValidator::StatementPassesCommonBlacklistRules(const vector<Token>& statement_tokens) {
  Token last_token = statement_tokens.at(statement_tokens.size() - 1);
  bool valid_last_token
      = std::regex_match(last_token.GetTokenString(), RegexPatterns::GetValidStatementTerminalToken());
  // for container statements (if and while statements)
  int num_open_brackets = CountTokens(statement_tokens, TokenTag::kOpenBracket);
  int num_close_brackets = CountTokens(statement_tokens, TokenTag::kCloseBracket);
  bool brackets_are_balanced = num_close_brackets == num_open_brackets;
  return valid_last_token && brackets_are_balanced;
}



