/**
 *  This file contains the implementation for syntax validation for a single token, based on the
 *  tag of the token. To prevent unnecessary bi-directional relationships, we shall not modify flags
 *  within PSubsystem from within this class.
 */
#include "SyntaxValidator.h"
#include <cassert>
#include <datatype/RegexPatterns.h>
#include <stack>

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
  Token first_token = statement_tokens.at(0);
  if (Token::IsKeywordToken(first_token)) {
    switch (first_token.GetTokenTag()) {
      case TokenTag::kProcedureKeyword:return ValidateProcedureSyntax(statement_tokens);
      case TokenTag::kReadKeyword:
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
  return false; // fall through
}

/**
 * Validates Procedure statements.
 * Please note that for things like else statements, such a fixed way of handling things depends on the fact that the
 * Parser insert newlines at the end of every `{`, `}`, `;` symbols
 * @param tokens
 * @return
 */
bool SyntaxValidator::ValidateProcedureSyntax(const vector<Token>& tokens) {
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kOpenBrace;
}

/**
 * Validates read, print and call statements
 * @param tokens
 * @return
 */
bool SyntaxValidator::ValidateMacroFunctionSyntax(const vector<Token>& tokens) {
  return tokens.capacity() == 3
      && tokens.at(1).GetTokenTag() == TokenTag::kName
      && tokens.at(2).GetTokenTag() == TokenTag::kSemicolon;
}

/**
 * Validates an If statement. Checks the following conditions:
 * - If statement must contain a Then keyword
 * - There's no accidental vector out of boundary access (transposing of vector error)
 * @param tokens
 * @return
 */
bool SyntaxValidator::ValidateIfStatementSyntax(const vector<Token>& tokens) {
  bool guarantee = tokens.at(0).GetTokenTag() == TokenTag::kIfKeyword;
  assert(guarantee);

  bool contains_single_then_keyword = Token::CountTokens(tokens, TokenTag::kThenKeyword) == 1;
  bool terminal_token_is_open_brace = tokens.at(tokens.size() - 1).GetTokenTag() == TokenTag::kOpenBrace;
  if (!contains_single_then_keyword || !terminal_token_is_open_brace) {
    return false;
  }
  // [TODO iter 2]: [cosmetic] probably no need to use counting, need to see what the finder function returns if it doesn't exist
  int then_keyword_idx = Token::GetFirstMatchingTokenIdx(tokens, TokenTag::kThenKeyword);
  int first_open_bracket = Token::GetFirstMatchingTokenIdx(tokens, TokenTag::kOpenBracket);
  int last_close_bracket = Token::GetLastMatchingTokenIdx(tokens, TokenTag::kCloseBracket);
  bool valid_cond_expression = then_keyword_idx > 4 // min number of tokens required
      && tokens.at(then_keyword_idx - 1).GetTokenTag() == TokenTag::kCloseBracket // cond_expr surrounded by brackets
      && tokens.at(1).GetTokenTag() == TokenTag::kOpenBracket
      && IsCondExpr(tokens, first_open_bracket + 1, last_close_bracket - 1); // guaranteed in bound
  return valid_cond_expression;
}

/**
 * Validates Else statements in a fixed way.
 * Please note that for things like else statements, such a fixed way of handling things depends on the fact that the
 * Parser insert newlines at the end of every `{`, `}`, `;` symbols
 * @param tokens
 * @return
 */
bool SyntaxValidator::ValidateElseKeyword(const vector<Token>& tokens) {
  return tokens.capacity() == 2 && tokens.at(1).GetTokenTag() == TokenTag::kOpenBrace;
}

bool SyntaxValidator::ValidateWhileKeyword(const vector<Token>& tokens) {
  bool guarantee = tokens.at(0).GetTokenTag() == TokenTag::kWhileKeyword;
  assert(guarantee);
  int first_open_bracket_idx = Token::GetFirstMatchingTokenIdx(tokens, TokenTag::kOpenBracket);
  int last_close_bracket_idx = Token::GetLastMatchingTokenIdx(tokens, TokenTag::kCloseBracket);
  bool terminal_token_is_open_brace = tokens.at(tokens.size() - 1).GetTokenTag() == TokenTag::kOpenBrace;
  bool valid_cond_expression = tokens.size() > 4
      && first_open_bracket_idx + 1 <= last_close_bracket_idx - 1
      && first_open_bracket_idx == 1
      && IsCondExpr(tokens, first_open_bracket_idx + 1, last_close_bracket_idx - 1);
  return terminal_token_is_open_brace && valid_cond_expression;
}

/**
 * Validates whether the statement is an assignment statement
 * @param statement_tokens
 * @return
 */
bool SyntaxValidator::ValidateAssignmentSentenceSyntax(const vector<Token>& statement_tokens) {
  bool terminates_with_semicolon =
      statement_tokens.at(statement_tokens.size() - 1).GetTokenTag() == TokenTag::kSemicolon;
  if (!terminates_with_semicolon) {
    return false;
  }
  bool has_only_one_assignment_operator = Token::CountTokens(statement_tokens, TokenTag::kAssignmentOperator);
  if (!has_only_one_assignment_operator) {
    return false;
  }
  // validate the tokens to the right of the assignment operator as an expression;
  int assignment_operator_idx = Token::GetFirstMatchingTokenIdx(statement_tokens, TokenTag::kAssignmentOperator);
  int right_boundary = statement_tokens.size() - 2; // left of the semicolon
  if (right_boundary <= assignment_operator_idx) {
    return false; // boundary invariant check
  }
  return IsExpr(statement_tokens, assignment_operator_idx + 1, right_boundary);
}

bool SyntaxValidator::ValidateCloseBrace(const vector<Token>& tokens) {
  return tokens.size() == 1;
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
  if ((right_boundary_idx == left_boundary_idx) // there's only 1 token in range and that is a name or an integer
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
      return false; // uneven bracketing encountered
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
//    return IsTerm(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
    return IsFactor(statement_tokens, left_boundary_idx, right_boundary_idx);
  }
  /// case 1: whole range makes up a factor:
  if (IsFactor(statement_tokens, left_boundary_idx, right_boundary_idx)) {
    return true;
  }
  /// case 2: <term><operator><factor>
  int delim_idx = Token::GetLastMatchingTokenIdx(statement_tokens,
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
  /// case 0: accept and recurse if there are redundant set of brackets around current inspection range
  const Token& first_token = statement_tokens.at(left_boundary_idx);
  const Token& last_token = statement_tokens.at(right_boundary_idx);
  // check that first and last tokens are not operators
  bool first_token_is_expr_delim =
      std::regex_match(first_token.GetTokenString(), RegexPatterns::GetExprDelimiterPattern());
  bool last_token_is_expr_delim =
      std::regex_match(last_token.GetTokenString(), RegexPatterns::GetExprDelimiterPattern());
  if (first_token_is_expr_delim || last_token_is_expr_delim) {
    return false;
  }
  // if it's a set of extra brackets:
  if (first_token.GetTokenTag() == TokenTag::kOpenBracket && last_token.GetTokenTag() == TokenTag::kCloseBracket) {
    return IsExpr(statement_tokens, left_boundary_idx + 1, right_boundary_idx - 1);
  }
  /// case 1, it's a term, recurse early
  if (IsTerm(statement_tokens, left_boundary_idx, right_boundary_idx)) {
    return true;
  }
  std::stack<Token> bracket_stack;
  int middle_ptr = -1;
  /// case 2: <expr><delim><term>
  // sweep from right and check with stack
  for (int right_ptr = right_boundary_idx; right_ptr >= left_boundary_idx; right_ptr--) {
    const Token& current_token = statement_tokens.at(right_ptr);
    TokenTag current_tag = current_token.GetTokenTag();
    if (current_tag == TokenTag::kCloseBracket) {
      bracket_stack.push(current_token);
    } else if (current_tag == TokenTag::kOpenBracket) {
      if (bracket_stack.empty()) {
        return false;
      } else {
        bracket_stack.pop();
      }
    } else if (current_token.GetTokenString() == "+"
        || current_token.GetTokenString() == "-") {
      bool can_recurse = bracket_stack.empty();
      if (can_recurse) {
        // break because found my "middle"
        middle_ptr = right_ptr;
        break;
      }
    }
  }

  // guarantee: middle_ptr will always be init has been init:
  /*
   * Reason being that if there were no desired delims (+ or -) it would be a term
   * and if it wasn't a term then there would have been extra brackets
   */
  if (middle_ptr <= left_boundary_idx) {
    return false;
    throw std::invalid_argument("help me");
  }
  bool left_is_expr = IsExpr(statement_tokens, left_boundary_idx, middle_ptr - 1);
  bool right_is_term = IsTerm(statement_tokens, middle_ptr + 1, right_boundary_idx);
  return left_is_expr && right_is_term;
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
  // todo: try do let IsExpr handle the strpping and don't do any stripping here, just recurse into IsExpr regardless
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
  const Token& first_token = statement_tokens.at(left_boundary_idx);
  const Token& last_token = statement_tokens.at(right_boundary_idx);
  bool first_token_is_binary_comp_delim =
      std::regex_match(first_token.GetTokenString(), RegexPatterns::GetBinaryComparisonPattern());
  bool last_token_is_binary_comp_delim =
      std::regex_match(last_token.GetTokenString(), RegexPatterns::GetBinaryComparisonPattern());
  // because ! at the end is undesirable
  if (first_token_is_binary_comp_delim || last_token_is_binary_comp_delim) {
    return false;
  }

  int bracket_stack_indicator = 0;
  int middle_ptr = left_boundary_idx; // set invalid first
  for (; middle_ptr <= right_boundary_idx; middle_ptr++) {
    const Token& current_token = statement_tokens.at(middle_ptr);
    string current_token_string = current_token.GetTokenString();
    TokenTag current_tag = current_token.GetTokenTag();
    // case 1: it's openbracket
    if (current_tag == TokenTag::kOpenBracket) {
      bracket_stack_indicator++;
    } else if (current_tag == TokenTag::kCloseBracket) {
      if (bracket_stack_indicator == 0) {
        return false;
      } else {
        bracket_stack_indicator--;
      }
    } else if (std::regex_match(current_token_string,
                                RegexPatterns::GetBinaryComparisonPattern())) { // encountered delim
      if (bracket_stack_indicator == 0) {
        break;
      }
    }
  }

  if (middle_ptr >= right_boundary_idx) { // iterate thru, nothing hits
    return false; // cfm got operator
  } else {
    bool left_is_rel_factor = IsRelFactor(statement_tokens, left_boundary_idx, middle_ptr - 1);
    bool right_is_rel_factor = IsRelFactor(statement_tokens, middle_ptr + 1, right_boundary_idx);
    return left_is_rel_factor && right_is_rel_factor;
  }

}

bool SyntaxValidator::IsCondExpr(const vector<Token>& statement_tokens, int left_boundary_idx, int right_boundary_idx) {
  // [TODO iter 2]: not sure how to handle redundant bracket for cond expr because will wrongly remove outer brackets for
  //               (cond_expr) <bool> (cond_expr) because removing the outer ones
  //               will give unequal bracketing

  const Token& first_token = statement_tokens.at(left_boundary_idx);
  const Token& last_token = statement_tokens.at(right_boundary_idx);
  // check that first and last tokens are not operators
  // !(....) ==> still okay
  // &&(...) ==> fail
  bool first_token_is_binary_bool_delim =
      std::regex_match(first_token.GetTokenString(), RegexPatterns::GetBinaryBooleanOperatorPattern());
  bool last_token_is_bool_delim =
      std::regex_match(last_token.GetTokenString(), RegexPatterns::GetBooleanOperatorPattern());
  // because ! at the end is undesirable
  if (first_token_is_binary_bool_delim || last_token_is_bool_delim) {
    return false;
  }
  bool is_negated_expr = first_token.GetTokenString() == "!"
      && statement_tokens.at(left_boundary_idx + 1).GetTokenTag() == TokenTag::kOpenBracket
      && statement_tokens.at(right_boundary_idx).GetTokenTag() == TokenTag::kCloseBracket;
  if (is_negated_expr) {
    return IsCondExpr(statement_tokens, left_boundary_idx + 2, right_boundary_idx - 1);
  } else { // it's a binary boolean operation
    int bracket_stack_indicator = 0;
    int middle_ptr = left_boundary_idx; // set invalid first
    for (; middle_ptr <= right_boundary_idx; middle_ptr++) {
      const Token& current_token = statement_tokens.at(middle_ptr);
      string current_token_string = current_token.GetTokenString();
      TokenTag current_tag = current_token.GetTokenTag();
      // case 1: it's openbracket
      if (current_tag == TokenTag::kOpenBracket) {
        bracket_stack_indicator++;
      } else if (current_tag == TokenTag::kCloseBracket) {
        if (bracket_stack_indicator == 0) {
          return false;
        } else {
          bracket_stack_indicator--;
        }
      } else if (std::regex_match(current_token_string,
                                  RegexPatterns::GetBinaryBooleanOperatorPattern())) { // encountered && or ||
        if (bracket_stack_indicator == 0) {
          break;
        }
      }
    }
    if (middle_ptr >= right_boundary_idx) { // iterate thru, nothing hits
      return IsRelExpr(statement_tokens, left_boundary_idx, right_boundary_idx);
      // rel expr
    } else {
      bool left_is_cond_expr = IsCondExpr(statement_tokens, left_boundary_idx + 1, middle_ptr - 2);
      bool right_is_cond_expr = IsCondExpr(statement_tokens, middle_ptr + 2, right_boundary_idx - 1);
      return left_is_cond_expr && right_is_cond_expr;
    }

  }

}


/**
 * This runs the statement against a blacklist rule: \n
 * 1. Statement either ends with these possible tokens: \n \t\t
 *          ";", ==> read,print,call statement  | assignment statement \n \t\t
 *          "{" (opening of a new container), if | while | procedure \n \t\t
 *          "}" only token, closing \n\n
 * 2. If there are brackets, it means it's an if or while statement. In such a case, we'd need an even number of
 * brackets. \n
 *
 * Other blacklist rules should be applied depending on the type of statement it is actually.
 *
 * @param statement_tokens tokens that represent the entire sentence
 * @return
 */
bool SyntaxValidator::StatementPassesCommonBlacklistRules(const vector<Token>& statement_tokens) {
  // todo: check bracket using a stack instead of pure counting
  const Token& last_token = statement_tokens.at(statement_tokens.size() - 1);
  bool valid_last_token
      = std::regex_match(last_token.GetTokenString(), RegexPatterns::GetValidStatementTerminalToken());
  if (!valid_last_token) {
    return false;
  }
  bool brackets_are_balanced = true;
  int stack_indicator = 0;
  for (int i = 0; i < statement_tokens.size(); i++) {
    Token current_token = statement_tokens.at(i);
    TokenTag current_tag = current_token.GetTokenTag();
    if (current_tag == TokenTag::kOpenBracket) {
      stack_indicator++;
    } else if (current_tag == TokenTag::kCloseBracket) {
      stack_indicator--;
      if (stack_indicator < 0) {
        brackets_are_balanced = false;
      }
    }
  }
  if (stack_indicator != 0) {
    brackets_are_balanced = false;
  }
  if (!brackets_are_balanced) {
    return false;
  }
  bool name_tokens_are_delimited = VerifyNameTokensAreDelimited(statement_tokens);
  return name_tokens_are_delimited;
}
// verify that if there's more than one name token, they aren't side by side without any delimiter b/w them
bool SyntaxValidator::VerifyNameTokensAreDelimited(const vector<Token>& statement_tokens) {
  int num_k_names = Token::CountTokens(statement_tokens, TokenTag::kName);
  for (int i = 0, prev_k_name_idx = -2, curr_k_name_idx = -1;
       i < num_k_names && curr_k_name_idx + 1 <= statement_tokens.size() - 1;
       i++) {
    curr_k_name_idx = Token::GetFirstMatchingTokenIdx(statement_tokens,
                                                      TokenTag::kName,
                                                      curr_k_name_idx + 1,
                                                      statement_tokens.size() - 1);
    // false if out of bounds, or it's side by side
    if ((prev_k_name_idx >= 0 && curr_k_name_idx <= prev_k_name_idx + 1) // i.e. not first iteration of for loop
        || curr_k_name_idx > statement_tokens.size() - 1) {
      return false;
    }
    prev_k_name_idx = curr_k_name_idx;
  }
  return true;
}



