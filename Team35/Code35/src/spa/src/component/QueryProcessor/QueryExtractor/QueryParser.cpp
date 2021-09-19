#include "QueryParser.h"
#include "QueryValidator.h"
#include <component/QueryProcessor/types/Exceptions.h>
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

/* Helper function to obtain synonym info (ie type) given it exists in list of synonyms. */
Synonym QueryParser::GetSynonymInfo(std::string syn_name, std::list<Synonym>* synonyms) {
  for (Synonym& t : *synonyms) {
    if (t.GetName().compare(syn_name) == 0) {
      return Synonym(syn_name, t.GetType());
    }
  }
  return Synonym("", DesignEntity::kInvalid);
};

std::vector<Token> QueryParser::GetSynonyms() {
  // TODO: here, add support for constant type, since TokenTag for that will be int.
  std::vector<Token> tokens;
  tokens.push_back(Eat(TokenTag::kName));
  while (lookahead.GetTokenTag() != TokenTag::kSemicolon) {
    Eat(TokenTag::kComma);
    tokens.push_back(Eat(TokenTag::kName));
  }
  return tokens;
}

void QueryParser::GetDeclaration() {
  std::string design_entity = lookahead.GetTokenString();
  // validate designEntity since alphabet is known.
  // TODO: Abstract this validation logic into a helper function.
  if (!regex_match(design_entity, RegexPatterns::GetDesignEntityPattern())) {
    throw PQLParseException("A synonym of unknown Design Entity type " + design_entity + " was declared.");
  }
  Eat(TokenTag::kName); // advance lookahead after parsing designEntity.
  DesignEntity de = GetDesignEntity(design_entity);
  std::vector<Token> s = GetSynonyms();
  // populate synonyms list with Synonym objects.
  for (Token& t : s) {
    if (synonyms_name_set.find(t.GetTokenString()) != synonyms_name_set.end()) {
      throw PQLValidationException("Duplicate synonym was declared.");
    }
    this->synonyms.emplace_back(t.GetTokenString(), de);
    this->synonyms_name_set.emplace(t.GetTokenString());
  }
}

void QueryParser::ParseDeclarations() {
  // a syntactically valid synonym list contains > 0 synonym declarations, each followed by a semicolon.
  GetDeclaration();
  Eat(TokenTag::kSemicolon);
  while (lookahead.GetTokenString().compare("Select") != 0 && lookahead.GetTokenTag() != TokenTag::kInvalid ) {
    GetDeclaration();
    Eat(TokenTag::kSemicolon);
  }
}

void QueryParser::GetTarget() {
   Token target_synonym = Eat(TokenTag::kName);
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
std::tuple<std::string, bool, bool> QueryParser::ParseStmtRef() {
  bool is_synonym = false;
  bool is_target_synonym = false;
  std::string curr_lookahead = lookahead.GetTokenString();

  if (lookahead.GetTokenTag() == TokenTag::kName) {
    // parse as synonym
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
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
  } else {
    throw PQLParseException("Incorrect stmtRef supplied in clause.");
  }
  Eat(lookahead.GetTokenTag());
  return std::make_tuple(curr_lookahead, is_synonym, is_target_synonym);
}
std::pair<Clause*, bool> QueryParser::ParseRelRef() {
  std::unordered_set<std::string> relRefs = {"Follows", "Parent", "Uses", "Modifies"};
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

  // TODO: add support in the future for modifiesP, usesP case: (entRef ‘,’ entRef)
  std::string lhs; bool is_lhs_syn; bool is_lhs_tgt_syn;
  // UsesS|ModifiesS: ‘Uses|Modifies’ ‘(’ stmtRef ‘,’ entRef ‘)’
  std::tie(lhs, is_lhs_syn, is_lhs_tgt_syn) = ParseStmtRef();

  if (lhs.compare("_") == 0 && (rel_type.compare("Modifies") == 0 || rel_type.compare("Uses") == 0)) {
    throw PQLValidationException("Semantically invalid to have _ as first argument for " + rel_type);
  }

  Eat(TokenTag::kComma);
  std::string rhs; bool is_rhs_syn; bool is_rhs_tgt_syn;
  if (rel_type.compare("Uses") == 0 || rel_type.compare("Modifies") == 0) {
    Token tok = ParseEntRef(false);
    rhs = tok.GetTokenString();
    bool _is_rhs_syn = false;
    bool _is_rhs_tgt_syn = false;
    for (Synonym& s : synonyms) {
      if (s.GetName() == rhs) {
        _is_rhs_syn = true;
        if (rhs == this->target.GetName()) {
          _is_rhs_tgt_syn = true;
        }
        break;
      }
    }
    is_rhs_syn = _is_rhs_syn;
    is_rhs_tgt_syn = _is_rhs_tgt_syn;
  } else {
    std::tie(rhs, is_rhs_syn, is_rhs_tgt_syn) = ParseStmtRef();
  }

  Eat(TokenTag::kCloseBracket);

  // temporary
  if (rel_type.compare("Uses") == 0 || rel_type.compare("Modifies") == 0) {
    rel_type = std::string(rel_type + "S");
  }

  // semantic validation of relRef.
  if (!QueryValidator::Is_Semantically_Valid_RelRef(lhs, rhs, GetRelRef(rel_type),
                                                   is_lhs_syn, is_rhs_syn, &synonyms)) {
    throw PQLValidationException("Received semantically invalid " + rel_type + " cl.");
  }
  // TODO: this could introduce a memory leak...
  Clause* cl = new SuchThat(lhs, rhs, GetRelRef(rel_type), is_lhs_syn, is_rhs_syn);
  return std::make_pair(cl, is_lhs_tgt_syn || is_rhs_tgt_syn);
}

void QueryParser::ParseSuchThat() {
  // TODO: add support for multiple relRefs (separated by and)
  Eat(TokenTag::kSuchThat);
  std::pair<Clause*, bool> clause_info = ParseRelRef();
  // add group.
  clauses.emplace_back(clause_info.first);
}

// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’
/* Takes in a boolean 1 if parsing of entRef is for pattern, 0 otherwise */
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

// iteration 1: factor: var_name | const_value
std::string QueryParser::ParseFactor() {
  std::stringstream ss;
  if (lookahead.GetTokenTag() == TokenTag::kInteger) {
    // parse as constant
    ss << Eat(TokenTag::kInteger).GetTokenString();
  } else if (lookahead.GetTokenTag() == TokenTag::kName){
    // parse as var_name
    ss << Eat(TokenTag::kName).GetTokenString();
  } else {
    throw PQLParseException("Received invalid factor in rhs of pattern cl.");
  }
  return ss.str();
}

// iteration 1: expression-spec: ‘_’ ‘"’ factor ‘"’ ‘_’ | ‘_’
std::pair<std::string, bool> QueryParser::ParseExpressionSpec() {
//  std::cout << "parsing expressionSpec..." << std::endl;
  bool is_exact_match = false;
  std::stringstream rhs_ss;

  // for iteration 1, there are no exact matches that are to be expected.
  if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    throw PQLParseException("Invalid expression-spec for rhs of pattern clause in iteration 1.");
  }
  Eat(TokenTag::kUnderscore);
  rhs_ss << "_";
  if (lookahead.GetTokenTag() == TokenTag::kCloseBracket) {
    // consider rhs as '_' case due to end of expression-spec.
  } else if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    // consider as ‘_’ ‘"’ factor ‘"’ ‘_’
    rhs_ss.str("");
    Eat(TokenTag::kStringQuote);
    rhs_ss << ParseFactor();
    Eat(TokenTag::kStringQuote);
    Eat(TokenTag::kUnderscore);
  } else {
    throw PQLParseException("Invalid expression-spec for rhs of pattern clause in iteration 1.");
  }

  return std::make_pair(rhs_ss.str(), is_exact_match);
}

bool QueryParser::IsValidSynonym(Token token) {
  if (token.GetTokenTag() != TokenTag::kName) {
    return 0;
  }
  std::string syn_name = token.GetTokenString();
  // syn_name must be a known synonym.
  bool is_valid = false;
  for (Synonym& s : synonyms) {
    if (s.GetName() == syn_name) {
      is_valid = true;
      break;
    }
  }
  if (!is_valid) {
    return 0;
  }
  return 1;
}
// returns 1 if syn for pattern clause is valid name and type, 0 otherwise.
bool QueryParser::IsValidSynonym(Token token, DesignEntity de) {
  if (token.GetTokenTag() != TokenTag::kName) {
    return 0;
  }
  std::string syn_name = token.GetTokenString();
  // syn_name must be a known synonym, and of permitted type.
  bool is_valid = false;
  for (Synonym& s : synonyms) {
    if (s.GetName() == syn_name && s.GetType() == de) {
      is_valid = true;
      break;
    }
  }
  if (!is_valid) {
    return 0;
  }
  return 1;
}

void QueryParser::ParsePattern() {
  Eat(TokenTag::kName); // eat 'pattern' keyword
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

  // TODO: this could introduce a memory leak...
  Clause* cl = new Pattern(lhs, rhs_info.first, assn_tok.GetTokenString(), lhs_is_syn, rhs_info.second);
  clauses.emplace_back(cl);
}

void QueryParser::ParseSelect() {
  if (lookahead.GetTokenString().compare("Select") != 0) {
    throw PQLParseException("Expected \'Select\' keyword, instead got " + lookahead.GetTokenString());
  }
  Eat(TokenTag::kName); // 'Select' is tokenized as a name.
  GetTarget();
  // if there are more tokens, we are expecting either such that or pattern clauses.
  if (lookahead.GetTokenTag() == TokenTag::kInvalid) {
    return;
  }
  if (lookahead.GetTokenTag() == TokenTag::kSuchThat) {
    ParseSuchThat();
  }
  if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("pattern") == 0) {
    ParsePattern();
  }
  if (lookahead.GetTokenTag() != TokenTag::kInvalid) {
    throw PQLParseException("Incorrect query. Expected at most 1 such that and 1 pattern for iteration 1.");
  }
}

void QueryParser::ParseQuery() {
  // parse synonym declarations first, if they exist.
  if (lookahead.GetTokenString().compare("Select") != 0) {
    ParseDeclarations();
  }
  // TODO: more validation for synonyms (ie. no duplicate synonym)
  // parse select clause
  ParseSelect();
}

void QueryParser::GroupClauses() {
  // TODO: implement grouping algorithm.
  // current implementation assumes only 1 such that and 1 pattern clause; manually grouping by common synonym.
  if (clauses.size() == 0) {
    return;
  }

  if (clauses.size() == 1) {
    // expecting 1 such that or 1 pattern clause
    bool has_target_syn = false;
    // such that clause
    if (typeid(*clauses[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>(clauses[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    } else {
      // pattern clause
      Pattern* pt = dynamic_cast<Pattern*>(clauses[0]);
      // has_target_syn if lhs is a target synonym, or if  syn-assn is a target synonym
      if (pt->assign_synonym.compare(target.GetName()) == 0 ||
          pt->left_is_synonym && pt->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    }
    // add clause into its own group
    std::vector<Clause*> cl1;
    cl1.push_back(clauses[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups.push_back(g1);

  } else if (clauses.size() == 2) {
    // expecting 1 such that followed by 1 pattern clause
    bool has_target_syn = false;
    if (typeid(*clauses[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>(clauses[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    }
    std::vector<Clause*> cl1;
    cl1.push_back(clauses[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups.push_back(g1);

    // if 1x pattern cl has common synonym, it goes in the same group.
    Pattern* pt = dynamic_cast<Pattern*>(clauses[1]);
    bool has_common_assign_syn = pt->assign_synonym.compare(cl1[0]->left_hand_side) == 0;
    bool has_common_args = pt->left_is_synonym && (
            pt->left_hand_side.compare(cl1[0]->left_hand_side) == 0 ||
            pt->left_hand_side.compare(cl1[0]->right_hand_side) == 0);
    if (has_common_assign_syn || has_common_args) {
      g1->AddClauseToVector(pt);
    } else {
      std::vector<Clause*> cl2;
      cl2.push_back(pt);
      bool pattern_cl_has_tgt_syn = false;
      if (pt->assign_synonym.compare(target.GetName()) == 0 ||
      pt->left_is_synonym && pt->left_hand_side.compare(target.GetName()) == 0) {
        pattern_cl_has_tgt_syn = true;
      }
      Group* g2 = new Group(cl2, pattern_cl_has_tgt_syn);
      groups.push_back(g2);
    }
  } else {
    throw PQLValidationException("Got more than 2 clauses.");
  }
}

void QueryParser::Parse() {
  // lookahead is used for LL(1) predictive parsing.
  lookahead = tokenizer.GetNextToken();
  ParseQuery();
  GroupClauses();
}
