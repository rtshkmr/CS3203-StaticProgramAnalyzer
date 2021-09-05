#include "catch.hpp"

#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>
#include <regex>

using namespace std;
vector<string> program_lines = {
//    R"(procedure         Week4 {read x;)",
    R"(procedure Week4 {read x;)",
    R"(    print x; )",
    R"( y = 1;)",
    R"(z                                                                                                   = 3;)",
    R"(call Z;})",
    R"(if(x >= 1) {read x; call helloProc;})",
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

TEST_CASE("Regex pattern tests for token_strings") {
  regex fixed_keyword_pat(R"(procedure|read|print|call|while|if|)");
  regex fixed_char_pat(R"(\{|\}|;)"); // for braces, semicolon...
  regex binary_arithmetic_operator_pat(R"(\+|\-|\*|\/|%|=|==|>|>=|<|<=|!=)"); // for math
  regex binary_comparison_operator_pat(R"(==|>|>=|<|<=|!=)"); // for comparator chars
  regex name_pat(R"(^[[:alpha:]]+([0-9]+|[[:alpha:]]+)*)"); // names, integers... todo: check alphanum
  regex integer_pat(R"([0-9]+)");

  bool handle_valid_names_integers = regex_match("procedure", fixed_keyword_pat)
      && regex_match("if", fixed_keyword_pat)
      && regex_match("read", fixed_keyword_pat)
      && regex_match("call", fixed_keyword_pat)
      && (regex_match("1", integer_pat)) // single digit
      && (regex_match("1124124", integer_pat)) // multiple digit numbers
      && (regex_match("h1", name_pat)) // letter + digit
      && (regex_match("h1h1h1h1h1h1", name_pat)) // letter + digit
      && (regex_match("h1continue", name_pat)); // letter + digit + letter

  bool handle_invalid_names_integers = !regex_match("Proc", fixed_keyword_pat)
      && !regex_match("proceDure", fixed_keyword_pat)
      && !(regex_match("h", integer_pat)) // single letter
      && !(regex_match("hello", integer_pat)) // a word as a name
      && !(regex_match("HeLlo", integer_pat)) // a varying upper and lower cases
      && !(regex_match("HELLO", integer_pat)) // upper case
      && !(regex_match("1hello", name_pat)); // illegal starting char

  bool handle_valid_comparators = regex_match(">", binary_comparison_operator_pat)
      && regex_match("<=", binary_comparison_operator_pat)
      && regex_match("==", binary_comparison_operator_pat)
      && !regex_match("=", binary_comparison_operator_pat); // not a comparator
  bool handle_valid_arithmetic_operators = regex_match("+", binary_arithmetic_operator_pat)
      && regex_match("/", binary_arithmetic_operator_pat)
      && regex_match("%", binary_arithmetic_operator_pat)
      && !regex_match("+=", binary_arithmetic_operator_pat) // not a comparator
      && !regex_match("+++", binary_arithmetic_operator_pat); // not a comparator

  bool regex_checking_success = (handle_valid_names_integers
      && handle_invalid_names_integers
      && handle_valid_comparators
      && handle_valid_arithmetic_operators);

  REQUIRE(regex_checking_success);
}

TEST_CASE("Test successful tagging of strings") {
  Token keyword_proc = Token("procedure", TokenTag::kKeyword);
  Token keyword_call = Token("call", TokenTag::kKeyword);
  Token plus_token = Token("+", TokenTag::kBinaryArithmeticOperator);
  Token open_brace_token = Token("{", TokenTag::kOpenBrace);
  Token close_brace_token = Token("}", TokenTag::kCloseBrace);

  bool successful_tagging = Token("procedure", Token::TagStringWithToken("procedure")) == keyword_proc
      && Token("call", Token::TagStringWithToken("call")) == keyword_call
      && Token("+", Token::TagStringWithToken("+")) == plus_token
      && Token("{", Token::TagStringWithToken("{")) == open_brace_token
      && Token("}", Token::TagStringWithToken("}")) == close_brace_token
      && Token("call", Token::TagStringWithToken("call")) == keyword_call;
  REQUIRE(successful_tagging);
}
