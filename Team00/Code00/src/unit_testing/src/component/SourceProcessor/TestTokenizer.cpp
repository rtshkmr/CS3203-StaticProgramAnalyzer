#include "catch.hpp"
#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>
#include <datatype/RegexPatterns.h>
#include <regex>

using namespace std;

vector<string> lines = {
//    R"(procedure         Week4 {read x;)",
//    R"(procedure Week4 {read x;)",
//    R"(    print x; )",
//    R"( y = 1;)",
//    R"(z                                                                                                   = 3;)",
//    R"(call Z;})",
//    R"(if(x >= 1) {read x; call helloProc;})", // cond_expr encapsulated by brackets
//    R"(if(x >= 1))", // cond_expr encapsulated by brackets
//    R"(if(x >= 1) {read x; call helloProc;} else { x = 1; })", // cond_expr encapsulated by brackets
//    R"(x=1;)", // cond_expr encapsulated by brackets : this fails to separate the =1;
//    R"(if(x >= 1) {read x; call helloProc;} else {x= 1; })",
//    R"(if(x >= 1) {read x; call helloProc;} else {x =1; })", // cond_expr encapsulated by brackets
    R"(if((x>=1)&&((y>69)||(z<0)) && (!!TheMatrixIsReal)){)", // nested cond expr without any spacing b/w any impt characters
//    R"(if(x >= 1) then {read x; call helloProc;} else {x=1;})", // todo: the {x=1;} is not supported by tokenizer (i.e. more than 2 special delimiters. if necessary, need to make the splitting functions do a recursive call)
};

TEST_CASE("1.Tokenizer.Tokenizer displays current tokenization status") {
  vector<Token> program_tokens = {};
  int line_counter = 0;
  cout << " currently testing:   \n";
//  vector<string> valid_program_lines =
  for (const auto& line: lines) {
    cout << line << "\n";
  }
  cout << "\n\n";
  for (const auto& line : lines) {
    cout << "=============[Line Number: " << ++line_counter << " ]===================\n";
    int token_counter = 0;
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    program_tokens.insert(program_tokens.end(), tokens.begin(), tokens.end());
    for (auto token : tokens) {
      cout << "[" << ++token_counter << "]token string:  " + token.GetTokenString() << "\n";
    }
    cout << "===================================================\n";
  }
}

/*
TEST_CASE(
    "1.Tokenizer.Tokenize Starting valid_program_lines with variable white spacing between important delimiting characters should give the same number of tokens") {
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
*/

TEST_CASE("1.Tokenizer.RegexPatterns pattern tests for token_strings") {

  regex fixed_keyword_pat = RegexPatterns::GetFixedKeywordPattern();
  regex fixed_char_pat = RegexPatterns::GetFixedCharPattern(); // for braces, semicolon...
  regex binary_arithmetic_operator_pat = RegexPatterns::GetBinaryArithmeticOperatorPattern(); // for math
  regex binary_comparison_operator_pat = RegexPatterns::GetBinaryComparisonPattern(); // for comparator chars
  regex name_pat = RegexPatterns::GetNamePattern(); // names, integers... todo: check alphanum
  regex integer_pat = RegexPatterns::GetIntegerPattern();

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

  SECTION("Tokenizer successfully handles variable names and integers") {
    REQUIRE(handle_valid_names_integers);
    REQUIRE(handle_invalid_names_integers);
  }
  bool handle_valid_comparators = regex_match(">", binary_comparison_operator_pat)
      && regex_match("<=", binary_comparison_operator_pat)
      && regex_match(">=", binary_comparison_operator_pat)
      && regex_match("==", binary_comparison_operator_pat)
      && !regex_match("=", binary_comparison_operator_pat); // not a comparator

  bool handle_valid_arithmetic_operators = regex_match("+", binary_arithmetic_operator_pat)
      && regex_match("/", binary_arithmetic_operator_pat)
      && regex_match("%", binary_arithmetic_operator_pat)
      && !regex_match("+=", binary_arithmetic_operator_pat) // not a comparator
      && !regex_match("+++", binary_arithmetic_operator_pat); // not a comparator

  SECTION("Tokenizer successfully handles valid comparators") {
    REQUIRE(handle_valid_comparators);
  }

  SECTION("Tokenizer successfully handles valid arithmetic operators") {
    REQUIRE(handle_valid_arithmetic_operators);
  }
}

TEST_CASE("1.Tokenizer.Successfully handles the tagging strings with a TokenTag") {
  Token keyword_proc = Token("procedure", TokenTag::kProcedureKeyword);
  Token keyword_call = Token("call", TokenTag::kCallKeyword);
  Token plus_token = Token("+", TokenTag::kBinaryArithmeticOperator);
  Token open_brace_token = Token("{", TokenTag::kOpenBrace);
  Token open_bracket_token = Token("(", TokenTag::kOpenBracket);
  Token close_brace_token = Token("}", TokenTag::kCloseBrace);
  Token close_bracket_token = Token(")", TokenTag::kCloseBracket);
  Token then_token = Token("then", TokenTag::kThenKeyword);
  Token else_token = Token("else", TokenTag::kElseKeyword);

  string invalid_token_strings[] = {"1Hello", "}}", ">>"};
  vector<Token> invalid_tokens;
  for (const auto& token_string:invalid_token_strings) {
    invalid_tokens.emplace_back(token_string, Token::TagStringWithToken(token_string));
  }

  bool successful_handling_valid_inputs = Token("procedure", Token::TagStringWithToken("procedure")) == keyword_proc
      && Token("call", Token::TagStringWithToken("call")) == keyword_call
      && Token("+", Token::TagStringWithToken("+")) == plus_token
      && Token("{", Token::TagStringWithToken("{")) == open_brace_token
      && Token("}", Token::TagStringWithToken("}")) == close_brace_token
      && Token("(", Token::TagStringWithToken("(")) == open_bracket_token
      && Token(")", Token::TagStringWithToken(")")) == close_bracket_token
      && Token("then", Token::TagStringWithToken("then")) == then_token;

  bool successful_handling_invalid_inputs = true;
  for (auto& token : invalid_tokens) {
    successful_handling_invalid_inputs = (token.GetTokenTag() == TokenTag::kInvalid);
  }

  SECTION("Tagging of token strings successfully handles valid inputs") {
    REQUIRE(successful_handling_valid_inputs);
  }
  SECTION("Tagging of token strings successfully handles invalid inputs") {
    REQUIRE(successful_handling_invalid_inputs);
  }

}
