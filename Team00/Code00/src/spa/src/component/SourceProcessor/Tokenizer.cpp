#include <iostream>
#include "Tokenizer.h"
#include <regex>
#include <string>
#include <vector>

/**
 * A static
 * @param statement_string A SIMPLE statement in its string form
 * @return A vector of Token objects that represent that SIMPLE statement
 */


std::string delmiter = " ";
using namespace std;

std::vector<Token> Tokenizer::CreateTokens(const string& statement_string) {
  vector<Token> tokens;

  auto start = 0U;
  auto end = statement_string.find(delmiter);
  while (end != string::npos) { // until the end of the string:
    string token_string = statement_string.substr(start, end - start);
    tokens.push_back(*CreateToken(token_string));
    start = end + delmiter.length();
    end = statement_string.find(delmiter, start);
  }
  string token_string = statement_string.substr(start, end);
  tokens.push_back(*CreateToken(token_string));
  return tokens;
}

Token* Tokenizer::CreateToken(string token_string) {
  return new Token(token_string, TokenTag::kInteger);
}
