#include "QueryTokenizer.h"
#include <utility>
#include <map>
#include <unordered_map>
#include <regex>
#include <datatype/RegexPatterns.h>
#include <exception/SpaException.h>

// note: order of regex evaluation matters! always retrieve key-values based on defined insertion_order.
std::vector<std::string> insertion_order = {"*", "STRING_QUOTE", "INTEGER", "SUCH_THAT", "PROG_LINE",
                                            "stmt#", "IDENT", ";", "SPACINGS", "(", ")", ",", "_", "<", ">", ".", "="};
static std::map<std::string, std::regex> spec_table {
    {"*", std::regex("^[*]")},
    {"STRING_QUOTE", std::regex("^\"")},
    {"INTEGER", RegexPatterns::GetIntegerPatternNonTerminating()},
    {"SUCH_THAT", std::regex("^such that")},
    {"PROG_LINE", std::regex("^prog_line")},
    {"stmt#", std::regex("^stmt#")},
    {"IDENT", RegexPatterns::GetNamePattern()}, // IDENT is TokenTag:kName
    {";", std::regex("^;")},
    {"SPACINGS", std::regex(R"(^[\n\r\s\t]+)")},
    {"(", std::regex("^[(]")},
    {")", std::regex("^[)]")},
    {",", std::regex("^,")},
    {"_", std::regex("^_")},
    {"<", std::regex("^[<]")},
    {">", std::regex("^[>]")},
    {".", std::regex("^[.]")},
    {"=", std::regex("^[=]")},
};

static std::unordered_map<std::string, TokenTag> spec_type_to_tag_table {
  {"stmt#", TokenTag::kStmtHash},
  {"*", TokenTag::kTimes},
  {"STRING_QUOTE", TokenTag::kStringQuote},
  {"INTEGER", TokenTag::kInteger},
  {"PROG_LINE", TokenTag::kProgLine},
  {"SUCH_THAT", TokenTag::kSuchThat},
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
  std::smatch match;
  for (auto const& sp: insertion_order) {
    auto spec = * spec_table.find(sp);
    if (!std::regex_search(curr_string, match, spec.second, std::regex_constants::match_continuous)) {
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
  if (!std::regex_search(curr_string, match, std::regex("^[^\"]*")), std::regex_constants::match_continuous) {
    throw PQLTokenizeException("could not find string quote delimiter in query stream.");
  }
  std::string matched_str = match[0].str();
  cursor += matched_str.size();
  return matched_str;
}
