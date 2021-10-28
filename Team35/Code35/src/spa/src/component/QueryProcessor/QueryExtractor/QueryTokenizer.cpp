#include "QueryTokenizer.h"
#include <utility>
#include <map>
#include <regex>
#include <datatype/RegexPatterns.h>
#include <exception/SpaException.h>

// note: order of regex evaluation matters! always retrieve key-values based on defined insertion_order.
std::vector<std::string> insertion_order = {"*", "STRING_QUOTE", "INTEGER", "SUCH_THAT", "PROG_LINE",
                                            "stmt#", "IDENT", ";", "SPACINGS", "(", ")", ",", "_", "<", ">", ".", "="};
static std::map<std::string, std::regex> spec_table{
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

/* Gets correct TokenTag specific to PQL applications. Allowed alphabet of TokenTags corresponds to specTable.
 * Note that this function does not check that the token is of SPACINGS type, as such tokens have already been dropped.
 */
TokenTag QueryTokenizer::GetPqlTokenType(std::string type) {
  // TODO: optimise this using a lookup table.
  if (type.compare("stmt#") == 0) { return TokenTag::kStmtHash; }
  if (type.compare("*") == 0) { return TokenTag::kTimes; }
  if (type.compare("STRING_QUOTE") == 0) { return TokenTag::kStringQuote; }
  if (type.compare("INTEGER") == 0) { return TokenTag::kInteger; }
  if (type.compare("PROG_LINE") == 0) { return TokenTag::kProgLine; }
  if (type.compare("SUCH_THAT") == 0) { return TokenTag::kSuchThat; }
  if (type.compare("IDENT") == 0) { return TokenTag::kName; }
  if (type.compare(";") == 0) { return TokenTag::kSemicolon; }
  if (type.compare("(") == 0) { return TokenTag::kOpenBracket; }
  if (type.compare(")") == 0) { return TokenTag::kCloseBracket; }
  if (type.compare(",") == 0) { return TokenTag::kComma; }
  if (type.compare("_") == 0) { return TokenTag::kUnderscore; }
  if (type.compare("<") == 0) { return TokenTag::kOpenKarat; }
  if (type.compare(">") == 0) { return TokenTag::kCloseKarat; }
  if (type.compare(".") == 0) { return TokenTag::kDot; }
  if (type.compare("=") == 0) { return TokenTag::kEquals; }

  return TokenTag::kInvalid;
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
