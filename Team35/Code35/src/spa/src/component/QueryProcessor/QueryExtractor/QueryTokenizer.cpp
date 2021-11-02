#include "QueryTokenizer.h"
#include <utility>
#include <map>
#include <unordered_map>
#include <regex>
#include <datatype/RegexPatterns.h>
#include <exception/SpaException.h>

static std::unordered_set<std::string> exact_tokens { "*", ";", "(", ")", ",", "_", "<", ">", ".", "=",
                                                      "stmt#", "prog_line", "such that", "\"" };

std::vector<std::string> insertion_order = { "SPACINGS", "IDENT", "INTEGER" };
static std::unordered_map<std::string, std::regex> spec_table {
    {"INTEGER", RegexPatterns::GetIntegerPatternNonTerminating()},
    {"IDENT", RegexPatterns::GetNamePattern()}, // IDENT is TokenTag:kName
    {"SPACINGS", std::regex(R"(^[\n\r\s\t]+)")},
};

static std::unordered_map<std::string, TokenTag> spec_type_to_tag_table {
  {"stmt#", TokenTag::kStmtHash},
  {"*", TokenTag::kTimes},
  {"\"", TokenTag::kStringQuote},
  {"INTEGER", TokenTag::kInteger},
  {"prog_line", TokenTag::kProgLine},
  {"such that", TokenTag::kSuchThat},
  {"IDENT", TokenTag::kName},
  {";", TokenTag::kSemicolon},
  {"(", TokenTag::kOpenBracket},
  {")", TokenTag::kCloseBracket},
  {",", TokenTag::kComma},
  {"_", TokenTag::kUnderscore},
  {"<", TokenTag::kOpenKarat},
  {">", TokenTag::kCloseKarat},
  {".", TokenTag::kDot},
  {"=", TokenTag::kEquals}
};

/**
 * Gets correct TokenTag specific to PQL applications by consulting spec_type_to_tag_table.
 * Note that this function does not check that the token is of SPACINGS type, as such tokens have already been dropped.
 * @param type is a string corresponding to a valid key in spec_table.
 * @return a TokenTag enum type.
 */
TokenTag QueryTokenizer::GetPqlTokenType(std::string type) {
  std::unordered_map<std::string, TokenTag>::const_iterator got = spec_type_to_tag_table.find(type);
  if (got == spec_type_to_tag_table.end()) {
    return TokenTag::kInvalid;
  }
  return got->second;
}

bool QueryTokenizer::HasMoreTokens() {
  return cursor < query.size();
}

/**
 * Tokenizes the next token based on the current cursor position in the query input stream through regex matching.
 * @return a Token object which contains information about the type and value of input tokenized.
 * Note that if there are no more tokens to tokenize (end of input), a token with non-meaningful fields is returned.
 */
Token QueryTokenizer::GetNextToken() {
  if (!HasMoreTokens()) {
    return Token("", TokenTag::kInvalid);
  }
  std::string curr_string = query.substr(cursor);
  // try matching known exact tokens first
  bool has_exact_token_match;
  Token exact_token = Token("", TokenTag::kInvalid);
  std::tie(has_exact_token_match, exact_token) = EvaluateExactTokenMatch(curr_string);
  if (has_exact_token_match) {
    cursor += exact_token.GetTokenString().size();
    return exact_token;
  }
  // not an exact token, so we need to evaluate with known regex patterns
  std::smatch match;
  for (auto const& sp: insertion_order) {
    auto spec = * spec_table.find(sp);
    if (!std::regex_search(curr_string, match, spec.second)) {
      continue;
    }
    cursor += match[0].str().size();
    if (spec.first.compare("SPACINGS") == 0) {
      return GetNextToken();
    }
    return Token(match[0].str(), GetPqlTokenType(spec.first));
  }

  throw PQLTokenizeException("No patterns matched. Error in tokenizing pql.");
}

std::pair<bool, Token> QueryTokenizer::EvaluateExactTokenMatch(std::string curr_string) {
  std::string match;
  std::unordered_set<std::string>::const_iterator got;
  // "such that" / "prog_line" are 9 chars long, "stmt#" is 5, "\"" is 2. Rest are 1.
  std::vector<int> lengths = {1, 2, 5, 9};
  for (int i : lengths) {
    match = curr_string.substr(0, i);
    got = exact_tokens.find(match);
    if (got != exact_tokens.end()) return {true, Token(match, spec_type_to_tag_table.at(match))};
  }
  // curr_string does not start with an exact token, return dummy
  return {false, Token("", TokenTag::kInvalid)};
}

/**
 * Allows caller to skip the tokenizing of the substring from the current cursor till
 * the first string quote delimiter "\"" is found.
 * @return the substring up till and excluding the delimiter, that the tokenizer skipped.
 */
std::string QueryTokenizer::SkipTokenizerTillStringQuoteDelimiter() {
  if (!HasMoreTokens()) {
    throw PQLTokenizeException("reached end of query stream before string quote delimiter was reached.");
  }
  std::string curr_string = query.substr(cursor);
  std::smatch match;
  if (!std::regex_search(curr_string, match, std::regex("^[^\"]*"))) {
    throw PQLTokenizeException("could not find string quote delimiter in query stream.");
  }
  std::string matched_str = match[0].str();
  cursor += matched_str.size();
  return matched_str;
}
