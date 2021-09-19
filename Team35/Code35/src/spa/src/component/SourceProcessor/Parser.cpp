#include <string>
#include <vector>
#include <util/Logger.h>

#include "Parser.h"
#include "TNode.h"
#include "PSubsystem.h"

using namespace std;
using namespace par;
constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

void Parser::Parse(const std::string& file_name) {
  L("[ENTER] PARSE parser");
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

      if (byte == '\t' || byte == '\a' || byte == '\n' || byte == '\v' || byte == '\r' || byte == '\f') {
        byte = ' ';
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
    deliverable_ = * p_subsystem.GetDeliverables();
    source_file.close();
    L("[EXIT] PARSE parser");
  }

}

Deliverable* Parser::GetDeliverables() {
  return & this->deliverable_;
}
