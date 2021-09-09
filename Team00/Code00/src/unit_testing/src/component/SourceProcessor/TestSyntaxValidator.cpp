#include "catch.hpp"

#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>
#include <component/SourceProcessor/SyntaxValidator.h>
#include <regex>

using std::string;
using std::vector;

static vector<string> valid_program_lines = {
    // procedure:
    R"(procedure         Week4{)", // idx 0
    // macro functions:
    R"(read x;)",
    R"(    print x; )",
    R"(    call hackTheG; )",

    // if line:
    R"(   if (x >= 1) then { )", // idx: 4
    R"(   if (1 >= x) then { )",
    R"(   if (1 >= 1) then { )", // valid



    //else statement positive:
    R"(   else {)", // idx: 7
    R"(   else{)", //


    // while statement:
    R"(  while (x >= 1) then { )", // idx: 9

    R"(})",

    // assignment:
    R"( y = 1;)",
    R"(z                                                                                                   = 3;)",
};

static vector<string> invalid_program_lines = {
    // procedure:
    R"(procedure       {)", // no procname
    R"(procedure       )",
    // macro functions:
    R"(read ;)", // idx 2
    R"(read x)",
    R"(    print  )",
    R"(    print x )",
    R"(    print ; )",
    R"(    call hackTheG )",
    R"(    call ; )",

    // if line:
    R"(   if x >= 1) then { )", // idx: 9
    R"(   if x >= 1 then { )",
    R"(   if (x = 1) then { )",
    R"(   if (x >= 1) then  )",
    R"(   if (x >= 1) { )",
    R"(   if then { )",
    R"(   if (x) then { )",
    R"(   if () then { )",

    // else line:
    R"(   else )", //idx: 17

    // while statement:
    R"(   while x >= 1) then { )", // idx: 18
    R"(   while x >= 1 then { )",
    R"(   while (x = 1) then { )",
    R"(   while (x >= 1) then  )",
    R"(   while (x >= 1) { )",
    R"(   while then { )",
    R"(   while (x) then { )",
    R"(   while () then { )",

//    // assignment:
//    R"( y = 1;)",
//    R"(z                                                                                                   = 3;)",
};

static void CheckAgainstSampleLines(int start, int end, vector<string>& lines, bool expected) {
  for (int i = start; i <= end; i++) {
    string line = lines.at(i);
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    bool output = SyntaxValidator::ValidateSemanticSyntax(tokens);
    bool matches_with_expected = output == expected;
    REQUIRE(matches_with_expected);
  }
}

TEST_CASE("1.SyntaxValidator.Validator handles basic statements:") {
  SECTION("handles procedure statement") {
    SECTION("positive cases") {
      CheckAgainstSampleLines(0, 0, valid_program_lines, true);
    }
    SECTION("negative cases") {
      CheckAgainstSampleLines(0, 1, invalid_program_lines, false);
    }
  }

  SECTION("handles macro function calls like read, print and call") {
    SECTION("positive cases") {
      CheckAgainstSampleLines(1, 3, valid_program_lines, true);
    }
    SECTION("negative cases") {
      CheckAgainstSampleLines(2, 8, invalid_program_lines, false);
    }
  }
  SECTION("handles \"if\" statements") {
    SECTION("positive cases") {
      CheckAgainstSampleLines(4, 6, valid_program_lines, true);
    }
    SECTION("negative cases") {
      CheckAgainstSampleLines(9, 16, invalid_program_lines, false);
    }
  }


  SECTION("handles \"else\" statements") {
    SECTION("positive cases") {
      CheckAgainstSampleLines(7, 8, valid_program_lines, true);
    }
    SECTION("negative cases") {
      CheckAgainstSampleLines(17, 17, invalid_program_lines, false);
    }
  }


  SECTION("handles while statements") {
    SECTION("positive cases") {
      CheckAgainstSampleLines(9, 9, valid_program_lines, true);
    }
    SECTION("negative cases") {
      CheckAgainstSampleLines(18, 25, invalid_program_lines, false);
    }
  }


}

