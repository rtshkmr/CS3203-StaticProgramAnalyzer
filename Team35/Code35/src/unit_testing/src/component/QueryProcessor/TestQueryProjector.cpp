#include "catch.hpp"
#include <list>
#include <model/Statement.h>
#include "component/QueryProcessor/QueryProjector.h"
#include "../../../utils/EntityUtils.h"

TEST_CASE("3.QueryProjector.Stringify tables") {
  SECTION("all statements") {
    AssignEntity* a1 = entity_utils::GetAssign1();
    a1->SetStatementNumber(new StatementNumber(1));
    AssignEntity* a2 = entity_utils::GetAssign2();
    a2->SetStatementNumber(new StatementNumber(2));
    AssignEntity* a3 = entity_utils::GetAssign3();
    a3->SetStatementNumber(new StatementNumber(3));

    PrintEntity* p1 = entity_utils::GetPrintX();
    p1->SetStatementNumber(new StatementNumber(4));
    PrintEntity* p2 = entity_utils::GetPrintY();
    p2->SetStatementNumber(new StatementNumber(5));
    PrintEntity* p3 = entity_utils::GetPrintZ();
    p3->SetStatementNumber(new StatementNumber(6));

    ReadEntity* r1 = entity_utils::GetReadX();
    r1->SetStatementNumber(new StatementNumber(7));
    ReadEntity* r2 = entity_utils::GetReadX();
    r2->SetStatementNumber(new StatementNumber(8));
    ReadEntity* r3 = entity_utils::GetReadX();
    r3->SetStatementNumber(new StatementNumber(9));

    std::vector<Synonym*> synonyms = std::vector<Synonym*>{
        new Synonym("a", DesignEntity::kAssign),
        new Synonym("p", DesignEntity::kPrint),
        new Synonym("r", DesignEntity::kRead),
    };
    std::vector<std::vector<Entity*>> entity_table = std::vector<std::vector<Entity*>>{
        {a1, a2, a3},
        {p1, p2, p3},
        {r1, r2, r3},
    };

    std::vector<std::vector<std::string>> expected_table = std::vector<std::vector<std::string>>{
        {"1", "2", "3"},
        {"4", "5", "6"},
        {"7", "8", "9"},
    };
    std::vector<std::vector<std::string>> actual_table = QueryProjector::StringifyTable(synonyms, entity_table);
    CHECK(expected_table == actual_table);
  }

  SECTION("mixed things") {
    Procedure* proc1 = new Procedure(new ProcedureName("proc1"));
    Procedure* proc2 = new Procedure(new ProcedureName("proc2"));
    Procedure* proc3 = new Procedure(new ProcedureName("proc3"));
    Variable* vx = entity_utils::var_x_;
    Variable* vz = entity_utils::var_z_;
    Variable* vi = entity_utils::var_i_;
    Constant* c1 = new Constant(entity_utils::const_1_);
    Constant* c0 = new Constant(entity_utils::const_0_);
    Constant* c3 = new Constant(entity_utils::const_3_);

    std::vector<Synonym*> synonyms = std::vector<Synonym*>{
        new Synonym("a", DesignEntity::kProcedure),
        new Synonym("p", DesignEntity::kVariable),
        new Synonym("r", DesignEntity::kConstant),
    };
    std::vector<std::vector<Entity*>> entity_table = std::vector<std::vector<Entity*>>{
        {proc1, proc2, proc3},
        {vz, vx, vi},
        {c0, c1, c3},
    };

    std::vector<std::vector<std::string>> expected_table = std::vector<std::vector<std::string>>{
        {"proc1", "proc2", "proc3"},
        {"z", "x", "i"},
        {"0", "1", "3"},
    };
    std::vector<std::vector<std::string>> actual_table = QueryProjector::StringifyTable(synonyms, entity_table);
    CHECK(expected_table == actual_table);
  }
}

TEST_CASE("3.QueryProjector.Cross product tables") {
  SECTION("equal sized table") {
    std::vector<std::vector<std::string>> t1 = std::vector<std::vector<std::string>>{
        {"0", "2", "3"},
        {"a", "b", "c"},
        {"def", "ghi", "jkl"},
    };
    std::vector<std::vector<std::string>> t2 = std::vector<std::vector<std::string>>{
        {"7", "8", "9"},
        {"123", "456", "789"},
        {"x", "y", "z"}
    };
    std::vector<std::vector<std::string>> expected_table = std::vector<std::vector<std::string>>{
        {"0", "0", "0", "2", "2", "2", "3", "3", "3",},
        {"a", "a", "a", "b", "b", "b", "c", "c", "c",},
        {"def", "def", "def", "ghi", "ghi", "ghi", "jkl", "jkl", "jkl"},
        {"7", "8", "9", "7", "8", "9", "7", "8", "9"},
        {"123", "456", "789", "123", "456", "789", "123", "456", "789"},
        {"x", "y", "z", "x", "y", "z", "x", "y", "z"}
    };
    std::vector<std::vector<std::string>> actual_table = QueryProjector::CrossProductTables(t1, t2);
    CHECK(expected_table == actual_table);
  }

  SECTION("unequal sized table") {
    std::vector<std::vector<std::string>> t1 = std::vector<std::vector<std::string>>{
        {"0", "2", "3", "4"},
        {"a", "b", "c", "d"},
        {"def", "ghi", "jkl", "mno"},
    };
    std::vector<std::vector<std::string>> t2 = std::vector<std::vector<std::string>>{
        {"7", "8", "9"},
        {"123", "456", "789"},
        {"x", "y", "z"}
    };
    std::vector<std::vector<std::string>> expected_table = std::vector<std::vector<std::string>>{
        {"0", "0", "0", "2", "2", "2", "3", "3", "3", "4", "4", "4"},
        {"a", "a", "a", "b", "b", "b", "c", "c", "c", "d", "d", "d"},
        {"def", "def", "def", "ghi", "ghi", "ghi", "jkl", "jkl", "jkl", "mno", "mno", "mno"},
        {"7", "8", "9", "7", "8", "9", "7", "8", "9", "7", "8", "9"},
        {"123", "456", "789", "123", "456", "789", "123", "456", "789", "123", "456", "789"},
        {"x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z"}
    };
    std::vector<std::vector<std::string>> actual_table = QueryProjector::CrossProductTables(t1, t2);
    CHECK(expected_table == actual_table);
  }
}

TEST_CASE("3.QueryProjector.Reorder tables") {
  std::vector<Synonym> desired_order = std::vector<Synonym>{
      Synonym("b", DesignEntity::kVariable),
      Synonym("f", DesignEntity::kVariable),
      Synonym("a", DesignEntity::kAssign),
      Synonym("d", DesignEntity::kIf),
      Synonym("e", DesignEntity::kWhile),
      Synonym("c", DesignEntity::kProcedure),
  };
  std::list<Synonym*> current_order = std::list<Synonym*>{
      new Synonym("a", DesignEntity::kAssign),
      new Synonym("b", DesignEntity::kVariable),
      new Synonym("c", DesignEntity::kProcedure),
      new Synonym("d", DesignEntity::kIf),
      new Synonym("e", DesignEntity::kWhile),
      new Synonym("f", DesignEntity::kVariable),
  };
  std::vector<std::vector<std::string>> input_table = std::vector<std::vector<std::string>>{
      {"0", "0", "0", "2", "2", "2", "3", "3", "3", "4", "4", "4"},
      {"a", "a", "a", "b", "b", "b", "c", "c", "c", "d", "d", "d"},
      {"def", "def", "def", "ghi", "ghi", "ghi", "jkl", "jkl", "jkl", "mno", "mno", "mno"},
      {"7", "8", "9", "7", "8", "9", "7", "8", "9", "7", "8", "9"},
      {"123", "456", "789", "123", "456", "789", "123", "456", "789", "123", "456", "789"},
      {"x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z"}
  };
  std::vector<std::vector<std::string>> expected_table = std::vector<std::vector<std::string>>{
      {"a", "a", "a", "b", "b", "b", "c", "c", "c", "d", "d", "d"},
      {"x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z"},
      {"0", "0", "0", "2", "2", "2", "3", "3", "3", "4", "4", "4"},
      {"7", "8", "9", "7", "8", "9", "7", "8", "9", "7", "8", "9"},
      {"123", "456", "789", "123", "456", "789", "123", "456", "789", "123", "456", "789"},
      {"def", "def", "def", "ghi", "ghi", "ghi", "jkl", "jkl", "jkl", "mno", "mno", "mno"},
  };
  std::vector<std::vector<std::string>>
      actual_table = QueryProjector::ReorderTable(desired_order, current_order, input_table);
  CHECK(expected_table == actual_table);
}

TEST_CASE("3.QueryProjector.Join tuples of tables") {
  std::vector<std::vector<std::string>> input_table = std::vector<std::vector<std::string>>{
    {"a", "a", "a", "b", "b", "b", "c", "c", "c", "d", "d", "d"},
    {"x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z"},
    {"0", "0", "0", "2", "2", "2", "3", "3", "3", "4", "4", "4"},
    {"7", "8", "9", "7", "8", "9", "7", "8", "9", "7", "8", "9"},
    {"123", "456", "789", "123", "456", "789", "123", "456", "789", "123", "456", "789"},
    {"def", "def", "def", "ghi", "ghi", "ghi", "jkl", "jkl", "jkl", "mno", "mno", "mno"},
    };
  std::vector<std::string> expected_table = std::vector<std::string>{
    "a x 0 7 123 def", "a y 0 8 456 def", "a z 0 9 789 def",
     "b x 2 7 123 ghi", "b y 2 8 456 ghi", "b z 2 9 789 ghi",
     "c x 3 7 123 jkl", "c y 3 8 456 jkl", "c z 3 9 789 jkl",
     "d x 4 7 123 mno", "d y 4 8 456 mno", "d z 4 9 789 mno"};
  std::vector<std::string> actual_table = QueryProjector::JoinTuples(input_table);
  CHECK(expected_table == actual_table);
}

TEST_CASE("3.QueryProjector.1 target synonym") {
  AssignEntity* a1 = entity_utils::GetAssign1();
  a1->SetStatementNumber(new StatementNumber(1));
  AssignEntity* a2 = entity_utils::GetAssign2();
  a2->SetStatementNumber(new StatementNumber(2));
  AssignEntity* a3 = entity_utils::GetAssign3();
  a3->SetStatementNumber(new StatementNumber(3));

  Synonym* target_syn = new Synonym("a", DesignEntity::kAssign);
  QueryEvaluatorTable* table1 = new QueryEvaluatorTable(target_syn);
  std::vector<Entity*> col = std::vector<Entity*>{
      a1, a2, a3
  };
  table1->AddTargetSynonymValues(col);

  UnformattedQueryResult uqr = UnformattedQueryResult(true);
  uqr.AddTable(table1);

  std::vector<Synonym> syn_list = std::vector<Synonym>{*target_syn};
  QueryProjector qp = QueryProjector(syn_list);

  std::vector<std::string> expected_values = std::vector<std::string>{"1", "2", "3"};
  std::vector<std::string> actual_values = qp.FormatQuery(uqr);

  CHECK(expected_values == actual_values);
}
