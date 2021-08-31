/**
 *  This file contains the implementation for syntax validation for a single token, based on the
 *  tag of the token. To prevent unnecessary bi-directional relationships, we shall not modify flags
 *  within PSubsystem from within this class.
 */
#include "ConcreteSyntaxValidator.h"

ConcreteSyntaxValidator::ConcreteSyntaxValidator() = default;

static bool ConcreteSyntaxValidator::validateToken(Token token) {
  return true;
}

static bool ConcreteSyntaxValidator::validateConcreteSyntax(vector<Token> tokens) {
  return true;
}
