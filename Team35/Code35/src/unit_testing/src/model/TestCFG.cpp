#include "catch.hpp"
#include "model/CFG.h"

TEST_CASE("1.CFG.Basic Block") {
  Block b;
  b.stmtNoList.insert(StatementNumber(1));
  b.stmtNoList.insert(StatementNumber(2));
  b.stmtNoList.insert(StatementNumber(3));

  REQUIRE(b.stmtNoList.find(StatementNumber(1)) != b.stmtNoList.end()); //check exist
  REQUIRE(b.stmtNoList.find(StatementNumber(2)) != b.stmtNoList.end());
  REQUIRE(b.stmtNoList.find(StatementNumber(3)) != b.stmtNoList.end());
  REQUIRE(b.stmtNoList.find(StatementNumber(4)) == b.stmtNoList.end());

  b.next_block_ = {&b}; //self-loop block
  Block* nextBlock = *b.next_block_.begin();

  REQUIRE(&b == nextBlock);
}
