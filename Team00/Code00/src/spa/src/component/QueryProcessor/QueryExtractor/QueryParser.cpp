#include "QueryParser.h"
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>
#include <unordered_set>

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

// stmtRef: synonym | ‘_’ | INTEGER
// returns a tuple of values corresponding to <stmtref string, isSynonym, isTargetSyononym>
std::tuple<std::string, bool, bool> QueryParser::parse_stmtRef() {
  std::string stmtRef_string;
  bool is_synonym = false;
  bool is_target_synonym = false;
  std::string curr_lookahead = lookahead.GetTokenString();

  if (lookahead.GetTokenTag() == TokenTag::kName) {
    // parse as synonym
    // std::cout << "parsing stmtref with value: " + curr_lookahead + " as SYONONYM" << std::endl;
    // validate that synonym is known.
    for (Synonym& s : synonyms) {
      if (s.GetName() == curr_lookahead) {
        is_synonym = true;
        if (curr_lookahead == this->target.GetName()) {
          is_target_synonym = true;
        }
        break;
      }
    }
    if (!is_synonym) {
      throw PQLParseException("Unknown synonym supplied in clause.");
    }
  } else if (lookahead.GetTokenTag() == TokenTag::kInteger) {
    // parse as INTEGER
    // std::cout << "parsing stmtref with value: " + lookahead.GetTokenString() + " as INTEGER" << std::endl;
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
    // std::cout << "parsing stmtref with value: " + lookahead.GetTokenString() + " as UNDERSCORE" << std::endl;
  } else {
    throw PQLParseException("Incorrect stmtRef supplied in clause.");
  }
  eat(lookahead.GetTokenTag());
  return std::make_tuple(curr_lookahead, is_synonym, is_target_synonym);
}
std::pair<Clause*, bool> QueryParser::parse_relRef() {
  // std::cout << "parsing relress" << std::endl;
  // std::cout << lookahead.GetTokenString() << std::endl;
  std::unordered_set<std::string> relRefs = {"Follows", "Follows*", "Parent", "Parent*", "Uses", "Modifies"};
  // std::cout << "parsing relrefsss" << std::endl;
  std::unordered_set<std::string>::const_iterator got = relRefs.find(lookahead.GetTokenString());
  if (got == relRefs.end()) {
    throw PQLParseException("Invalid relRef in such that clause.");
  }
  std::string rel_type = lookahead.GetTokenString();
  eat(TokenTag::kName);
  eat(TokenTag::kOpenBracket);
  // std::cout << "parsing relref of type " + rel_type << std::endl;
  // std::cout << "lookahead: " + lookahead.GetTokenString() << std::endl;

  // TODO: add support in the future for modifiesP, usesP case: (entRef ‘,’ entRef)
  std::string lhs; bool is_syn; bool is_tgt_syn;
  std::tie(lhs, is_syn, is_tgt_syn) = parse_stmtRef();
  eat(TokenTag::kComma);
  std::string rhs; bool is_syn_; bool is_tgt_syn_;
  std::tie(lhs, is_syn_, is_tgt_syn_) = parse_stmtRef();

  eat(TokenTag::kCloseBracket);

  // temporary
  if (rel_type.compare("Uses") == 0 || rel_type.compare("Modifies") == 0) {
    rel_type = std::string(rel_type + "S");
  }

  // TODO: this could introduce a memory leak...
  Clause* cl = new SuchThat(lhs, rhs, GetRelRef(rel_type), is_syn, is_syn_);
  return std::make_pair(cl, is_tgt_syn || is_tgt_syn_);
}

void QueryParser::parse_such_that() {
  // TODO: add support for multiple relRefs (separated by and)
  eat(TokenTag::kSuchThat);
  std::pair<Clause*, bool> clause_info = parse_relRef();
  // add group.
  std::vector<Clause*> clauses;
  clauses.emplace_back(clause_info.first);
  Group group = Group(clauses, clause_info.second);
  groups.push_back(group);
}

void QueryParser::parse_pattern() {
  return;
}

void QueryParser::parse_select() {
  if (lookahead.GetTokenString().compare("Select") != 0) {
    throw PQLParseException("Expected \'Select\' keyword, instead got " + lookahead.GetTokenString());
  }
  eat(TokenTag::kName); // 'Select' is tokenized as a name.
  GetTarget();
  // if there are more tokens, we are expecting either such that or pattern clauses.
  if (lookahead.GetTokenTag() == TokenTag::kInvalid) {
    return;
  }
  // std::cout << "Expecting such that or pattern clause..." << std::endl;
  if (lookahead.GetTokenTag() == TokenTag::kSuchThat) {
    // std::cout << "parsing such that" << std::endl;
    parse_such_that();
  } else if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("pattern") == 0) {
    // std::cout << "parsing pattern" << std::endl;
    parse_pattern();
  } else {
    // std::cout << "Incorrect query. Expected such that or pattern clause but got lookahead: " + lookahead.GetTokenString() << std::endl;
    throw PQLParseException("Incorrect query. Expected such that or pattern clause.");
  }
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