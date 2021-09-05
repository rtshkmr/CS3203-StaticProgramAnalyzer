#include <iostream>
#include "Tokenizer.h"
#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/**
 * A static
 * @param statement_string A SIMPLE statement in its string form
 * @return A vector of Token objects that represent that SIMPLE statement
 */
vector<Token> Tokenizer::CreateTokens(const string& statement_string) {
  std::string delimiter = " ";
  vector<Token> tokens;
  vector<string> split_strings = SplitString(" ", statement_string, false);

  for (auto& string_token : split_strings) {
    // delimit by spaces first, and then search for more specific delimiters (kCloseBrace, kOpenBrace, kSemicolon)
    if (IsWhiteSpace(string_token)) {
      continue;
    }
    vector<string> split_sub_tokens_strings = SplitSubTokens(string_token);
    if (split_sub_tokens_strings.empty()) {
      tokens.push_back(*(CreateToken(string_token)));
    } else {
      for (auto& sub_token_string: split_sub_tokens_strings) {
        tokens.push_back(*(CreateToken(sub_token_string)));
      }
    }
  }
  return tokens;
}

Token* Tokenizer::CreateToken(string token_string) {
  TokenTag token_tag = Token::TagStringWithToken(token_string);
//  return new Token(std::move(token_string), TokenTag::kInteger);
  return new Token(std::move(token_string), token_tag);
}

vector<string> Tokenizer::SplitString(const string& delimiter, const string& input, bool retain_delimiter) {
  vector<string> split_strings;
  auto start = 0U;
  auto end = input.find(delimiter); // first occurance of delimiter
  while (end != string::npos) {
    string split_string = input.substr(start, end - start);
    if (!IsWhiteSpace(split_string)) {
      split_strings.push_back(split_string);
    }
//    split_strings.push_back(split_string);
    if (retain_delimiter) {
      split_strings.push_back(delimiter);
    }
    start = end + delimiter.length();
    end = input.find(delimiter, start);
  }
  string split_string = input.substr(start, end);
  // if there are no delimiters in the input, return:
  if (start >= input.size()) { // i.e. scanned the whole string
    return split_strings;
  } else {
    split_strings.push_back(split_string);
    // check if the delimiter appears at the end of the input string:
    string s;
    bool last_char_is_delimiter = (s + input[input.size() - 1]) == delimiter;
    if (retain_delimiter && last_char_is_delimiter) {
      split_strings.push_back(delimiter);
    }
    return split_strings;
  }
}

/**
 * Given a token_string, recursively checks if there are any other delimiters that are contained within it.
 * @param string_token
 * @param tokens
 * @return
 */
vector<string> Tokenizer::SplitSubTokens(string& string_token) {
  if (IsDelimiter(string_token)) { // if the input string exact matches a delimiter(i.e. "{" or "}" or ";"
    return {};
  } else {
    vector<string> extra_token_strings;
    string delimiters[]{"{","(",")", ";", "}"};
    for (auto& delimiter : delimiters) {
      bool contains_delim = (string_token.find(delimiter) != string::npos);
      // todo: add some invariant check: at any time, a string_token may only contain one delim e.g."proc{" and not "proc {;"
      if (contains_delim) { // found a valid delim:
        vector<string> new_token_strings = SplitString(delimiter, string_token, true);
        extra_token_strings.insert(extra_token_strings.end(), new_token_strings.begin(), new_token_strings.end());
        break; // run this once only(?)
      }
    }
    return extra_token_strings;
  }
}

bool Tokenizer::IsDelimiter(string& test_string) {
  string delimiters[]{"{", ";", "}"};
  for (auto& delimiter : delimiters) {
    if (delimiter == test_string) {
      return true;
    }
  }
  return false;
}

bool Tokenizer::IsWhiteSpace(string& test_string) {
  if (test_string.empty()) return true;
  string targets[]{" "};
  for (auto& target : targets) {
    if (target == test_string) {
      return true;
    }
  }
  return false;
}


