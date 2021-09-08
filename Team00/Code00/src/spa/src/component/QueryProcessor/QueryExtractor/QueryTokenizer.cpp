#include "QueryTokenizer.h"
#include <map>
#include <regex>
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>

static std::map<std::string, std::regex> specTable {
  {"IDENT", RegexPatterns::GetNamePattern()}, // IDENT is TokenTag:kName
  {";", std::regex("^;")},
  {"SPACINGS", std::regex("^\\s+")} // TODO: add tabs & newline (including carriage return) to regex
};

/* Gets correct TokenTag specific to PQL applications. Allowed alphabet of TokenTags corresponds to specTable.
 * Note that this function does not check that the token is of SPACINGS type, as such tokens have already been dropped.
 */
TokenTag QueryTokenizer::getPqlTokenType(std::string type) {
  if (type.compare("IDENT") == 0) { return TokenTag::kName; }
  if (type.compare(";") == 0) { return TokenTag::kSemicolon; }

  return TokenTag::kInvalid;
}

bool QueryTokenizer::hasMoreTokens() {
  return cursor < query.size();
}

Token QueryTokenizer::getNextToken() {
  if (!this->hasMoreTokens()) {
    return Token("", TokenTag::kInvalid);
  }
  std::string currString = query.substr(cursor);
  std::smatch match;
  for (auto const& spec : specTable) {
    // x.first, x.second
    if (!std::regex_search(currString, match, spec.second)) {
      continue;
    }
    cursor += match[0].str().size();
    if (spec.first.compare("SPACINGS") == 0) {
      return getNextToken();
    }
    return Token(match[0].str(), getPqlTokenType(spec.first));
  }

  throw PQLTokenizeException("No patterns matched. Error in tokenizing pql.");
}
