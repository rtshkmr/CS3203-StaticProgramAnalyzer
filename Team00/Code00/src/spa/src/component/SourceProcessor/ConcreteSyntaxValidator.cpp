/**
 *  This file contains the implementation for syntax validation for a single token, based on the
 *  tag of the token. To prevent unnecessary bi-directional relationships, we shall not modify flags
 *  within PSubsystem from within this class.
 */
#include "ConcreteSyntaxValidator.h"

ConcreteSyntaxValidator::ConcreteSyntaxValidator() = default;

bool ConcreteSyntaxValidator::ValidateToken(Token token) {
  return true;
}

bool ConcreteSyntaxValidator::ValidateConcreteSyntax(vector<Token> tokens) {
  return true;
}
