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
  Cluster* c1 = new Cluster();
  Cluster* c2 = new Cluster();
  Cluster* c3 = new Cluster();
  Cluster* c4 = new Cluster();
  Cluster* c5 = new Cluster();
  Procedure* proc1 = GetProc1();
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

//  SECTION("1 while with next") {
//    /*
//     * s1
//     * w2
//     *  s3
//     *  s4
//     *  s5
//     * s6
//     */
//    Block* b1 = new Block();
//    Block* b2 = new Block();
//    Block* b3 = new Block();
//    Block* b4 = new Block();
//    b1->AddStmt(StatementNumber(1));
//    b2->AddStmt(StatementNumber(2));
//    b3->AddStmt(StatementNumber(3));
//    b3->AddStmt(StatementNumber(4));
//    b3->AddStmt(StatementNumber(5));
//    b4->AddStmt(StatementNumber(6));
//    Cluster* c1 = new Cluster();
//    c1->SetStartEnd(1, 6);
//    b1->AddNextBlock(b2);
//    b2->AddNextBlock(b3);
//    b2->AddNextBlock(b4);
//    b3->AddNextBlock(b2);
//    Procedure* proc1 = GetProc1();
//    proc1->SetBlockRoot(b1);
//    proc1->SetClusterRoot(c1);
//    deliverable.proc_list_.push_back(proc1);
//
//    Statement* s1 = CreateStatement(GetAssign1(), 1);
//    Statement* s2 = CreateStatement(GetWhileEntity1(), 2);
//    Statement* s3 = CreateStatement(GetAssign2(), 3);
//    Statement* s4 = CreateStatement(GetPrintX(), 4);
//    Statement* s5 = CreateStatement(GetAssign3(), 5);
//    Statement* s6 = CreateStatement(GetAssign4(), 6);
//    deliverable.AddStatement(s1);
//    deliverable.AddStatement(s2);
//    deliverable.AddStatement(s3);
//    deliverable.AddStatement(s4);
//    deliverable.AddStatement(s5);
//    deliverable.AddStatement(s6);
//    NextExtractor next_extractor{};
//    next_extractor.Extract(&deliverable);
//
//    CHECK(deliverable.next_hash_.size() == 5);
//    std::list<Statement*> expected_s1 = {s2};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s1)->second, expected_s1));
//    std::list<Statement*> expected_s2 = {s3, s6};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s2)->second, expected_s2));
//    std::list<Statement*> expected_s3 = {s4};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s3)->second, expected_s3));
//    std::list<Statement*> expected_s4 = {s5};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s4)->second, expected_s4));
//    std::list<Statement*> expected_s5 = {s2};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s5)->second, expected_s5));
//  }
//
//  SECTION("1 while without next") {
//    /*
//     * w1
//     *  s2
//     *  s3
//     */
//    Block* b1 = new Block();
//    Block* b2 = new Block();
//    b1->AddStmt(StatementNumber(1));
//    b2->AddStmt(StatementNumber(2));
//    b2->AddStmt(StatementNumber(3));
//    Cluster* c1 = new Cluster();
//    c1->SetStartEnd(1, 3);
//    b1->AddNextBlock(b2);
//    b2->AddNextBlock(b1);
//    Procedure* proc1 = GetProc1();
//    proc1->SetBlockRoot(b1);
//    proc1->SetClusterRoot(c1);
//    deliverable.proc_list_.push_back(proc1);
//
//    Statement* s1 = CreateStatement(GetWhileEntity1(), 1);
//    Statement* s2 = CreateStatement(GetAssign2(), 2);
//    Statement* s3 = CreateStatement(GetPrintX(), 3);
//    deliverable.AddStatement(s1);
//    deliverable.AddStatement(s2);
//    deliverable.AddStatement(s3);
//    NextExtractor next_extractor{};
//    next_extractor.Extract(&deliverable);
//
//    CHECK(deliverable.next_hash_.size() == 3);
//    std::list<Statement*> expected_s1 = {s2};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s1)->second, expected_s1));
//    std::list<Statement*> expected_s2 = {s3};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s2)->second, expected_s2));
//    std::list<Statement*> expected_s3 = {s1};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s3)->second, expected_s3));
//  }
//
//  SECTION("multiple procedures") {
//    /*
//     * p1
//     * w1
//     *  s2
//     *  c3
//     * p2
//     *  s4
//     * p3
//     * if5
//     *  s6
//     * else
//     *  s7
//     */
//    Block* b1 = new Block();
//    Block* b2 = new Block();
//    Block* b3 = new Block();
//    Block* b4 = new Block();
//    Block* b5 = new Block();
//    Block* b6 = new Block();
//    b1->AddStmt(StatementNumber(1));
//    b2->AddStmt(StatementNumber(2));
//    b2->AddStmt(StatementNumber(3));
//    b3->AddStmt(StatementNumber(4));
//    b4->AddStmt(StatementNumber(5));
//    b5->AddStmt(StatementNumber(6));
//    b6->AddStmt(StatementNumber(7));
//    Cluster* c1 = new Cluster();
//    Cluster* c2 = new Cluster();
//    Cluster* c3 = new Cluster();
//    c1->SetStartEnd(1, 3);
//    c2->SetStartEnd(4, 4);
//    c3->SetStartEnd(5, 7);
//    b1->AddNextBlock(b2);
//    b2->AddNextBlock(b1);
//    b4->AddNextBlock(b5);
//    b4->AddNextBlock(b6);
//    Procedure* proc1 = GetProc1();
//    proc1->SetBlockRoot(b1);
//    proc1->SetClusterRoot(c1);
//    Procedure* proc2 = GetProc1();
//    proc2->SetBlockRoot(b3);
//    proc2->SetClusterRoot(c2);
//    Procedure* proc3 = GetProc1();
//    proc3->SetBlockRoot(b4);
//    proc3->SetClusterRoot(c3);
//    deliverable.proc_list_.push_back(proc1);
//    deliverable.proc_list_.push_back(proc2);
//    deliverable.proc_list_.push_back(proc3);
//
//    Statement* s1 = CreateStatement(GetWhileEntity1(), 1);
//    Statement* s2 = CreateStatement(GetAssign1(), 2);
//    Statement* s3 = CreateStatement(GetAssign2(), 3);
//    Statement* s4 = CreateStatement(GetPrintX(), 4);
//    Statement* s5 = CreateStatement(GetIf1(), 5);
//    Statement* s6 = CreateStatement(GetAssign4(), 6);
//    Statement* s7 = CreateStatement(GetAssign5(), 7);
//    deliverable.AddStatement(s1);
//    deliverable.AddStatement(s2);
//    deliverable.AddStatement(s3);
//    deliverable.AddStatement(s4);
//    deliverable.AddStatement(s5);
//    deliverable.AddStatement(s6);
//    deliverable.AddStatement(s7);
//    NextExtractor next_extractor{};
//    next_extractor.Extract(&deliverable);
//
//    CHECK(deliverable.next_hash_.size() == 4);
//    std::list<Statement*> expected_s1 = {s2};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s1)->second, expected_s1));
//    std::list<Statement*> expected_s2 = {s3};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s2)->second, expected_s2));
//    std::list<Statement*> expected_s3 = {s1};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s3)->second, expected_s3));
//    std::list<Statement*> expected_s5 = {s6, s7};
//    CHECK(TestUtils::AreListsEqual(
//        *deliverable.next_hash_.find(s5)->second, expected_s5));
//  }
}

TEST_CASE("2.PKB.NextTExtractor nested containers") {}
