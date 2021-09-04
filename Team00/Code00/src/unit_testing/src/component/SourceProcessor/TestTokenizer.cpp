#include "catch.hpp"

#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>

using namespace std;
vector<string> program_lines = {
//    R"(procedure         Week4 {read x;)",
    R"(procedure Week4 {read x;)",
    R"(    print x; )",
    R"( y = 1;)",
    R"(z                                                                                                   = 3;)",
    R"(call Z;})"
};

TEST_CASE("Tokenizer display current tokenization status") {
  vector<Token> program_tokens = {};
  int line_counter = 0;
  cout << " currently testing:   \n";
  for (const auto& line: program_lines) {
    cout << line << "\n";
  }
  cout << "\n\n";
  for (const auto& line : program_lines) {
    cout << "=============[Line Number: " << ++line_counter << " ]===================\n";
    int token_counter = 0;
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    for (auto token : tokens) {
      cout << "[" << ++token_counter << "]token string:  " + token.GetTokenString() << "\n";
    }
    cout << "===================================================\n";
  }
}

TEST_CASE(
    "Tokenize Starting lines with variable white spacing between important delimiting characters should give the same number of tokens") {
  vector<string> starting_variations = {
      R"(procedure Week4{read x;)", // no extra white space
      R"(procedure Week4 { read x;)", // formatted whitespace
      R"(procedure Week4 {                                            read                                  x          ;                              )", // extreme white space
      R"(procedure Week4 {read x;)",
      R"(procedure Week4{ read x ;)",
  };

  vector<string> ending_variations = {
      R"(call Z;})",
      R"(call Z;})",
      R"(call                  Z                      ;          }         )",
      R"(call Z ;})",
      R"(call Z; })",
      R"(call Z ; })",
  };
  vector<vector<Token>> starting_outputs;
  for (auto& starting_line_variation : starting_variations) {
    vector<Token> output = Tokenizer::CreateTokens(starting_line_variation);
    starting_outputs.push_back(output);
  }
  vector<Token> starting_variation_reference = starting_outputs.at(0);
  for (int i = 1; i < starting_outputs.capacity() - 1; i++) {
    vector<Token> other = starting_outputs.at(i);
    REQUIRE(starting_variation_reference.capacity() == other.capacity());
    // fixme: fix equality operator after implementing tagging
//    for(int x = 0; x < starting_variation_reference.capacity();x++) {
//      Token reference_token = starting_variation_reference.at(x);
//      Token other_token = other.at(x);
//      REQUIRE(reference_token == other_token);
//    }
  }

  vector<vector<Token>> ending_outputs;
  for (auto& ending_line_variation:ending_variations) {
    vector<Token> output = Tokenizer::CreateTokens(ending_line_variation);
    ending_outputs.push_back(output);
  }
  vector<Token> ending_variation_reference = ending_outputs.at(0);
  for (int i = 1; i < ending_outputs.capacity() - 1; i++) {
    vector<Token> other = ending_outputs.at(i);
    REQUIRE(ending_variation_reference.capacity() == other.capacity());
  }
}
