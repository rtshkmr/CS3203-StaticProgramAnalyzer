#include "catch.hpp"
#include "model/CFG.h"

TEST_CASE("1.CFG.Block.StatementRange") {
  Block b;

  REQUIRE_FALSE(b.find(3));
  REQUIRE(b.size() == 0);

  b.AddStmt(StatementNumber(2));
  b.AddStmt(StatementNumber(3));
  b.AddStmt(StatementNumber(4));
  b.AddStmt(StatementNumber(5));
  b.AddStmt(StatementNumber(6));

  REQUIRE(b.size() == 5);
  REQUIRE_FALSE(b.find(StatementNumber(1)));
  REQUIRE(b.find(StatementNumber(2)));
  REQUIRE(b.find(StatementNumber(3)));
  REQUIRE(b.find(StatementNumber(4)));
  REQUIRE(b.find(StatementNumber(5)));
  REQUIRE(b.find(StatementNumber(6)));
  REQUIRE_FALSE(b.find(StatementNumber(7)));

  SECTION("Remove middle stmt") {
    REQUIRE_THROWS(b.RemoveStmt(StatementNumber(4)));
  }

  SECTION("Remove tail stmt - success") {
    b.RemoveStmt(6);
    REQUIRE_FALSE(b.find(StatementNumber(6)));
  }

  SECTION("Remove tail stmt twice - success") {
    b.RemoveStmt(6);
    b.RemoveStmt(5);
    REQUIRE_FALSE(b.find(StatementNumber(5)));
  }

  SECTION("Remove head stmt - success") {
    b.RemoveStmt(2);
    REQUIRE_FALSE(b.find(StatementNumber(2)));
  }

  SECTION("Remove tail stmt twice - success") {
    b.RemoveStmt(2);
    b.RemoveStmt(3);
    REQUIRE_FALSE(b.find(StatementNumber(3)));
  }
}

TEST_CASE("1.CFG.Block.Linkage") {
  SECTION ("Self-Linkage") {
    Block b;
    b.next_block_ = {&b}; //self-loop block
    Block* nextBlock = *b.next_block_.begin();

    REQUIRE(&b == nextBlock);
  }
}
