#include "catch.hpp"
#include <component/PKB/NextTExtractor.h>
#include "../../../utils/EntityUtils.h"
#include "../../../utils/TestUtils.h"

using namespace entity_utils;

void UpdateRanges(std::vector<Cluster*> clusters) {
  for (Cluster* cluster: clusters) {
    cluster->UpdateClusterRange();
  }
}

void AddChildClusters(Cluster* c, std::vector<Cluster*> clusters) {
  for (Cluster* cluster: clusters) {
    c->AddChildClusterToBack(cluster);
  }
}

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
    CHECK(next_t_extractor.GetNextT(1, proc_list, stmt_list).empty());
    CHECK_FALSE(next_t_extractor.HasNextT(1, 2, proc_list, stmt_list));
    CHECK(next_t_extractor.GetAllNextTLHS(proc_list, stmt_list).empty());
    CHECK(next_t_extractor.GetAllNextT(proc_list, stmt_list).empty());

    CHECK(next_t_extractor.GetPrevT(1, proc_list, stmt_list).empty());
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
    std::vector<Entity*> expected_s2 = {s3};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    CHECK(next_t_extractor.GetNextTSize() == 2);
    CHECK(next_t_extractor.HasNextT(1, 3, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(2, 3, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(2, 1, proc_list, stmt_list));
    std::vector<Entity*> expected_lhs = {s1, s2};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));
    std::vector<std::tuple<Entity*, Entity*>> expected_all = {{s1, s2}, {s1, s3}, {s2, s3}};
    std::vector<std::tuple<Entity*, Entity*>> actual_all = next_t_extractor.GetAllNextT(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_all, expected_all));

    std::vector<Entity*> expected_p1 = {s1, s2};
    std::vector<Entity*> expected_p2 = {s1};
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p2));
    std::vector<Entity*> actual_p1 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p1, expected_p1));
    CHECK(next_t_extractor.GetPrevTSize() == 2);
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
    AddChildClusters(c2, {b2, b3, b4});
    AddChildClusters(c1, {b1, c2, b5});
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 7));
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3, s4, s5, s6, s7};

    NextTExtractor next_t_extractor{};
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));
    CHECK(next_t_extractor.HasNextT(2, 7, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(1, 6, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(1, 7, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(3, 7, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 5, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(4, 5, proc_list, stmt_list));
    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6, s7};
    std::vector<Entity*> expected_s2 = {s3, s4, s5, s6, s7};
    std::vector<Entity*> expected_s3 = {s4, s7};
    std::vector<Entity*> expected_s4 = {s7};
    std::vector<Entity*> expected_s5 = {s6, s7};
    std::vector<Entity*> expected_s6 = {s7};
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
    std::vector<Entity*> actual_s6 = next_t_extractor.GetNextT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s6, expected_s6));
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s4));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    CHECK(next_t_extractor.GetNextTSize() == 6);
    std::vector<std::tuple<Entity*, Entity*>> expected_all = {{s1, s2}, {s1, s3}, {s1, s4}, {s1, s5}, {s1, s6},
                                                              {s1, s7}, {s2, s3}, {s2, s4}, {s2, s5}, {s2, s6},
                                                              {s2, s7}, {s3, s4}, {s3, s7}, {s4, s7}, {s5, s6},
                                                              {s5, s7}, {s6, s7}};
    std::vector<std::tuple<Entity*, Entity*>> actual_all = next_t_extractor.GetAllNextT(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_all, expected_all));

    std::vector<Entity*> expected_p2 = {s1};
    std::vector<Entity*> expected_p3 = {s1, s2};
    std::vector<Entity*> expected_p4 = {s1, s2, s3};
    std::vector<Entity*> expected_p5 = {s1, s2};
    std::vector<Entity*> expected_p6 = {s1, s2, s5};
    std::vector<Entity*> expected_p7 = {s1, s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p2));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p3));
    std::vector<Entity*> actual_p4 = next_t_extractor.GetPrevT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p4, expected_p4));
    std::vector<Entity*> actual_p5 = next_t_extractor.GetPrevT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p5, expected_p5));
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p6));
    std::vector<Entity*> actual_p7 = next_t_extractor.GetPrevT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p7, expected_p7));
    CHECK(next_t_extractor.GetPrevTSize() == 6);
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
    AddChildClusters(c2, {b2, b3});
    AddChildClusters(c1, {b1, c2, b4});
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 6));
    proc1->SetClusterRoot(c1);
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3, s4, s5, s6};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s1));
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s1));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    CHECK(next_t_extractor.HasNextT(2, 4, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(3, 2, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(1, 6, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(5, 6, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(3, 1, proc_list, stmt_list));
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s4, s5};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s1));
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s1));
    CHECK(next_t_extractor.GetNextTSize() == 5);
    std::vector<std::tuple<Entity*, Entity*>> expected_all = {{s1, s2}, {s1, s3}, {s1, s4}, {s1, s5}, {s1, s6},
                                                              {s2, s2}, {s2, s3}, {s2, s4}, {s2, s5}, {s2, s6},
                                                              {s3, s2}, {s3, s3}, {s3, s4}, {s3, s5}, {s3, s6},
                                                              {s4, s2}, {s4, s3}, {s4, s4}, {s4, s5}, {s4, s6},
                                                              {s5, s2}, {s5, s3}, {s5, s4}, {s5, s5}, {s5, s6}};
    std::vector<std::tuple<Entity*, Entity*>> actual_all = next_t_extractor.GetAllNextT(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_all, expected_all));

    std::vector<Entity*> expected_p6 = {s1, s2, s3, s4, s5};
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p6));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p6));
    std::vector<Entity*> actual_p4 = next_t_extractor.GetPrevT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p4, expected_p6));
    std::vector<Entity*> actual_p5 = next_t_extractor.GetPrevT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p5, expected_p6));
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p6));
    CHECK(next_t_extractor.GetPrevTSize() == 5);
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
    AddChildClusters(c2, {b1, b2});
    AddChildClusters(c1, {c2});
    c1->UpdateClusterRange();
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 3));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    proc_list.push_back(proc1);
    stmt_list = {s1, s2, s3};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s1, s2, s3};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_s1));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s1));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s1));
    CHECK(next_t_extractor.GetNextTSize() == 3);
    CHECK(next_t_extractor.HasNextT(1, 3, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(3, 2, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(2, 2, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(3, 4, proc_list, stmt_list));

    std::vector<Entity*> expected_p1 = {s1, s2, s3};
    std::vector<Entity*> actual_p1 = next_t_extractor.GetPrevT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p1, expected_p1));
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p1));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p1));
    CHECK(next_t_extractor.GetPrevTSize() == 3);
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
    AddChildClusters(c2, {b1, b2});
    AddChildClusters(c1, {c2});
    AddChildClusters(c3, {b3});
    AddChildClusters(c5, {b4, b5, b6});
    AddChildClusters(c4, {c5});
    UpdateRanges({c4, c3, c1});
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 3));
    CHECK(c3->GetStartEndRange() == std::make_pair(4, 4));
    CHECK(c4->GetStartEndRange() == std::make_pair(5, 7));
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    proc2->SetBlockRoot(b3);
    proc2->SetClusterRoot(c3);
    proc3->SetBlockRoot(b4);
    proc3->SetClusterRoot(c4);
    proc_list = {proc1, proc2, proc3};
    stmt_list = {s1, s2, s3, s4, s5, s6, s7};

    NextTExtractor next_t_extractor{};
    CHECK(next_t_extractor.HasNextT(1, 1, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(3, 2, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(5, 7, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(3, 4, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(4, 5, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 7, proc_list, stmt_list));
    std::vector<Entity*> expected_s1 = {s1, s2, s3};
    std::vector<Entity*> expected_s2 = {s1, s2, s3};
    std::vector<Entity*> expected_s3 = {s1, s2, s3};
    std::vector<Entity*> expected_s5 = {s6, s7};
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 1);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(next_t_extractor.GetNextTSize() == 4);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s5};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));
    std::vector<std::tuple<Entity*, Entity*>> expected_all = {{s1, s2}, {s1, s3}, {s1, s1},
                                                              {s2, s2}, {s2, s3}, {s2, s1},
                                                              {s3, s2}, {s3, s3}, {s3, s1},
                                                              {s5, s6}, {s5, s7}};
    std::vector<std::tuple<Entity*, Entity*>> actual_all = next_t_extractor.GetAllNextT(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_all, expected_all));

    std::vector<Entity*> expected_p1 = {s1, s2, s3};
    std::vector<Entity*> actual_p1 = next_t_extractor.GetPrevT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p1, expected_p1));
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p1));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p1));
    CHECK(next_t_extractor.GetPrevTSize() == 3);
    std::vector<Entity*> expected_p6 = {s5};
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p6));
    std::vector<Entity*> actual_p7 = next_t_extractor.GetPrevT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p7, expected_p6));
    CHECK(next_t_extractor.GetPrevTSize() == 5);
  }
}

TEST_CASE("2.PKB.NextTExtractor nested containers") {
  std::vector<Procedure*> proc_list;
  std::vector<Statement*> stmt_list;
  Block* b1 = new Block();
  Block* b2 = new Block();
  Block* b3 = new Block();
  Block* b4 = new Block();
  Block* b5 = new Block();
  Block* b6 = new Block();
  Block* b7 = new Block();
  Block* b8 = new Block();
  Block* b9 = new Block();
  Block* b10 = new Block();
  Block* b11 = new Block();
  Block* b12 = new Block();
  Cluster* c0 = new Cluster();
  Cluster* c1 = new Cluster();
  Cluster* c2 = new Cluster();
  Cluster* c3 = new Cluster();
  Cluster* c4 = new Cluster();
  Cluster* c5 = new Cluster();
  Cluster* c6 = new Cluster();
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
  Statement* s8 = CreateStatement(GetReadI(), 8);

  SECTION("if x3") {
    /*
     * if1
     *  if2
     *    s3
     *  else
     *    if4
     *      s5
     *    else
     *      s6
     * else
     *  s7
     * s8
     */
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b3->AddStmt(StatementNumber(3));
    b4->AddStmt(StatementNumber(4));
    b5->AddStmt(StatementNumber(5));
    b6->AddStmt(StatementNumber(6));
    b7->AddStmt(StatementNumber(7));
    b8->AddStmt(StatementNumber(8));
    b1->AddNextBlock(b2);
    b1->AddNextBlock(b7);
    b2->AddNextBlock(b3);
    b2->AddNextBlock(b4);
    b3->AddNextBlock(b8);
    b4->AddNextBlock(b5);
    b4->AddNextBlock(b6);
    b5->AddNextBlock(b8);
    b6->AddNextBlock(b8);
    b7->AddNextBlock(b8);
    proc1->SetBlockRoot(b1);
    AddChildClusters(c3, {b4, b5, b6});
    AddChildClusters(c2, {b2, b3, c3});
    AddChildClusters(c1, {b1, c2, b7, b8});
    UpdateRanges({c3, c2, c1});
    proc1->SetClusterRoot(c1);
    proc_list.push_back(proc1);
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 8));

    stmt_list = {s1, s2, s3, s4, s5, s6, s7, s8};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6, s7, s8};
    std::vector<Entity*> expected_s2 = {s3, s4, s5, s6, s8};
    std::vector<Entity*> expected_s3 = {s8};
    std::vector<Entity*> expected_s4 = {s5, s6, s8};
    std::vector<Entity*> expected_s5 = {s8};
    std::vector<Entity*> expected_s6 = {s8};
    std::vector<Entity*> expected_s7 = {s8};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s4));
    std::vector<Entity*> actual_s7 = next_t_extractor.GetNextT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s7, expected_s7));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    std::vector<Entity*> actual_s6 = next_t_extractor.GetNextT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s6, expected_s6));
    CHECK(next_t_extractor.HasNextT(1, 6, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(2, 8, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(4, 8, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(7, 8, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(1, 1, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(3, 4, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 7, proc_list, stmt_list));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
    CHECK(next_t_extractor.GetNextTSize() == 7);
    std::vector<std::tuple<Entity*, Entity*>> expected_all = {{s1, s2}, {s1, s3}, {s1, s4}, {s1, s5}, {s1, s6},
                                                              {s1, s7}, {s1, s8}, {s2, s3}, {s2, s4}, {s2, s5},
                                                              {s2, s6}, {s2, s8}, {s3, s8}, {s5, s8}, {s6, s8},
                                                              {s7, s8}, {s4, s5}, {s4, s6}, {s4, s8}};
    std::vector<std::tuple<Entity*, Entity*>> actual_all = next_t_extractor.GetAllNextT(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_all, expected_all));
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s4, s5, s6, s7};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));

    std::vector<Entity*> expected_p2 = {s1};
    std::vector<Entity*> expected_p3 = {s1, s2};
    std::vector<Entity*> expected_p5 = {s1, s2, s4};
    std::vector<Entity*> expected_p7 = {s1};
    std::vector<Entity*> expected_p8 = {s1, s2, s3, s4, s5, s6, s7};
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p2));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p3));
    std::vector<Entity*> actual_p4 = next_t_extractor.GetPrevT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p4, expected_p3));
    std::vector<Entity*> actual_p5 = next_t_extractor.GetPrevT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p5, expected_p5));
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p5));
    std::vector<Entity*> actual_p7 = next_t_extractor.GetPrevT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p7, expected_p7));
    std::vector<Entity*> actual_p8 = next_t_extractor.GetPrevT(8, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p8, expected_p8));
    CHECK(next_t_extractor.GetPrevTSize() == 7);
  }

  SECTION("while x3") {
    /*
     * w1
     *  w2
     *    s3
     *    w4
     *      s5
     * s6
     */
    b1->AddStmt(StatementNumber(1));
    b1->isWhile = true;
    b2->AddStmt(StatementNumber(2));
    b2->isWhile = true;
    b3->AddStmt(StatementNumber(3));
    b4->AddStmt(StatementNumber(4));
    b4->isWhile = true;
    b5->AddStmt(StatementNumber(5));
    b6->AddStmt(StatementNumber(6));
    b1->AddNextBlock(b2);
    b1->AddNextBlock(b6);
    b2->AddNextBlock(b3);
    b2->AddNextBlock(b1);
    b3->AddNextBlock(b4);
    b4->AddNextBlock(b5);
    b4->AddNextBlock(b2);
    b5->AddNextBlock(b4);
    proc1->SetBlockRoot(b1);
    AddChildClusters(c4, {b4, b5});
    AddChildClusters(c3, {b2, b3, c4});
    AddChildClusters(c2, {b1, c3});
    AddChildClusters(c1, {c2, b6});
    proc1->SetClusterRoot(c1);
    UpdateRanges({c4, c3, c2, c1});
    proc_list.push_back(proc1);
    CHECK(c1->GetStartEndRange() == std::make_pair(1, 6));

    stmt_list = {s1, s2, s3, s4, s5, s6};

    NextTExtractor next_t_extractor{};

    CHECK(next_t_extractor.HasNextT(3, 3, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(5, 2, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(4, 1, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(3, 6, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 1, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 5, proc_list, stmt_list));
    std::vector<Entity*> expected_s1 = {s1, s2, s3, s4, s5, s6};
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s1));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s1));
    std::vector<Entity*> actual_s4 = next_t_extractor.GetNextT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s4, expected_s1));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s1));
    CHECK(next_t_extractor.GetNextTSize() == 5);
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s4, s5};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));

    std::vector<Entity*> expected_p1 = {s1, s2, s3, s4, s5};
    std::vector<Entity*> actual_p1 = next_t_extractor.GetPrevT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p1, expected_p1));
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p1));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p1));
    std::vector<Entity*> actual_p4 = next_t_extractor.GetPrevT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p4, expected_p1));
    std::vector<Entity*> actual_p5 = next_t_extractor.GetPrevT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p5, expected_p1));
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p1));
    CHECK(next_t_extractor.GetPrevTSize() == 6);
  }

  SECTION("Mixed containers multiple procedures") {
    /*
     * proc1
     * if1
     *  w2
     *    s3
     *  s4
     * else
     *  s5
     *    w6
     *      s7
     * proc2
     * w8
     *  if9
     *    s10
     *  else
     *    s11
     * s12
     */
    Statement* s9 = CreateStatement(GetAssign2(), 9);
    Statement* s10 = CreateStatement(GetAssign2(), 10);
    Statement* s11 = CreateStatement(GetAssign2(), 11);
    Statement* s12 = CreateStatement(GetAssign2(), 12);
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b2->isWhile = true;
    b3->AddStmt(StatementNumber(3));
    b4->AddStmt(StatementNumber(4));
    b5->AddStmt(StatementNumber(5));
    b6->AddStmt(StatementNumber(6));
    b6->isWhile = true;
    b7->AddStmt(StatementNumber(7));
    b8->AddStmt(StatementNumber(8));
    b8->isWhile = true;
    b9->AddStmt(StatementNumber(9));
    b10->AddStmt(StatementNumber(10));
    b11->AddStmt(StatementNumber(11));
    b12->AddStmt(StatementNumber(12));
    b1->AddNextBlock(b2);
    b1->AddNextBlock(b5);
    b2->AddNextBlock(b3);
    b2->AddNextBlock(b4);
    b3->AddNextBlock(b2);
    b5->AddNextBlock(b6);
    b6->AddNextBlock(b7);
    b7->AddNextBlock(b6);
    b8->AddNextBlock(b9);
    b8->AddNextBlock(b12);
    b9->AddNextBlock(b10);
    b9->AddNextBlock(b11);
    b10->AddNextBlock(b8);
    b11->AddNextBlock(b8);
    AddChildClusters(c3, {b6, b7});
    AddChildClusters(c2, {b2, b3});
    AddChildClusters(c1, {b1, c2, b4, b5, c3});
    AddChildClusters(c0, {c1});
    AddChildClusters(c6, {b9, b10, b11});
    AddChildClusters(c5, {b8, c6});
    AddChildClusters(c4, {c5, b12});
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c0);
    proc2->SetClusterRoot(c4);
    proc2->SetBlockRoot(b8);
    UpdateRanges({c6, c5, c4, c3, c2, c1, c0});
    proc_list.push_back(proc1);
    proc_list.push_back(proc2);
    CHECK(c0->GetStartEndRange() == std::make_pair(1, 7));
    CHECK(c4->GetStartEndRange() == std::make_pair(8, 12));

    stmt_list = {s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12};

    NextTExtractor next_t_extractor{};

    std::vector<Entity*> expected_s1 = {s2, s3, s4, s5, s6, s7};
    std::vector<Entity*> expected_s2 = {s2, s3, s4};
    std::vector<Entity*> expected_s3 = {s2, s3, s4};
    std::vector<Entity*> expected_s5 = {s6, s7};
    std::vector<Entity*> expected_s6 = {s6, s7};
    std::vector<Entity*> expected_s7 = {s6, s7};
    std::vector<Entity*> expected_s8 = {s8, s9, s10, s11, s12};
    std::vector<Entity*> actual_s10 = next_t_extractor.GetNextT(10, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s10, expected_s8));
    std::vector<Entity*> actual_s2 = next_t_extractor.GetNextT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s2, expected_s2));
    CHECK(next_t_extractor.GetNextTSize() == 6);
    CHECK(next_t_extractor.HasNextT(3, 4, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(7, 6, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(10, 9, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(11, 12, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(4, 6, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(6, 8, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(7, 9, proc_list, stmt_list));
    std::vector<Entity*> actual_s1 = next_t_extractor.GetNextT(1, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s1, expected_s1));
    std::vector<Entity*> actual_s3 = next_t_extractor.GetNextT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s3, expected_s3));
    std::vector<Entity*> actual_s7 = next_t_extractor.GetNextT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s7, expected_s7));
    std::vector<Entity*> actual_s5 = next_t_extractor.GetNextT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s5, expected_s5));
    std::vector<Entity*> actual_s6 = next_t_extractor.GetNextT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s6, expected_s6));
    std::vector<Entity*> actual_s11 = next_t_extractor.GetNextT(11, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s11, expected_s8));
    std::vector<Entity*> actual_s8 = next_t_extractor.GetNextT(8, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s8, expected_s8));
    std::vector<Entity*> actual_s9 = next_t_extractor.GetNextT(9, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_s9, expected_s8));
    CHECK(next_t_extractor.GetNextTSize() == 10);
    CHECK(next_t_extractor.HasNextT(10, 12, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(1, 3, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(1, 5, proc_list, stmt_list));
    CHECK(next_t_extractor.HasNextT(5, 6, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(4, 4, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(12, 1, proc_list, stmt_list));
    CHECK_FALSE(next_t_extractor.HasNextT(2, 5, proc_list, stmt_list));
    std::vector<Entity*> expected_lhs = {s1, s2, s3, s5, s6, s7, s8, s9, s10, s11};
    std::vector<Entity*> actual_lhs = next_t_extractor.GetAllNextTLHS(proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_lhs, expected_lhs));

    std::vector<Entity*> expected_p4 = {s1, s2, s3};
    std::vector<Entity*> expected_p5 = {s1};
    std::vector<Entity*> expected_p7 = {s1, s5, s6, s7};
    std::vector<Entity*> expected_p12 = {s8, s9, s10, s11};
    std::vector<Entity*> actual_p2 = next_t_extractor.GetPrevT(2, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p2, expected_p4));
    std::vector<Entity*> actual_p3 = next_t_extractor.GetPrevT(3, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p3, expected_p4));
    std::vector<Entity*> actual_p4 = next_t_extractor.GetPrevT(4, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p4, expected_p4));
    std::vector<Entity*> actual_p5 = next_t_extractor.GetPrevT(5, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p5, expected_p5));
    std::vector<Entity*> actual_p6 = next_t_extractor.GetPrevT(6, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p6, expected_p7));
    std::vector<Entity*> actual_p7 = next_t_extractor.GetPrevT(7, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p7, expected_p7));
    CHECK(next_t_extractor.GetPrevTSize() == 6);
    std::vector<Entity*> actual_p8 = next_t_extractor.GetPrevT(8, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p8, expected_p12));
    std::vector<Entity*> actual_p9 = next_t_extractor.GetPrevT(9, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p9, expected_p12));
    std::vector<Entity*> actual_p10 = next_t_extractor.GetPrevT(10, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p10, expected_p12));
    std::vector<Entity*> actual_p11 = next_t_extractor.GetPrevT(11, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p11, expected_p12));
    std::vector<Entity*> actual_p12 = next_t_extractor.GetPrevT(12, proc_list, stmt_list);
    CHECK(TestUtils::AreVectorsEqual(actual_p12, expected_p12));
    CHECK(next_t_extractor.GetPrevTSize() == 11);
  }
}
