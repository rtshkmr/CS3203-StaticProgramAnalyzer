/**
 * This class contains any generic utility methods implementation.
 */

#include <exception/SpaException.h>
#include "Utility.h"

/**
 * Perfectly converts String to Integer using stoi without partial conversion given by stoi.
 * @param input The String to be converted.
 * @return The integer value after being converted.
 * @throws SyntaxException when a non-integer in passed in or when integers that had exceeded the range.
 */
int Utility::ConvertStringToInt(std::string input) {
  size_t num_chars = 0;
  int value = 0;
  try {
    value = stoi(input, & num_chars);
  } catch (std::exception ia) {
    throw SyntaxException("Argument is not smaller that max int.");
  }
  if (num_chars != input.size()) {
    throw SyntaxException("Constant is not valid. Numbers mixed with letters.");
  }
}