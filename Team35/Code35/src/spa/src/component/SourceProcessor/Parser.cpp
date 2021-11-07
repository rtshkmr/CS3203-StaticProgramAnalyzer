#include <string>
#include <vector>
#include <util/Logger.h>

#include "Parser.h"
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

    while (source_file.get(byte)) {
      bool byte_is_a_terminal_char =
          byte == '\t' || byte == '\a' || byte == '\n' || byte == '\v' || byte == '\r' || byte == '\f';
      if (byte_is_a_terminal_char) {
        byte = ' ';
      }

      if (byte == ' ') {
        if (chara.empty() || !chara.empty() && chara.back() == ' ') {
          continue;
        }
      }

      chara.push_back(byte);

      bool byte_is_desired_delimiter = byte == '{' || byte == ';' || byte == '}';
      if (byte_is_desired_delimiter) {
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
