#include <datatype/DataType.h>
#include <model/Entity.h>
#include <model/Statement.h>
#include "component/SourceProcessor/PSubsystem.h"
#include "catch.hpp"

void MatchRequiredWithAnswer(std::vector<std::tuple<std::string, std::vector<std::vector<int>>>> required_list,
                             std::list<Variable*> var_list) {
  for (auto tup : required_list) {
    std::string var = std::get<0>(tup);
    std::vector<std::vector<int>> table = std::get<1>(tup);

    for (auto i = var_list.begin(); i != var_list.end(); ++i) {
      VariableName v_name = *(*i)->GetName();
      if (v_name.getName() == var) {
        std::vector<std::set<Statement*>> var_stmt_table = (*i)->GetStatementTable();

        for (int j = 0; j < table.size(); j++) {
          std::vector<int> row = table.at(j);
          std::set<Statement*> stmt_set = var_stmt_table.at(j);
          REQUIRE(row.size() == stmt_set.size()); // Possible extension, check row.at(x).stmtNo == stmt_set.find(y)
        }
      }
    }
  }
}

TEST_CASE("1.Model.Variable.Mapping") {
  psub::PSubsystem ps;
  ps.InitDataStructures();
  ps.ProcessStatement("procedure X {");
  ps.ProcessStatement("a = 1;");
  ps.ProcessStatement("b = 1;");
  ps.ProcessStatement("b = a;");
  ps.ProcessStatement("if (a == X) then {");
  ps.ProcessStatement("c = b;");
  ps.ProcessStatement("}");
  ps.ProcessStatement("else {");
  ps.ProcessStatement("b = c;");
  ps.ProcessStatement("}");

  ps.ProcessStatement("while (X == b) {");
  ps.ProcessStatement("q = a;");
  ps.ProcessStatement("q = b;");
  ps.ProcessStatement("q = c;");
  ps.ProcessStatement("q = d;");
  ps.ProcessStatement("q = X;");
  ps.ProcessStatement("print q;");
  ps.ProcessStatement("read q;");
  ps.ProcessStatement("}");
  ps.ProcessStatement("print a;");
  ps.ProcessStatement("read b;");
  ps.ProcessStatement("}");

  // { {}, {}, {}, {}, {}, {} }, If, While, Assign, Call (empty), Print, Read
  std::vector<std::tuple<std::string, std::vector<std::vector<int>>>> require_list = {
      {"X", { {4}, {7}, {13}, {}, {}, {} } },
      {"a", { {4}, {}, {1,3,7}, {}, {15}, {} } },
      {"b", { {}, {7}, {2,3,5,6,9}, {}, {}, {16} } },
      {"c", { {}, {}, {5, 6, 10}, {}, {}, {} } },
      {"d", { {}, {}, {11}, {}, {}, {} } },
      {"q", { {}, {}, {8,9,10,11,12}, {}, {13}, {14} } }
  };


  std::list<Variable*> actual_list = *ps.GetDeliverables()->GetVariableList();
  REQUIRE(require_list.size() == actual_list.size());
  MatchRequiredWithAnswer(require_list, actual_list);
}