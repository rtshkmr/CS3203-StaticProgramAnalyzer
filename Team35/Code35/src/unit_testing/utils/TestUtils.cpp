
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

/**
 * Checks if the contents of 2 lists are equal, which may not be in the same order.
 */
bool TestUtils::AreListsEqual(std::list<Statement*> as, std::list<Statement*> bs) {
  for (Statement* a: as) {
    bool found_a = false;
    for (Statement* b: bs) {
      found_a = found_a || a == b;
    }
    if (!found_a) {
      return false;
    }
  }

  return as.size() == bs.size();
}
