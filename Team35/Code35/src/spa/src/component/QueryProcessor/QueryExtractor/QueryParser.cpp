#include "QueryParser.h"
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Exceptions.h>
#include <datatype/RegexPatterns.h>
#include <unordered_set>
#include <sstream>

/* Checks that current lookahead has the same expected type. If valid, advances tokenizer to next lookahead.*/
Token QueryParser::eat(TokenTag token_type) {
  Token token = lookahead;
//  std::cout << "Tokenizer is eating: " + token.GetTokenString() << std::endl;
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
  std::vector<Token> tokens;
  tokens.push_back(eat(TokenTag::kName));
  while (lookahead.GetTokenTag() != TokenTag::kSemicolon) {
    eat(TokenTag::kComma);
    tokens.push_back(eat(TokenTag::kName));
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
//   std::cout << "Exiting from Getarget()..." << std::endl;
   if (!hasSynonym) {
     throw PQLParseException("Incorrect target synonym for \'Select\' query.");
   }
}

// stmtRef: synonym | ‘_’ | INTEGER
// returns a tuple of values corresponding to <stmtref string, isSynonym, isTargetSyononym>
std::tuple<std::string, bool, bool> QueryParser::parse_stmtRef() {
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
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
  } else {
    throw PQLParseException("Incorrect stmtRef supplied in clause.");
  }
  eat(lookahead.GetTokenTag());
  return std::make_tuple(curr_lookahead, is_synonym, is_target_synonym);
}
std::pair<Clause*, bool> QueryParser::parse_relRef() {
//  std::cout << "parsing relref" << std::endl;
  std::unordered_set<std::string> relRefs = {"Follows", "Follows*", "Parent", "Parent*", "Uses", "Modifies"};
  std::unordered_set<std::string>::const_iterator got = relRefs.find(lookahead.GetTokenString());
  if (got == relRefs.end()) {
    throw PQLParseException("Invalid relRef in such that clause.");
  }
  std::string rel_type = lookahead.GetTokenString();
  eat(TokenTag::kName);
  eat(TokenTag::kOpenBracket);
//  std::cout << "parsing relref of type " + rel_type << std::endl;

  // TODO: add support in the future for modifiesP, usesP case: (entRef ‘,’ entRef)
  std::string lhs; bool is_lhs_syn; bool is_lhs_tgt_syn;
  // UsesS|ModifiesS: ‘Uses|Modifies’ ‘(’ stmtRef ‘,’ entRef ‘)’
  std::tie(lhs, is_lhs_syn, is_lhs_tgt_syn) = parse_stmtRef();

  if (lhs.compare("_") == 0 && (rel_type.compare("Modifies") == 0 || rel_type.compare("Uses") == 0)) {
    throw PQLValidationException("Semantically invalid to have _ as first argument for " + rel_type);
  }

  eat(TokenTag::kComma);
  std::string rhs; bool is_rhs_syn; bool is_rhs_tgt_syn;
  if (rel_type.compare("Uses") == 0 || rel_type.compare("Modifies") == 0) {
//    std::cout << "encountered uses or modifies relRef" << std::endl;
    Token tok = parse_entRef(false);
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
    std::tie(rhs, is_rhs_syn, is_rhs_tgt_syn) = parse_stmtRef();
  }

  eat(TokenTag::kCloseBracket);

  // temporary
  if (rel_type.compare("Uses") == 0 || rel_type.compare("Modifies") == 0) {
    rel_type = std::string(rel_type + "S");
  }

  // TODO: this could introduce a memory leak...
  Clause* cl = new SuchThat(lhs, rhs, GetRelRef(rel_type), is_lhs_syn, is_rhs_syn);
  return std::make_pair(cl, is_lhs_tgt_syn || is_rhs_tgt_syn);
}

void QueryParser::parse_such_that() {
  // TODO: add support for multiple relRefs (separated by and)
  eat(TokenTag::kSuchThat);
  std::pair<Clause*, bool> clause_info = parse_relRef();
  // add group.
  clauses.emplace_back(clause_info.first);
}

// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’
/* Takes in a boolean 1 if parsing of entRef is for pattern, 0 otherwise */
Token QueryParser::parse_entRef(bool isPatternCl) {
//  std::cout << "parsing entRef..." << std::endl;
  std::string token_name;
  TokenTag token_type;
  if (lookahead.GetTokenTag() == TokenTag::kName) {
    // parse as known synonym. Synonym must be variable for pattern cl.
    if (isPatternCl && !is_valid_synonym(lookahead, DesignEntity::kVariable)) {
      throw PQLParseException("Unknown synonym received as entRef in lhs of pattern cl.");
    }
    if (!isPatternCl && !is_valid_synonym(lookahead)) {
      throw PQLParseException("Unknown synonym received as entRef in lhs of cl.");
    }
    token_name = eat(TokenTag::kName).GetTokenString();
    token_type = TokenTag::kName;
  } else if (lookahead.GetTokenTag() == TokenTag::kUnderscore) {
    // parse as underscore
    eat(TokenTag::kUnderscore);
    token_name = "_";
    token_type = TokenTag::kUnderscore;
  } else {
    // parse as "’ IDENT ‘"
    eat(TokenTag::kStringQuote);
    token_name = eat(TokenTag::kName).GetTokenString();
    token_type = TokenTag::kName;
    eat(TokenTag::kStringQuote);
  }
  return Token(token_name, token_type);
}

// iteration 1: factor: var_name | const_value
std::string QueryParser::parse_factor() {
//  std::cout << "parsing factor..." << std::endl;
  std::stringstream ss;
  if (lookahead.GetTokenTag() == TokenTag::kInteger) {
    // parse as constant
    ss << eat(TokenTag::kInteger).GetTokenString();
  } else if (lookahead.GetTokenTag() == TokenTag::kName){
    // parse as var_name
    ss << eat(TokenTag::kName).GetTokenString();
  } else {
    throw PQLParseException("Received invalid factor in rhs of pattern cl.");
  }
  return ss.str();
}

// iteration 1: expression-spec: ‘_’ ‘"’ factor ‘"’ ‘_’ | ‘_’
std::pair<std::string, bool> QueryParser::parse_expressionSpec() {
//  std::cout << "parsing expressionSpec..." << std::endl;
  bool is_exact_match = false;
  std::stringstream rhs_ss;

  // for iteration 1, there are no exact matches that are to be expected.
  if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    throw PQLParseException("Invalid expression-spec for rhs of pattern clause in iteration 1.");
  }
  eat(TokenTag::kUnderscore);
  rhs_ss << "_";
  if (lookahead.GetTokenTag() == TokenTag::kCloseBracket) {
    // consider rhs as '_' case due to end of expression-spec.
  } else if (lookahead.GetTokenTag() == TokenTag::kStringQuote) {
    // consider as ‘_’ ‘"’ factor ‘"’ ‘_’
    rhs_ss.str("");
    eat(TokenTag::kStringQuote);
    rhs_ss << parse_factor();
    eat(TokenTag::kStringQuote);
    eat(TokenTag::kUnderscore);
  } else {
    throw PQLParseException("Invalid expression-spec for rhs of pattern clause in iteration 1.");
  }

  return std::make_pair(rhs_ss.str(), is_exact_match);
}

bool QueryParser::is_valid_synonym(Token token) {
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
bool QueryParser::is_valid_synonym(Token token, DesignEntity de) {
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

void QueryParser::parse_pattern() {
//  std::cout << "parsing pattern clause..." << std::endl;
  eat(TokenTag::kName); // eat 'pattern' keyword
  // check if syn-assign valid
  if (!is_valid_synonym(lookahead, DesignEntity::kAssign)) {
    throw PQLParseException("Expected valid syn-assign for pattern cl, instead got " + lookahead.GetTokenString());
  }
//  std::cout << "Got valid syn assign: " + lookahead.GetTokenString() << std::endl;
  Token assn_tok = Token(lookahead.GetTokenString(), lookahead.GetTokenTag());

  eat(TokenTag::kName); // eat 'syn-assign'
  eat(TokenTag::kOpenBracket);
  // parse_lhs
  Token lhs_token = parse_entRef(true);
  eat(TokenTag::kComma);
  std::string lhs = lhs_token.GetTokenString();
  bool lhs_is_syn = is_valid_synonym(lhs_token, DesignEntity::kVariable);
//  std::cout << "lhs_is_syn has value "; std::cout << lhs_is_syn << std::endl;
  //parse_rhs
  std::pair<std::string, bool> rhs_info = parse_expressionSpec();
//  std::cout << "grouping clauses...";

  // TODO: this could introduce a memory leak...
  Clause* cl = new Pattern(lhs, rhs_info.first, assn_tok.GetTokenString(), lhs_is_syn, rhs_info.second);
  clauses.emplace_back(cl);
}

void QueryParser::parse_select() {
  if (lookahead.GetTokenString().compare("Select") != 0) {
    throw PQLParseException("Expected \'Select\' keyword, instead got " + lookahead.GetTokenString());
  }
  eat(TokenTag::kName); // 'Select' is tokenized as a name.
//  std::cout << "Entering Getarget()..." << std::endl;
  GetTarget();
//  std::cout << "Exited from Getarget()..." << std::endl;
  // if there are more tokens, we are expecting either such that or pattern clauses.
  if (lookahead.GetTokenTag() == TokenTag::kInvalid) {
    return;
  }
//  std::cout << "Expecting such that or pattern clause..." << std::endl;
  if (lookahead.GetTokenTag() == TokenTag::kSuchThat) {
//    std::cout << "parsing such that" << std::endl;
    parse_such_that();
  } else if (lookahead.GetTokenTag() == TokenTag::kName && lookahead.GetTokenString().compare("pattern") == 0) {
//    std::cout << "parsing pattern" << std::endl;
    parse_pattern();
  } else {
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

void QueryParser::group_clauses() {
  // TODO: implement grouping algorithm.
  // current implementation assumes only 1 such that and 1 pattern clause; manually grouping by common synonym.
//  std::cout << "There are ";
  std::cout << clauses.size();

  if (clauses.size() == 0) {
    return;
  }
//  std::cout << " clauses. Applying grouping algorithm." << std::endl;

  if (clauses.size() == 1) {
    // expecting 1 such that or 1 pattern clause
    bool has_target_syn = false;
    // such that clause
//    std::cout << "printing typeids..." << std::endl;
    //std::cout << typeid(*clauses[0]).;
    //std::cout << typeid(SuchThat);

    if (typeid(*clauses[0]) == typeid(SuchThat)) {
//      std::cout << "received such that clause for grouping" << std::endl;
      SuchThat* st = dynamic_cast<SuchThat*>(clauses[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
//      std::cout << "printing value of has_target_syn for 1x such that... ";
      std::cout << has_target_syn << std::endl;
    } else {
      // pattern clause
//      std::cout << "received pattern clause for grouping" << std::endl;
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
    Group g1 = Group(cl1, has_target_syn);
    groups.push_back(g1);

  } else if (clauses.size() == 2) {
    // expecting 1 such that followed by 1 pattern clause
    bool has_target_syn = false;
    if (typeid(clauses[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>(clauses[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    }
    std::vector<Clause*> cl1;
    cl1.push_back(clauses[0]);
    Group g1 = Group(cl1, has_target_syn);
    groups.push_back(g1);

    // if 1x pattern cl has common synonym, it goes in the same group.
    Pattern* pt = dynamic_cast<Pattern*>(clauses[1]);
    if (pt->assign_synonym.compare(cl1[0]->left_hand_side) == 0 ||
        pt->left_is_synonym && (pt->left_hand_side.compare(cl1[0]->left_hand_side) == 0)) {
      g1.AddClauseToVector(pt);
    } else {
      std::vector<Clause*> cl2;
      cl2.push_back(pt);
      groups.push_back(Group(cl2, false));
    }
  } else {
//    std::cout << clauses.size() << std::endl;
    throw PQLValidationException("Got more than 2 clauses.");
  }
}

void QueryParser::parse() {
  // lookahead is used for LL(1) predictive parsing.
  lookahead = tokenizer.GetNextToken();
  parse_query();
  group_clauses();
//  std::cout << "printing group stats..." << std::endl;
//  std::cout << groups.size() << std::endl;
}
