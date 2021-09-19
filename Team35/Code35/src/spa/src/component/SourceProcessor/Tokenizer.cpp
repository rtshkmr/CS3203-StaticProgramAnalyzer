#include "Tokenizer.h"
#include <string>
#include <utility>
#include <vector>

using namespace std;

vector<Token> Tokenizer::CreateTokens(const string& statement_string) {
  vector<Token> tokens;
  vector<string> token_strings = SplitSentenceIntoStringTokens(statement_string);
  for (int i = 0; i < token_strings.size(); i++) {
    string token_string = token_strings.at(i);
    Token* new_token = CreateToken(token_string, i, token_strings);
    tokens.push_back(* new_token);
  }
  return tokens;
}

/**
 * Given a single string representing a statement, splits into discernible tokens
 * @param statement_string
 * @return
 */
vector<string> Tokenizer::SplitSentenceIntoStringTokens(const string& statement_string) {
  vector<string> final_token_strings;
  vector<string> split_strings_by_space = SplitString(" ", statement_string, false);
  for (auto& string_token: split_strings_by_space) {
    if (!IsWhiteSpace(string_token)) {
      vector<string> split_sub_tokens_strings = SplitSubTokenStrings(string_token);
      for (auto& sub_token_string: split_sub_tokens_strings) {
        if (!IsWhiteSpace(sub_token_string))
          final_token_strings.push_back(sub_token_string);
      }
    }
  }
  return final_token_strings;
}

/**
 *  Creates a new Token object for a given string that represents a possible token
 * @param token_string is a non-whitespace character/string that will be tokenized
 * @return  pointer to a newly created Token object for a given token_string
 */
Token* Tokenizer::CreateToken(const string& token_string,
                              int token_idx,
                              const vector<string>& statement_token_strings) {
  TokenTag token_tag = Token::TagStringWithToken(token_string);
  string copied_str = token_string;
  auto* new_token = new Token(std::move(copied_str), token_tag);
  if (Token::IsKeywordToken(* new_token)
      && !Tokenizer::IsActualKeyword(token_string, token_idx, statement_token_strings)) {
    new_token = new Token(new_token->GetTokenString(), TokenTag::kName);
  }
  return new_token;
}

/**
 *  Splits a given string via the single delimiter provided and retains the delimiter if retain_delimiter is true
 * @param delimiter string representation of the delimiter to split the string by
 * @param input the reference string to split
 * @param retain_delimiter  retains the delimiter within the result vector if set to true
 * @return a vector of strings representing the input string split at the delimiters
 */
vector<string> Tokenizer::SplitString(const string& delimiter, const string& input, bool retain_delimiter) {
  vector<string> split_strings;
  auto left = 0U;
  auto right = input.find(delimiter);
  while (right != string::npos) {
    string split_string = input.substr(left, right - left);
    if (!IsWhiteSpace(split_string)) {
      split_strings.push_back(split_string);
    }
    if (retain_delimiter) {
      split_strings.push_back(delimiter);
    }
    left = right + delimiter.length();
    right = input.find(delimiter, left);
  }
  if (left >= input.size()) { // i.e. scanned the whole string, there are no delimiters in input
    return split_strings;
  } else {
    string substr = input.substr(left, right);
    split_strings.push_back(substr);
    // check if the delimiter appears at the right of the input string:
    string s;
    bool last_char_is_delimiter = (s + input[input.size() - 1]) == delimiter;
    if (retain_delimiter && last_char_is_delimiter) {
      split_strings.push_back(delimiter);
    }
    return split_strings;
  }
}

/**
 * Given a token_string, recursively looks for sub tokens that are nested within. For example, the string_token
 * "x=y+1;" would have "=" and ";" and delimiters to split the string_token by and this is to be done
 * recursively.
 * @param string_token
 * @param tokens
 * @return
 */
vector<string> Tokenizer::SplitSubTokenStrings(string& string_token) {
  // base case: string of one char or is a special delim
  vector<string> result;
  if (string_token.size() == 1) {
    result.push_back(string_token);
  } else { // recursive cases:
    // the order of the delimiters matter here, use the 2-char comparison operators first:
    std::string special_delimiters[] = {
        "{", "(", ")", ";", "}", // important containerizing delims
        "==", ">=", "<=", "!=", // double char comparison operators
        ">", "<", // single char comparison operators
        "=",
        "!",
        "+", "-", "*", "%", "/",
    };
    for (auto& delimiter: special_delimiters) {
      int delim_idx = string_token.find(delimiter);
      if (delim_idx != string::npos) { // found first valid delim:
        if (delim_idx > 0) { // left substring exists, no left substring if delim_idx == 0
          string left_substring = string_token.substr(0, delim_idx);
          vector<string> left_token_strings = SplitSubTokenStrings(left_substring);
          result.insert(result.end(), left_token_strings.begin(), left_token_strings.end());
        }
        result.push_back(delimiter);
        if (delim_idx < string_token.size() - 1) { // right substring exists:
          string right_substring = string_token.substr(delim_idx + delimiter.size(),
                                                       string_token.size() - 1 - delim_idx);
          vector<string> right_token_strings = SplitSubTokenStrings(right_substring);
          result.insert(result.end(), right_token_strings.begin(), right_token_strings.end());
        }
        break; // only need to find first delim to recurse
      } else {
      }
    }
  }
  // last base case: string_token was not a single char and didn't have any delim inside it
  if (result.empty()) {
    result.push_back(string_token);
  }
  return result;
}

/**
 * Checks whether test_string exactly matches a list of possible whitespace characters
 * @param test_string
 * @return
 */
bool Tokenizer::IsWhiteSpace(string& test_string) {
  if (test_string.empty()) return true;
  string targets[]{" "};
  for (auto& target: targets) {
    if (target == test_string) {
      return true;
    }
  }
  return false;
}

/**
 * Based on the other tokens in the same sentence, determines if a token is an actual keyword.
 * This is a whitelist routine instead of blacklist.
 * Note that the ideal way would have been to judge in the same routine instead of create a separate routine for retagging,
 * but this is intended to be a quick fix. TODO [COSMETIC]: iter 2, improve data flow
 * @param token_string
 * @param token_idx the idx of the token string with respect to the statement
 * @param token_strings
 * @return true if actual keyword and false if it should actually be tagged as a kName
 */
bool Tokenizer::IsActualKeyword(const std::string& token_string, int token_idx, const vector<string>& token_strings) {
  if (token_string == "procedure"
      || token_string == "read"
      || token_string == "print"
      || token_string == "call") {
    return token_idx == 0 && token_strings.size() == 3;
  } else if (token_string == "if") {
    bool stmt_contains_then_keyword =
        token_strings.end() != std::find(token_strings.begin(), token_strings.end(), "then");
    bool next_is_open_bracket = (token_idx + 1 < token_strings.size())
        && (token_strings.at(token_idx + 1) == "(");
    bool stmt_ends_with_open_brace = token_strings.at(token_strings.size() - 1) == "{";
    return token_idx == 0
        && stmt_contains_then_keyword
        && next_is_open_bracket
        && stmt_ends_with_open_brace;
  } else if (token_string == "then") {
    bool stmt_contains_if_keyword = token_strings.end() != std::find(token_strings.begin(), token_strings.end(), "if");
    bool is_penultimate_token = token_idx == token_strings.size() - 2;
    bool next_is_open_brace = (token_idx + 1 < token_strings.size())
        && (token_strings.at(token_idx + 1) == "{");
    return stmt_contains_if_keyword
        && next_is_open_brace
        && is_penultimate_token;
  } else if (token_string == "else") {
    bool ends_with_open_brace = token_strings.at(token_strings.size() - 1) == "{";
    return token_idx == 0
        && token_strings.size() == 2
        && ends_with_open_brace;
  } else if (token_string == "while") {
    bool next_is_open_bracket = (token_idx + 1 < token_strings.size())
        && (token_strings.at(token_idx + 1) == "(");
    bool stmt_ends_with_open_brace = token_strings.at(token_strings.size() - 1) == "{";
    return token_idx == 0
        && next_is_open_bracket
        && stmt_ends_with_open_brace;
  } else {
    return false;
  }
}


