#include "QueryParser.h"
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>

/* Checks that current lookahead has the same expected type. If valid, advances tokenizer to next lookahead.*/
Token QueryParser::eat(TokenTag token_type) {
  Token token = lookahead;
  if (token.GetTokenTag() == TokenTag::kInvalid) {
    // TODO: Add a mapping from TokenTag enum to its string representation, so we can print the expected type.
    throw PQLParseException("Unexpected end of input.");
  }
  if (token_type != token.GetTokenTag()) {
    throw PQLParseException("Unexpected token: " + token.GetTokenString());
  }
  // Advance tokenizer to next lookahead
  lookahead = tokenizer.GetNextToken();
  return token;
}

std::vector<Token> QueryParser::GetSynonyms() {
  // TODO: here, add support for multiple synonyms. e.g. variable v1, v2;
  // TODO: here, add support for constant type, since TokenTag for that will be int.
  auto token = eat(TokenTag::kName);
  std::vector<Token> tokens;
  tokens.push_back(token);
  return tokens;
}

void QueryParser::GetDeclaration() {
  std::string design_entity = lookahead.GetTokenString();
  // validate designEntity since alphabet is known.
  // TODO: Abstract this validation logic into a helper function.
  if (!regex_match(design_entity, RegexPatterns::GetDesignEntityPattern())) {
    throw PQLParseException("A synonym of unknown Design Entity type " + design_entity + " was declared.");
  }
  eat(TokenTag::kName); // advance lookahead after parsing designEntity.
  DesignEntity de = GetDesignEntity(design_entity);
  std::vector<Token> s = GetSynonyms();
  // populate synonyms list with Synonym objects.
  for (Token& t : s) {
    this->synonyms.emplace_back(t.GetTokenString(), de);
  }
}

void QueryParser::parse_declarations() {
  // a syntactically valid synonym list contains > 0 synonym declarations, each followed by a semicolon.
  GetDeclaration();
  eat(TokenTag::kSemicolon);
  while (lookahead.GetTokenString().compare("Select") != 0 && lookahead.GetTokenTag() != TokenTag::kInvalid ) {
    GetDeclaration();
    eat(TokenTag::kSemicolon);
  }
}

void QueryParser::GetTarget() {
   Token target_synonym = eat(TokenTag::kName);
   std::string target = target_synonym.GetTokenString();
   // target must be a known synonym
   bool hasSynonym = false;
   for (Synonym& t : synonyms) {
     if (t.GetName() == target) {
       hasSynonym = true;
       this->target = Synonym(target, t.GetType());
       break;
     }
   }
   if (!hasSynonym) {
     throw PQLParseException("Incorrect target synonym for \'Select\' query.");
   }
}

void QueryParser::parse_select() {
  if (lookahead.GetTokenString().compare("Select") != 0) {
    throw PQLParseException("Expected \'Select\' keyword, instead got " + lookahead.GetTokenString());
  }
  eat(TokenTag::kName); // 'Select' is tokenized as a name.
  // TODO: add more advanced implementation for select. Following implementation just handles 'Select <target>'
  GetTarget();
}

void QueryParser::parse_query() {
  // parse synonym declarations first, if they exist.
  if (lookahead.GetTokenString().compare("Select") != 0) {
    parse_declarations();
  }
  // TODO: more validation for synonyms (ie. no duplicate synonym)
  // parse select clause
  parse_select();
}

void QueryParser::parse() {
  // lookahead is used for LL(1) predictive parsing.
  lookahead = tokenizer.GetNextToken();
  parse_query();
}