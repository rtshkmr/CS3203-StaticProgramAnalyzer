#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Parser.h"
#include "TNode.h"
#include "PSubsystem.h"

using namespace std;
using namespace par;

void Parser::Parse(std::string file_name) {
  psub::PSubsystem p_subsystem;
  p_subsystem.InitDataStructures();

  // check if file exists
  ifstream source_file(file_name);
  if (source_file.is_open()) {
    vector<char> chara;
    char byte;
    // read char by char, delimiter = { ; }

    while (source_file.get(byte)) {
      if (byte == '\n') {
        continue;
      }

      if (byte == ' ') {
        if (chara.empty() || !chara.empty() && chara.back() == ' ') {
          continue;
        }
      }

      chara.push_back(byte);

      if (byte == '{' || byte == ';' || byte == '}') {
        std::string s(chara.begin(), chara.end());
        p_subsystem.ProcessStatement(s);
        chara.clear();
      }
    }
    deliverable_ = *p_subsystem.GetDeliverables();
    source_file.close();
  }

}

Deliverable* Parser::GetDeliverables() {
  return &this->deliverable_;
}
