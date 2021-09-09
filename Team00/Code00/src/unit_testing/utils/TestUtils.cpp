
#include "TestUtils.h"
#include <vector>

std::vector<std::string> TestUtils::GetBasicSourceLines() {
  std::vector<std::string> lines = {
      R"(procedure         Week4 {read x;)",
      R"(procedure Week4 {read x;)",
      R"(    print x; )",
      R"( y = 1;)",
      R"(z                                                                                                   = 3;)",
      R"(call Z;})",
      R"(if(x >= 1) {read x; call helloProc;})", // cond_expr encapsulated by brackets
      R"(if(x >= 1) {read x; call helloProc;} else { x = 1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1;
      //    R"(if(x >= 1) {read x; call helloProc;} else {x= 1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1;
      //    R"(if(x >= 1) {read x; call helloProc;} else {x =1; })", // cond_expr encapsulated by brackets todo: this fails to separate the =1
      //    R"(if(x >= 1) then {read x; call helloProc;} else {x=1;})", // todo: the {x=1;} is not supported by tokenizer (i.e. more than 2 special delimiters. if necessary, need to make the splitting functions do a recursive call)
  };
  return lines;
}
