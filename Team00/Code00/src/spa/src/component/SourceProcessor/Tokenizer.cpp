#include "Tokenizer.h"

/**
 * A static
 * @param statement_string A SIMPLE statement in its string form
 * @return A vector of Token objects that represent that SIMPLE statement
 */
std::vector<Token> Tokenizer::CreateTokens(const string& statement_string) {
  // todo: add tokenization logic here
  return {};
}
Token Tokenizer::CreateToken(const string& string_token) {
  return Token(nullptr, TokenTag::kInteger);
}
Tokenizer::Tokenizer() = default;
