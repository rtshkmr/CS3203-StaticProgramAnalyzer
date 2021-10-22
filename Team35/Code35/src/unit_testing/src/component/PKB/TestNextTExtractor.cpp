#include "catch.hpp"
#include <component/PKB/NextTExtractor.h>
#include "../../../utils/EntityUtils.h"
#include "../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("2.PKB.NextTExtractor basic conditions") {
  std::vector<Procedure*> proc_list;
  std::vector<Statement*> stmt_list;
  Block* b1 = new Block();
  Block* b2 = new Block();
  Block* b3 = new Block();
  Block* b4 = new Block();
  Block* b5 = new Block();
  Block* b6 = new Block();
  Cluster* c1 = new Cluster();
  Cluster* c2 = new Cluster();
  Cluster* c3 = new Cluster();
  Cluster* c4 = new Cluster();
  Cluster* c5 = new Cluster();
  Procedure* proc1 = GetProc1();
  Procedure* proc2 = GetProc1();
  Procedure* proc3 = GetProc1();
  Statement* s1 = CreateStatement(GetAssign1(), 1);
  Statement* s2 = CreateStatement(GetIf1(), 2);
  Statement* s3 = CreateStatement(GetAssign2(), 3);
  Statement* s4 = CreateStatement(GetPrintX(), 4);
  Statement* s5 = CreateStatement(GetAssign3(), 5);
  Statement* s6 = CreateStatement(GetAssign4(), 6);
  Statement* s7 = CreateStatement(GetReadI(), 7);

  SECTION("1 statement") {
    b1->AddStmt(StatementNumber(1));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(b1);

    proc_list.push_back(proc1);
    stmt_list.push_back(s1);

    NextTExtractor next_t_extractor{};
    std::vector<Entity*> actual = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(actual.empty());
  }

  SECTION("single level statement list") {
    b1->AddStmt(StatementNumber(1));
    b1->AddStmt(StatementNumber(2));
    b1->AddStmt(StatementNumber(3));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(b1);

    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3};

    NextTExtractor next_t_extractor{};
    std::vector<Entity*> expected_s1 = {s2, s3};
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 2);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> expected_s2 = {s3};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT("2", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
  }

  SECTION("1 if") {
    /*
     * s1
     * if2
     *  s3
     *  s4
     * else
     *  s5
     *  s6
     * s7
     */
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b3->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b4->AddStmt(StatementNumber(5));
    b4->AddStmt(StatementNumber(6));
    b5->AddStmt(StatementNumber(7));
    b1->AddNextBlock(b2);
    b2->AddNextBlock(b3);
    b2->AddNextBlock(b4);
    b3->AddNextBlock(b5);
    b4->AddNextBlock(b5);
    c2->AddChildClusterToBack(b2);
    c2->AddChildClusterToBack(b3);
    c2->AddChildClusterToBack(b4);
    c1->AddChildClusterToBack(b1);
    c1->AddChildClusterToBack(c2);
    c1->AddChildClusterToBack(b5);
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 7));
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3, s4, s5, s6, s7};

    NextTExtractor next_t_extractor{};
    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6, s7};
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 6);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> expected_s2 = {s3, s4, s5, s6, s7};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT("2", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> expected_s3 = {s4, s7};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT("3", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> expected_s4 = {s7};
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT("4", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s4));
    std::vector<Entity*> expected_s5 = {s6, s7};
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT("5", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
    std::vector<Entity*> expected_s6 = {s7};
    std::vector<Entity*> actual_s6 = next_t_extractor.GetNextT("6", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s6, expected_s6));
  }

  SECTION("1 while with next") {
    /*
     * s1
     * w2
     *  s3
     *  s4
     *  s5
     * s6
     */
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b2->isWhile = true;
    b3->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b3->AddStmt(StatementNumber(5));
    b4->AddStmt(StatementNumber(6));
    b1->AddNextBlock(b2);
    b2->AddNextBlock(b3);
    b2->AddNextBlock(b4);
    b3->AddNextBlock(b2);
    proc1->SetBlockRoot(b1);
    c1->AddChildClusterToBack(b1);
    c2->AddChildClusterToBack(b2);
    c2->AddChildClusterToBack(b3);
    c1->AddChildClusterToBack(c2);
    c1->AddChildClusterToBack(b4);
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 6));
    proc1->SetClusterRoot(c1);
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3, s4, s5, s6};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 5);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> expected_s2 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT("2", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> expected_s3 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT("3", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> expected_s4 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT("4", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s4));
    std::vector<Entity*> expected_s5 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT("5", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
  }

  SECTION("1 while without next") {
    /*
     * w1
     *  s2
     *  s3
     */
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b2->AddStmt(StatementNumber(3));
    b1->isWhile = true;
    b1->AddNextBlock(b2);
    b2->AddNextBlock(b1);
    c2->AddChildClusterToBack(b1);
    c2->AddChildClusterToBack(b2);
    c1->AddChildClusterToBack(c2);
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 3));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s1, s2, s3};
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 3);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> expected_s2 = {s1, s2, s3};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT("2", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> expected_s3 = {s1, s2, s3};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT("3", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
  }

  SECTION("multiple procedures") {
    /*
     * p1
     * w1
     *  s2
     *  c3
     * p2
     *  s4
     * p3
     * if5
     *  s6
     * else
     *  s7
     */
    b1->AddStmt(StatementNumber(1));
    b1->isWhile = true;
    b2->AddStmt(StatementNumber(2));
    b2->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b4->AddStmt(StatementNumber(5));
    b5->AddStmt(StatementNumber(6));
    b6->AddStmt(StatementNumber(7));
    b1->AddNextBlock(b2);
    b2->AddNextBlock(b1);
    b4->AddNextBlock(b5);
    b4->AddNextBlock(b6);
    c2->AddChildClusterToBack(b1);
    c2->AddChildClusterToBack(b2);
    c1->AddChildClusterToBack(c2);
    c3->AddChildClusterToBack(b3);
    c5->AddChildClusterToBack(b4);
    c5->AddChildClusterToBack(b5);
    c5->AddChildClusterToBack(b6);
    c4->AddChildClusterToBack(c5);
    c1->UpdateClusterRange();
    c3->UpdateClusterRange();
    c4->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 3));
    CHECK(c3->GetStartEndRange() == std::make_pair(4, 4));
    CHECK(c4->GetStartEndRange() == std::make_pair(5, 7));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    proc2->SetBlockRoot(b3);
    proc2->SetClusterRoot(c3);
    proc3->SetBlockRoot(b4);
    proc3->SetClusterRoot(c4);
    proc_list.push_back(proc1);
    proc_list.push_back(proc2);
    proc_list.push_back(proc3);
    stmt_list = {s1, s2, s3, s4, s5, s6, s7};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s1, s2, s3};
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT("1", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 3);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> expected_s2 = {s1, s2, s3};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT("2", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> expected_s3 = {s1, s2, s3};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT("3", proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> expected_s5 = {s6, s7};
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT("5", proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 4);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
  }
}

TEST_CASE("2.PKB.NextTExtractor nested containers") {}
