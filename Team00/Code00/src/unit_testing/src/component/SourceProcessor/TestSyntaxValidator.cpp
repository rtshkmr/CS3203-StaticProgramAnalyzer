#include "catch.hpp"

#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>
#include <component/SourceProcessor/SyntaxValidator.h>
#include <regex>

using namespace std;

//vector<string> lines = {
//    R"(procedure         Week4 {read x;)",
//    R"(procedure Week4 {read x;)",
//    R"(    print x; )",
//    R"( y = 1;)",
//    R"(z                                                                                                   = 3;)",
//    R"(call Z;})",
//    R"(if(x >= 1) {read x; call helloProc;})", // cond_expr encapsulated by brackets
//    R"(if(x >= 1) {read x; call helloProc;} else { x = 1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1;
//    //    R"(if(x >= 1) {read x; call helloProc;} else {x= 1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1;
//    //    R"(if(x >= 1) {read x; call helloProc;} else {x =1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1
//    //    R"(if(x >= 1) then {read x; call helloProc;} else {x=1;})", // todo: the {x=1;} is not supported by tokenizer (i.e. more than 2 special delimiters. if necessary, need to make the splitting functions do a recursive call)
//};
//
//TEST_CASE("Display Test") {
//  string program_line = lines.at(0);
//  vector<Token> procedure_line_tokens = Tokenizer::CreateTokens(program_line);
//  bool successful_handling_procedure_semantics = SyntaxValidator::ValidateSemanticSyntax(procedure_line_tokens);
//  bool successful_semantic_validity_checks = successful_handling_procedure_semantics;
//  REQUIRE(successful_semantic_validity_checks);
//}

