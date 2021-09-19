#include "QueryTokenizer.h"
#include <utility>
#include <map>
#include <regex>
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>

// note: order of regex evaluation matters! always retrieve key-values based on defined insertion_order.
std::vector<std::string> insertion_order = {"+", "-", "%", "*", "/", "STRING_QUOTE", "INTEGER", "SUCH_THAT",
                                            "IDENT", ";", "SPACINGS", "(", ")", ",", "_"};
static std::map<std::string, std::regex> spec_table {
  // TODO: for performance optimization, group (+, -) and (%, *, /) together if separate regex is not required.
  {"+", std::regex("^[+]")},
  {"-", std::regex("^[-]")},
  {"%", std::regex("^[%]")},
  {"*", std::regex("^[*]")},
  {"/", std::regex("^[/]")},
  {"STRING_QUOTE", std::regex("^\"")},
  {"INTEGER", RegexPatterns::GetIntegerPatternNonTerminating()},
  {"SUCH_THAT", std::regex("^such that")},
  {"IDENT", RegexPatterns::GetNamePattern()}, // IDENT is TokenTag:kName
  {";", std::regex("^;")},
  {"SPACINGS", std::regex(R"(^[\n\r\s\t]+)")},
  {"(", std::regex("^[(]")},
  {")", std::regex("^[)]")},
  {",", std::regex("^,")},
  {"_", std::regex("^_")},
};

/* Gets correct TokenTag specific to PQL applications. Allowed alphabet of TokenTags corresponds to specTable.
 * Note that this function does not check that the token is of SPACINGS type, as such tokens have already been dropped.
 */
TokenTag QueryTokenizer::GetPqlTokenType(std::string type) {
  if (type.compare("+") == 0) { return TokenTag::kPlus; }
  if (type.compare("-") == 0) { return TokenTag::kMinus; }
  if (type.compare("%") == 0) { return TokenTag::kModulo; }
  if (type.compare("*") == 0) { return TokenTag::kTimes; }
  if (type.compare("/") == 0) { return TokenTag::kDivide; }
  if (type.compare("STRING_QUOTE") == 0) { return TokenTag::kStringQuote; }
  if (type.compare("INTEGER") == 0) { return TokenTag::kInteger; }
  if (type.compare("SUCH_THAT") == 0) { return TokenTag::kSuchThat; }
  if (type.compare("IDENT") == 0) { return TokenTag::kName; }
  if (type.compare(";") == 0) { return TokenTag::kSemicolon; }
  if (type.compare("(") == 0) { return TokenTag::kOpenBracket; }
  if (type.compare(")") == 0) { return TokenTag::kCloseBracket; }
  if (type.compare(",") == 0) { return TokenTag::kComma; }
  if (type.compare("_") == 0) { return TokenTag::kUnderscore; }

  return TokenTag::kInvalid;
}

bool QueryTokenizer::HasMoreTokens() {
  return cursor < query.size();
}

Token QueryTokenizer::GetNextToken() {
  if (!HasMoreTokens()) {
    return Token("", TokenTag::kInvalid);
  }
  std::string curr_string = query.substr(cursor);
  std::smatch match;
  for (auto const& sp : insertion_order) {
    auto spec = *spec_table.find(sp);
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
