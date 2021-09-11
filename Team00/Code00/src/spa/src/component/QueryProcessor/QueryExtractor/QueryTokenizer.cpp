#include "QueryTokenizer.h"
#include <map>
#include <regex>
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>

static std::map<std::string, std::regex> spec_table {
  // note: order of regex evaluation matters! Be careful before reordering.
  {"SUCH_THAT", std::regex("^Such that")},
  {"IDENT", RegexPatterns::GetNamePattern()}, // IDENT is TokenTag:kName
  {";", std::regex("^;")},
  {"SPACINGS", std::regex("^\\s+")} // TODO: add tabs & newline (including carriage return) to regex
};

/* Gets correct TokenTag specific to PQL applications. Allowed alphabet of TokenTags corresponds to specTable.
 * Note that this function does not check that the token is of SPACINGS type, as such tokens have already been dropped.
 */
TokenTag QueryTokenizer::GetPqlTokenType(std::string type) {
  if (type.compare("IDENT") == 0) { return TokenTag::kName; }
  if (type.compare(";") == 0) { return TokenTag::kSemicolon; }

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
  for (auto const& spec : spec_table) {
    // x.first, x.second
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
