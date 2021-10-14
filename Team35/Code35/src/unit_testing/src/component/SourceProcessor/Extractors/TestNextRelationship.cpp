#include "catch.hpp"
#include "component/SourceProcessor/Extractors/NextExtractor.h"
#include "../../../../utils/EntityUtils.h"
#include "../../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("1.NextExtractor.basic conditions") {
  Deliverable deliverable;

  SECTION("1 statement") {
    Block* b1 = new Block();
    b1->AddStmt(StatementNumber(1));
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(b1);
    deliverable.proc_list_.push_back(proc1);

    Statement* s1 = CreateStatement(GetAssign1(), 1);
    deliverable.AddStatement(s1);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 0);
  }

  SECTION("single level statement list") {
    Block* b1 = new Block();
    b1->AddStmt(StatementNumber(1));
    b1->AddStmt(StatementNumber(2));
    b1->AddStmt(StatementNumber(3));
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(b1);
    deliverable.proc_list_.push_back(proc1);

    Statement* s1 = CreateStatement(GetAssign1(), 1);
    Statement* s2 = CreateStatement(GetPrintX(), 2);
    Statement* s3 = CreateStatement(GetReadI(), 3);
    deliverable.AddStatement(s1);
    deliverable.AddStatement(s2);
    deliverable.AddStatement(s3);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 2);
    std::list<Statement*> expected_s1 = {s2};
    CHECK(*deliverable.next_hash_.find(s1)->second == expected_s1);
    std::list<Statement*> expected_s2 = {s3};
    CHECK(*deliverable.next_hash_.find(s2)->second == expected_s2);
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
    Block* b1 = new Block();
    Block* b2 = new Block();
    Block* b3 = new Block();
    Block* b4 = new Block();
    Block* b5 = new Block();
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b3->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b4->AddStmt(StatementNumber(5));
    b4->AddStmt(StatementNumber(6));
    b5->AddStmt(StatementNumber(7));
    Cluster* c1 = new Cluster();
    c1->SetStartEnd(1, 7);
    b1->next_blocks_.insert(b2);
    b2->next_blocks_.insert(b3);
    b2->next_blocks_.insert(b4);
    b3->next_blocks_.insert(b5);
    b4->next_blocks_.insert(b5);
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    deliverable.proc_list_.push_back(proc1);

    Statement* s1 = CreateStatement(GetAssign1(), 1);
    Statement* s2 = CreateStatement(GetIf1(), 2);
    Statement* s3 = CreateStatement(GetAssign2(), 3);
    Statement* s4 = CreateStatement(GetPrintX(), 4);
    Statement* s5 = CreateStatement(GetAssign3(), 5);
    Statement* s6 = CreateStatement(GetAssign4(), 6);
    Statement* s7 = CreateStatement(GetReadI(), 7);
    deliverable.AddStatement(s1);
    deliverable.AddStatement(s2);
    deliverable.AddStatement(s3);
    deliverable.AddStatement(s4);
    deliverable.AddStatement(s5);
    deliverable.AddStatement(s6);
    deliverable.AddStatement(s7);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 6);
    std::list<Statement*> expected_s1 = {s2};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s1)->second, expected_s1));
    std::list<Statement*> expected_s2 = {s3, s5};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s2)->second, expected_s2));
    std::list<Statement*> expected_s3 = {s4};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s3)->second, expected_s3));
    std::list<Statement*> expected_s4 = {s7};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s4)->second, expected_s4));
    std::list<Statement*> expected_s5 = {s6};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s5)->second, expected_s5));
    std::list<Statement*> expected_s6 = {s7};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s6)->second, expected_s6));
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
    Block* b1 = new Block();
    Block* b2 = new Block();
    Block* b3 = new Block();
    Block* b4 = new Block();
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b3->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b3->AddStmt(StatementNumber(5));
    b4->AddStmt(StatementNumber(6));
    Cluster* c1 = new Cluster();
    c1->SetStartEnd(1, 6);
    b1->next_blocks_.insert(b2);
    b2->next_blocks_.insert(b3);
    b2->next_blocks_.insert(b4);
    b3->next_blocks_.insert(b2);
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    deliverable.proc_list_.push_back(proc1);

    Statement* s1 = CreateStatement(GetAssign1(), 1);
    Statement* s2 = CreateStatement(GetWhileEntity1(), 2);
    Statement* s3 = CreateStatement(GetAssign2(), 3);
    Statement* s4 = CreateStatement(GetPrintX(), 4);
    Statement* s5 = CreateStatement(GetAssign3(), 5);
    Statement* s6 = CreateStatement(GetAssign4(), 6);
    deliverable.AddStatement(s1);
    deliverable.AddStatement(s2);
    deliverable.AddStatement(s3);
    deliverable.AddStatement(s4);
    deliverable.AddStatement(s5);
    deliverable.AddStatement(s6);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 5);
    std::list<Statement*> expected_s1 = {s2};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s1)->second, expected_s1));
    std::list<Statement*> expected_s2 = {s3, s6};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s2)->second, expected_s2));
    std::list<Statement*> expected_s3 = {s4};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s3)->second, expected_s3));
    std::list<Statement*> expected_s4 = {s5};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s4)->second, expected_s4));
    std::list<Statement*> expected_s5 = {s2};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s5)->second, expected_s5));
  }

  SECTION("1 while without next") {
    /*
     * w1
     *  s2
     *  s3
     */
    Block* b1 = new Block();
    Block* b2 = new Block();
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b2->AddStmt(StatementNumber(3));
    Cluster* c1 = new Cluster();
    c1->SetStartEnd(1, 3);
    b1->next_blocks_.insert(b2);
    b2->next_blocks_.insert(b1);
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    deliverable.proc_list_.push_back(proc1);

    Statement* s1 = CreateStatement(GetWhileEntity1(), 1);
    Statement* s2 = CreateStatement(GetAssign2(), 2);
    Statement* s3 = CreateStatement(GetPrintX(), 3);
    deliverable.AddStatement(s1);
    deliverable.AddStatement(s2);
    deliverable.AddStatement(s3);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 3);
    std::list<Statement*> expected_s1 = {s2};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s1)->second, expected_s1));
    std::list<Statement*> expected_s2 = {s3};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s2)->second, expected_s2));
    std::list<Statement*> expected_s3 = {s1};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s3)->second, expected_s3));
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
    Block* b1 = new Block();
    Block* b2 = new Block();
    Block* b3 = new Block();
    Block* b4 = new Block();
    Block* b5 = new Block();
    Block* b6 = new Block();
    b1->AddStmt(StatementNumber(1));
    b2->AddStmt(StatementNumber(2));
    b2->AddStmt(StatementNumber(3));
    b3->AddStmt(StatementNumber(4));
    b4->AddStmt(StatementNumber(5));
    b5->AddStmt(StatementNumber(6));
    b6->AddStmt(StatementNumber(7));
    Cluster* c1 = new Cluster();
    Cluster* c2 = new Cluster();
    Cluster* c3 = new Cluster();
    c1->SetStartEnd(1, 3);
    c2->SetStartEnd(4, 4);
    c3->SetStartEnd(5, 7);
    b1->next_blocks_.insert(b2);
    b2->next_blocks_.insert(b1);
    b4->next_blocks_.insert(b5);
    b4->next_blocks_.insert(b6);
    Procedure* proc1 = GetProc1();
    proc1->SetBlockRoot(b1);
    proc1->SetClusterRoot(c1);
    Procedure* proc2 = GetProc1();
    proc2->SetBlockRoot(b3);
    proc2->SetClusterRoot(c2);
    Procedure* proc3 = GetProc1();
    proc3->SetBlockRoot(b4);
    proc3->SetClusterRoot(c3);
    deliverable.proc_list_.push_back(proc1);
    deliverable.proc_list_.push_back(proc2);
    deliverable.proc_list_.push_back(proc3);

    Statement* s1 = CreateStatement(GetWhileEntity1(), 1);
    Statement* s2 = CreateStatement(GetAssign1(), 2);
    Statement* s3 = CreateStatement(GetAssign2(), 3);
    Statement* s4 = CreateStatement(GetPrintX(), 4);
    Statement* s5 = CreateStatement(GetIf1(), 5);
    Statement* s6 = CreateStatement(GetAssign4(), 6);
    Statement* s7 = CreateStatement(GetAssign5(), 7);
    deliverable.AddStatement(s1);
    deliverable.AddStatement(s2);
    deliverable.AddStatement(s3);
    deliverable.AddStatement(s4);
    deliverable.AddStatement(s5);
    deliverable.AddStatement(s6);
    deliverable.AddStatement(s7);
    NextExtractor next_extractor{};
    next_extractor.Extract(&deliverable);

    CHECK(deliverable.next_hash_.size() == 4);
    std::list<Statement*> expected_s1 = {s2};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s1)->second, expected_s1));
    std::list<Statement*> expected_s2 = {s3};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s2)->second, expected_s2));
    std::list<Statement*> expected_s3 = {s1};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s3)->second, expected_s3));
    std::list<Statement*> expected_s5 = {s6, s7};
    CHECK(TestUtils::AreListsEqual(
        *deliverable.next_hash_.find(s5)->second, expected_s5));
  }
}

TEST_CASE("1.NextExtractor.nested containers") {
  SECTION("if x3") {}
  SECTION("while x3") {}
  SECTION("if and while") {}
  SECTION("multiple nesting of if and while") {}
}
