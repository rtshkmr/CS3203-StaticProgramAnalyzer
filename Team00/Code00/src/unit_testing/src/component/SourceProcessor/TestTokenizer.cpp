#include "catch.hpp"

#include <iostream>
#include <component/SourceProcessor/Tokenizer.h>

using namespace std;
vector<string> program_lines = {
    R"(procedure Week4{ read x;)",
    R"(    print x; )",
    R"( y = 1;)",
    R"(z = 3;)",
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
