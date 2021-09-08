#include "Tokenizer.h"
#include <string>
#include <utility>
#include <vector>
#include <datatype/RegexPatterns.h>

using namespace std;

/**
 * Creates a vector of Token objects for a given string that represents a SIMPLE statement. Syntax validation
 * is done for every token but semantic (order of tokens and such rules) are not validated.
 * @param statement_string A SIMPLE statement in its string form
 * @return A vector of Token objects that represent that SIMPLE statement
 */
vector<Token> Tokenizer::CreateTokens(const string& statement_string) {
  vector<Token> tokens;
  vector<string> split_strings = SplitString(" ", statement_string, false);

  for (auto& string_token : split_strings) {
    if (IsWhiteSpace(string_token)) {
      continue; // ignore white spaces
    }

    // example: "abc=1;"
    vector<string> split_sub_tokens_strings = SplitSubTokenStrings(string_token);
    for (auto& sub_token_string: split_sub_tokens_strings) {
      tokens.push_back(*(CreateToken(sub_token_string)));
    }



//    if (split_sub_tokens_strings.empty()) {
//      tokens.push_back(*(CreateToken(string_token)));
//    } else {
//      for (auto& sub_token_string: split_sub_tokens_strings) {
//        tokens.push_back(*(CreateToken(sub_token_string)));
//      }
//    }
  }
  return tokens;
}

Token* Tokenizer::CreateToken(string token_string) {
  TokenTag token_tag = Token::TagStringWithToken(token_string);
  return new Token(std::move(token_string), token_tag);
}

/**
 *  Splits a given string via the delimiter provided and retains the delimiter if @param retain_delimiter is true
 * @param delimiter string representation of the delimiter to split the string by
 * @param input the reference string to split
 * @param retain_delimiter  retains the delimiter within the result vector if set to true
 * @return a vector of strings representing the input string split at the delimiters
 */
vector<string> Tokenizer::SplitString(const string& delimiter, const string& input, bool retain_delimiter) {
  vector<string> split_strings;
  auto start = 0U;
  auto end = input.find(delimiter); // first occurance of delimiter
  while (end != string::npos) {
    string split_string = input.substr(start, end - start);
    if (!IsWhiteSpace(split_string)) {
      split_strings.push_back(split_string);
    }
    if (retain_delimiter) {
      split_strings.push_back(delimiter);
    }
    start = end + delimiter.length();
    end = input.find(delimiter, start);
  }
  // if there are no delimiters in the input, return:
  if (start >= input.size()) { // i.e. scanned the whole string
    return split_strings;
  } else {
    string substr = input.substr(start, end);
    split_strings.push_back(substr);
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
vector<string> Tokenizer::SplitSubTokenStrings(string& string_token) {
  // base case: string of one char or is a special delim
  vector<string> result;
  if (/*IsSpecialDelimiter(string_token) || */string_token.size() == 1) {
    result.push_back(string_token);
    return (result);
  } else {
    // the order of the delimiters matter here, use the 2-char comparison operators first:
    std::string special_delimiters[] = {
        "{", "(", ")", ";", "}", // important delims
        "==", ">=", "<=", "!=", // double char comparison operators
        ">",
        "=", "<", // single char comparison operators
        "+", "-", "*", "%", "/",
    };
    for (auto& delimiter : special_delimiters) {
      int delim_idx = string_token.find(delimiter);
      if (delim_idx != string::npos) { // found first valid delim:
        if (delim_idx > 0) { // left substring exists, no left substring if delim_idx == 0
          string left_substring = string_token.substr(0, delim_idx);
          vector<string> left_token_strings = SplitSubTokenStrings(left_substring);
          result.insert(result.end(), left_token_strings.begin(), left_token_strings.end());
        }
        result.push_back(delimiter);
        if (delim_idx < string_token.size() - 1) { // right substring exists:
          string right_substring = string_token.substr(delim_idx + 1, string_token.size() - 1 - delim_idx);
          vector<string> right_token_strings = SplitSubTokenStrings(right_substring);
          result.insert(result.end(), right_token_strings.begin(), right_token_strings.end());
        }
        break; // find first delim
      } else {

      }
    }
    // if code reaches here, it means there are no delimiters within the string_token, so the whole string_token
    // should be added to result:
  }
  if(result.empty()) { // wasn't a single string and didn't have any delim inside it
    result.push_back(string_token);
  }
  return result;
}

bool Tokenizer::IsSpecialDelimiter(string& test_string) {
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


