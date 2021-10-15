#include "QueryParser.h"
#include "QueryValidator.h"
#include <component/QueryProcessor/types/Exceptions.h>
#include <component/SourceProcessor/Tokenizer.h>
#include <component/SourceProcessor/SyntaxValidator.h>
#include <datatype/RegexPatterns.h>
#include <unordered_set>
#include <sstream>

/**
 * Checks that current lookahead has the same expected type. If valid, advances tokenizer to next lookahead.
 * @param token_type is the type of the lookahead token, of type TokenTag enum.
 * @return Token instance of the lookahead.
 */
Token QueryParser::Eat(TokenTag token_type) {
  Token token = lookahead;
  if (token.GetTokenTag() == TokenTag::kInvalid) {
    throw PQLParseException("Unexpected end of input.");
  }
  if (token_type != token.GetTokenTag()) {
    throw PQLParseException("Unexpected token: " + token.GetTokenString());
  }
  // Advance tokenizer to next lookahead
  lookahead = tokenizer.GetNextToken();
  return token;
}

/**
 * Helper function to obtain synonym info (ie type) given it exists in list of synonyms.
 * @param syn_name is a string representing the name of the synonym.
 * @param synonyms is a reference to a list of synonym pointers.
 * @return a Synonym object with the corresponding information, or a dummy Synonym with invalid fields if not exists.
 */
Synonym* QueryParser::GetSynonymInfo(std::string syn_name, std::list<Synonym*>* synonyms) {
  // Todo: Optimize.
  for (auto t: * synonyms) {
    if (t->GetName().compare(syn_name) == 0) {
      return t;
    }
  }
  return new Synonym("", DesignEntity::kInvalid);
};

/**
 * Parses the given synonyms within a synonym declaration.
 * @return a vector of Token objects representing the parsed synonyms.
 */
std::vector<Token> QueryParser::ParseSynonyms() {
  std::vector<Token> tokens;
  tokens.push_back(Eat(TokenTag::kName));
  while (lookahead.GetTokenTag() != TokenTag::kSemicolon) {
    Eat(TokenTag::kComma);
    tokens.push_back(Eat(TokenTag::kName));
  }
  return tokens;
}

/**
 * Parses a synonym declaration.
 */
void QueryParser::ParseDeclaration() {
  std::string design_entity = lookahead.GetTokenString();
  // validate designEntity since alphabet is known.
  if (!regex_match(design_entity, RegexPatterns::GetDesignEntityPattern())) {
    throw PQLParseException("A synonym of unknown Design Entity type " + design_entity + " was declared.");
  }
  Eat(TokenTag::kName); // advance lookahead after parsing designEntity.
  DesignEntity de = GetDesignEntity(design_entity);
  std::vector<Token> s = ParseSynonyms();
  // populate synonyms list with Synonym objects.
  for (Token& t: s) {
    if (synonyms_name_set.find(t.GetTokenString()) != synonyms_name_set.end()) {
      throw PQLValidationException("Duplicate synonym was declared.");
    }
    this->synonyms.emplace_back(new Synonym(t.GetTokenString(), de));
    this->synonyms_name_set.emplace(t.GetTokenString());
  }
}

/**
 * Parses all synonym declarations.
 */
void QueryParser::ParseDeclarations() {
  // a syntactically valid synonym list contains > 0 synonym declarations, each followed by a semicolon.
  ParseDeclaration();
  Eat(TokenTag::kSemicolon);
  while (lookahead.GetTokenString().compare("Select") != 0 && lookahead.GetTokenTag() != TokenTag::kInvalid) {
    ParseDeclaration();
    Eat(TokenTag::kSemicolon);
  }
}

// attrName : ‘procName’| ‘varName’ | ‘value’ | ‘stmt#’
void QueryParser::ParseAttrName(Synonym* s) {
  // determine the attribute type.
  Attribute attr;
  if (lookahead.GetTokenTag() == TokenTag::kStmtHash) {
    attr = Attribute::kStmtNumber;
    Eat(TokenTag::kStmtHash);
  } else if (lookahead.GetTokenTag() == TokenTag::kName) {
      attr = GetAttribute(lookahead.GetTokenString());
      if (attr == Attribute::kInvalid) {
        throw PQLParseException("Received unknown attrName: " + lookahead.GetTokenString());
      }
      Eat(TokenTag::kName);
  } else {
    throw PQLParseException("Received unknown attrName: " + lookahead.GetTokenString());
  }
  if (!QueryValidator::Is_Semantically_Valid_AttrRef(s, attr)) {
    throw PQLValidationException("Received semantically invalid AttrRef");
  }
  s->SetAttribute(attr);
}

// elem : synonym | attrRef
// attrRef : synonym ‘.’ attrName
void QueryParser::ParseElem() {
  // parse synonym portion
  Token target_synonym = Eat(TokenTag::kName);
  std::string target = target_synonym.GetTokenString();
  // target must be a known synonym
  if (synonyms_name_set.find(target) == synonyms_name_set.end()) {
    throw PQLParseException("Incorrect target synonym for \'Select\' query.");
  }

  Synonym* s = QueryParser::GetSynonymInfo(target, &synonyms);
  this->target_synonyms_list.push_back(s);
  this->target_synonyms_map.emplace(std::make_pair(s->GetName(), s));

  // handle case where we need to parse attrRef
  if (lookahead.GetTokenTag() == TokenTag::kDot) {
    Eat(TokenTag::kDot);
    ParseAttrName(s);
  }
}

// tuple: elem | ‘<’ elem ( ‘,’ elem )* ‘>’
void QueryParser::ParseTuple() {
  // parse a single element
  if (lookahead.GetTokenTag() == TokenTag::kName) {
    ParseElem();
  } else {
    // parse multiple elems within karat notation
    Eat(TokenTag::kOpenKarat);
    ParseElem();
    while (lookahead.GetTokenTag() != TokenTag::kCloseKarat) {
      Eat(TokenTag::kComma);
      ParseElem();
    }
    Eat(TokenTag::kCloseKarat);
  }
}

/**
 * Parses the target synonyms.
 */
void QueryParser::ParseTarget() {
  if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("BOOLEAN") == 0) {
    // parse boolean
    Eat(TokenTag::kName);
  } else {
    ParseTuple();
  }
}

// stmtRef: synonym | ‘_’ | INTEGER
/**
 * Parses a statement reference (StmtRef).
 * @returns a tuple of values corresponding to lookahead string, bool isSynonym, bool isTargetSyononym.
 */
std::tuple<std::string, bool, bool> QueryParser::ParseStmtRef() {
  bool is_synonym = false;
  bool is_target_synonym = false;
  std::string curr_lookahead = lookahead.GetTokenString();

  if (lookahead.GetTokenTag() == TokenTag::kName) {
    if (synonyms_name_set.find(curr_lookahead) == synonyms_name_set.end()) {
      throw PQLParseException("Unknown synonym supplied in clause.");
    }
    is_synonym = true;
    if (target_synonyms_map.find(curr_lookahead) != target_synonyms_map.end()) {
      is_target_synonym = true;
    }
  } else if (lookahead.GetTokenTag() == TokenTag::kInteger) {
    // parse as INTEGER
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
  } else {
    throw PQLParseException("Incorrect stmtRef supplied in clause.");
  }
  Eat(lookahead.GetTokenTag());
  return std::make_tuple(curr_lookahead, is_synonym, is_target_synonym);
}

// stmtRef: synonym | ‘_’ | INTEGER
// entRef: synonym | ‘_’ | ‘"’ IDENT ‘"’
/**
 * Parses a statement reference (StmtRef) or an entity reference (entRef). This method is called when there is
 * an ambiguity of whether we are parsing StmtRef or entRef (i.e. for lhs of UsesS vs UsesP, ModifiesS vs ModifiesP).
 * @returns a tuple of values corresponding to lookahead string, bool isSynonym, bool isTargetSyononym,
 * bool is_uses_or_modifies_p.
 */
std::tuple<std::string, bool, bool, bool> QueryParser::ParseStmtOrEntRef() {
  bool is_uses_or_modifies_p = false; // assume we are parsing stmtref (not UsesP or ModifiesP) in default cases.
  bool is_synonym = false;
  bool is_target_synonym = false;
  std::string curr_lookahead = lookahead.GetTokenString();

  // determine if EntRef for ambiguous case of SYNONYM
  if (lookahead.GetTokenTag() == TokenTag::kName) {
    if (synonyms_name_set.find(curr_lookahead) == synonyms_name_set.end()) {
      throw PQLParseException("Unknown synonym supplied in clause.");
    }
    DesignEntity de = QueryParser::GetSynonymInfo(curr_lookahead, &synonyms)->GetType();
    is_uses_or_modifies_p = de == DesignEntity::kProcedure || de == DesignEntity::kCall;
  }

  // determine if EntRef for unambiguous cases
  if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    is_uses_or_modifies_p = true;
  }

  // dispatch parsing to correct handler
  if (is_uses_or_modifies_p) {
    Token tok = ParseEntRef(false);
    is_synonym = IsValidSynonym(tok);
    is_target_synonym = target_synonyms_map.find(tok.GetTokenString()) != target_synonyms_map.end();
  } else {
    std::tie(curr_lookahead, is_synonym, is_target_synonym) = ParseStmtRef();
  }

  return std::make_tuple(curr_lookahead, is_synonym, is_target_synonym, is_uses_or_modifies_p);
}

/**
 * Parses a relationship reference (RelRef).
 * @returns a tuple of values corresponding to Clause object, bool isTargetSynonym.
 */
std::pair<Clause*, bool> QueryParser::ParseRelRef() {
  std::unordered_set<std::string> relRefs = {"Follows", "Parent", "Uses", "Modifies", "Calls", "Next", "Affects"};
  std::unordered_set<std::string>::const_iterator got = relRefs.find(lookahead.GetTokenString());
  if (got == relRefs.end()) {
    throw PQLParseException("Invalid relRef in such that clause.");
  }
  std::string rel_type = lookahead.GetTokenString();
  Eat(TokenTag::kName);
  // check if next character is *. If so, rel_type is transitive variant.
  if (lookahead.GetTokenTag() == TokenTag::kTimes) {
    rel_type += "*";
    Eat(TokenTag::kTimes);
  }
  Eat(TokenTag::kOpenBracket);

  // parse left-hand side
  std::string lhs;
  bool is_lhs_syn;
  bool is_lhs_tgt_syn;
  bool is_uses_or_modifies_p = false;

  // 3 cases for parsing lhs of relref
  if (rel_type.find("Calls") == 0) {
    Token tok = ParseEntRef(false);
    lhs = tok.GetTokenString();
    is_lhs_syn = IsValidSynonym(tok);
    is_lhs_tgt_syn = target_synonyms_map.find(tok.GetTokenString()) != target_synonyms_map.end();
  }
  else if (rel_type == "Uses" || rel_type == "Modifies") {
    std::tie(lhs, is_lhs_syn, is_lhs_tgt_syn, is_uses_or_modifies_p) = ParseStmtOrEntRef();
  }
  else {
    std::tie(lhs, is_lhs_syn, is_lhs_tgt_syn) = ParseStmtRef();
  }

  if (lhs == "_" && (rel_type == "Modifies" || rel_type == "Uses")) {
    throw PQLValidationException("Semantically invalid to have _ as first argument for " + rel_type);
  }

  Eat(TokenTag::kComma);

  // parse right-hand side
  std::string rhs;
  bool is_rhs_syn = false;
  bool is_rhs_tgt_syn = false;
  if (rel_type == "Uses" || rel_type == "Modifies" || rel_type.find("Calls") == 0) {
    Token tok = ParseEntRef(false);
    rhs = tok.GetTokenString();
    is_rhs_syn = IsValidSynonym(tok);
    is_rhs_tgt_syn = target_synonyms_map.find(tok.GetTokenString()) != target_synonyms_map.end();
  } else {
    std::tie(rhs, is_rhs_syn, is_rhs_tgt_syn) = ParseStmtRef();
  }

  if (rel_type.compare("Modifies") == 0 || rel_type.compare("Uses") == 0) {
    rel_type = is_uses_or_modifies_p ? std::string(rel_type + "P") : std::string(rel_type + "S");
  }

  Eat(TokenTag::kCloseBracket);

  // semantic validation of relRef.
  if (!QueryValidator::Is_Semantically_Valid_RelRef(lhs, rhs, GetRelRef(rel_type),
                                                    is_lhs_syn, is_rhs_syn, & synonyms)) {
    throw PQLValidationException("Received semantically invalid " + rel_type + " cl.");
  }

  // create clause object
  Clause* cl = new SuchThat(lhs, rhs, GetRelRef(rel_type), is_lhs_syn, is_rhs_syn);
  if (is_lhs_syn) {
    cl->first_synonym = QueryParser::GetSynonymInfo(lhs, &synonyms);
  }
  if (is_rhs_syn) {
    cl->second_synonym = QueryParser::GetSynonymInfo(rhs, &synonyms);
  }
  return std::make_pair(cl, is_lhs_tgt_syn || is_rhs_tgt_syn);
}

/**
 * Parses a such that clause.
 */
// suchthat-cl : ‘such that’ relCond
// relCond: relRef (‘and’ relRef)*
void QueryParser::ParseSuchThat() {
  Eat(TokenTag::kSuchThat);
  std::pair<Clause*, bool> clause_info = ParseRelRef();
  clauses.emplace_back(clause_info.first);
  // check for multiple relRefs (separated by 'and')
  while (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("and") == 0) {
    Eat(TokenTag::kName);
    clause_info = ParseRelRef();
    clauses.emplace_back(clause_info.first);
  }
}

/**
 * Parses a with clause
 */
void QueryParser::ParseWith() {
  // TODO: add support for multiple relRefs (separated by and)
  throw PQLParseException("Support for with clause not implemented yet.");
}

// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’
/**
 * Parses a Entity Reference (EntRef).
 * @param isPatternCl is a bool that is true if EntRef to parse is for pattern clause, false if its for such that.
 * @return a Token object with the corresponding information of the parsed entRef.
 */
Token QueryParser::ParseEntRef(bool isPatternCl) {
  std::string token_name;
  TokenTag token_type;
  if (lookahead.GetTokenTag() == TokenTag::kName) {
    // parse as known synonym. Synonym must be variable for pattern cl.
    if (isPatternCl && !IsValidSynonym(lookahead, DesignEntity::kVariable)) {
      throw PQLParseException("Unknown synonym received as entRef in lhs of pattern cl.");
    }
    if (!isPatternCl && !IsValidSynonym(lookahead)) {
      throw PQLParseException("Unknown synonym received as entRef in lhs of cl.");
    }
    token_name = Eat(TokenTag::kName).GetTokenString();
    token_type = TokenTag::kName;
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
    Eat(TokenTag::kUnderscore);
    token_name = "_";
    token_type = TokenTag::kUnderscore;
  } else {
    // parse as "’ IDENT ‘"
    Eat(TokenTag::kStringQuote);
    token_name = Eat(TokenTag::kName).GetTokenString();
    token_type = TokenTag::kName;
    Eat(TokenTag::kStringQuote);
  }
  return Token(token_name, token_type);
}

// expression-spec : ‘"‘ expr’"’ | ‘_’ ‘"’ expr ‘"’ ‘_’ | ‘_’
/**
 * Parses an expression-spec.
 * @return a pair of values corresponding to string expr-spec, bool is_exact_match.
 * expr-spec only includes '_' for the case that it is a wildcard; for partial match leading/trailing '_' is excluded.
 * is_exact_match is true if expr-spec is an exact match, or false if it is a wildcard/partial match.
 */
std::pair<std::string, bool> QueryParser::ParseExpressionSpec() {
  bool is_exact_match = false;
  bool is_partial_match = false;
  bool is_wildcard = false;
  std::stringstream rhs_ss;

  // case wildcard or partial match
  if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    Eat(TokenTag::kUnderscore);
    if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
      is_partial_match = true;
    } else {
      rhs_ss << '_';
      is_wildcard = true;
    }
  }

  // parse expr for case partial or exact match.
  if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    if (!is_partial_match) {
      is_exact_match = true;
    }
    std::string expr_candidate_str = tokenizer.SkipTokenizerTillStringQuoteDelimiter();
    std::vector<Token> toks = Tokenizer::CreateTokens(expr_candidate_str);
    bool is_expr = SyntaxValidator::IsExpr(toks, 0, toks.size() - 1);
    if (is_expr) {
      rhs_ss << expr_candidate_str;
    } else {
      throw PQLParseException("Expected valid expression-spec. Received: " + expr_candidate_str);
    }
    // advance tokenizer past string quote and optional underscore
    lookahead = tokenizer.GetNextToken();
    Eat(TokenTag::kStringQuote);
    if (is_partial_match) {
      Eat(TokenTag::kUnderscore);
    }
  }

  if (!(is_wildcard || is_exact_match || is_partial_match)) {
    throw PQLParseException("Invalid expression-spec was neither wildcard, partial nor exact match.");
  }

  return std::make_pair(rhs_ss.str(), is_exact_match);
}

bool QueryParser::IsValidSynonym(Token token) {
  if (token.GetTokenTag() != TokenTag::kName) {
    return 0;
  }
  std::string syn_name = token.GetTokenString();
  // syn_name must be a known synonym.
  return synonyms_name_set.find(syn_name) != synonyms_name_set.end();
}

bool QueryParser::IsValidSynonym(Token token, DesignEntity de) {
  if (token.GetTokenTag() != TokenTag::kName) {
    return 0;
  }
  std::string syn_name = token.GetTokenString();
  // syn_name must be a known synonym, and of permitted type.
  bool is_valid = false;
  for (auto s: synonyms) {
    if (s->GetName() == syn_name && s->GetType() == de) {
      is_valid = true;
      break;
    }
  }
  if (!is_valid) {
    return 0;
  }
  return 1;
}

/**
 * Parses the PatternCond section of a pattern clause.
 */
void QueryParser::ParsePatternCond() {
  // check if syn-assign valid
  if (!IsValidSynonym(lookahead, DesignEntity::kAssign)) {
    throw PQLParseException("Expected valid syn-assign for pattern cl, instead got " + lookahead.GetTokenString());
  }
  Token assn_tok = Token(lookahead.GetTokenString(), lookahead.GetTokenTag());

  Eat(TokenTag::kName); // eat 'syn-assign'
  Eat(TokenTag::kOpenBracket);
  // parse_lhs
  Token lhs_token = ParseEntRef(true);
  Eat(TokenTag::kComma);
  std::string lhs = lhs_token.GetTokenString();
  bool lhs_is_syn = IsValidSynonym(lhs_token, DesignEntity::kVariable);
  //parse_rhs
  std::pair<std::string, bool> rhs_info = ParseExpressionSpec();
  Eat(TokenTag::kCloseBracket);

  // create clause object
  Clause* cl = new Pattern(lhs, rhs_info.first, assn_tok.GetTokenString(), lhs_is_syn, rhs_info.second);
  cl->first_synonym = QueryParser::GetSynonymInfo(assn_tok.GetTokenString(), &synonyms);
  if (lhs_is_syn) {
    cl->second_synonym = QueryParser::GetSynonymInfo(lhs, &synonyms);
  }
  clauses.emplace_back(cl);
}

/**
 * Parses the pattern clause.
 */
// pattern-cl : ‘pattern’ patternCond
// patternCond : pattern ( ‘and’ pattern )*
void QueryParser::ParsePattern() {
  Eat(TokenTag::kName); // eat 'pattern' keyword
  ParsePatternCond();
  // check for multiple relRefs (separated by 'and')
  while (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("and") == 0) {
    Eat(TokenTag::kName);
    ParsePatternCond();
  }
}

/**
 * Parses the select clause.
 */
void QueryParser::ParseSelect() {
  if (lookahead.GetTokenString().compare("Select") != 0) {
    throw PQLParseException("Expected \'Select\' keyword, instead got " + lookahead.GetTokenString());
  }
  Eat(TokenTag::kName); // 'Select' is tokenized as a name.
  ParseTarget();
  // while there are more tokens, we are expecting ( suchthat-cl | with-cl | pattern-cl )*
  while (lookahead.GetTokenTag() != TokenTag::kInvalid) {
    if (lookahead.GetTokenTag() == TokenTag::kSuchThat) {
      ParseSuchThat();
      continue;
    }
    if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("pattern") == 0) {
      ParsePattern();
      continue;
    }
    if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("with") == 0) {
      ParseWith();
      continue;
    }
    throw PQLParseException("Incorrect query. Expected at most 1 such that and 1 pattern for iteration 1.");
  }
}

/**
 * Parses the pql query.
 */
void QueryParser::ParseQuery() {
  // parse synonym declarations first, if they exist.
  if (lookahead.GetTokenString().compare("Select") != 0) {
    ParseDeclarations();
  }
  // parse select clause
  ParseSelect();
}

void QueryParser::Parse() {
  // lookahead is used for LL(1) predictive parsing.
  lookahead = tokenizer.GetNextToken();
  ParseQuery();
}
