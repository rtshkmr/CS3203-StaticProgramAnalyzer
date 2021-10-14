#include "catch.hpp"
#include "model/CFG.h"
#include "component/SourceProcessor/PSubsystem.h"
using psub::PSubsystem;

TEST_CASE("1.CFG.Block.StatementRange") {
  Block b;

  REQUIRE_FALSE(b.CheckIfStatementInRange(3));
  REQUIRE(b.size() == 0);

  b.AddStmt(StatementNumber(2));
  b.AddStmt(StatementNumber(3));
  b.AddStmt(StatementNumber(4));
  b.AddStmt(StatementNumber(5));
  b.AddStmt(StatementNumber(6));

  REQUIRE(b.size() == 5);
  REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(1)));
  REQUIRE(b.CheckIfStatementInRange(StatementNumber(2)));
  REQUIRE(b.CheckIfStatementInRange(StatementNumber(3)));
  REQUIRE(b.CheckIfStatementInRange(StatementNumber(4)));
  REQUIRE(b.CheckIfStatementInRange(StatementNumber(5)));
  REQUIRE(b.CheckIfStatementInRange(StatementNumber(6)));
  REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(7)));

  SECTION("Remove middle stmt") {
    REQUIRE_THROWS(b.RemoveStmt(StatementNumber(4)));
  }

  SECTION("Remove tail stmt - success") {
    b.RemoveStmt(6);
    REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(6)));
  }

  SECTION("Remove tail stmt twice - success") {
    b.RemoveStmt(6);
    b.RemoveStmt(5);
    REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(5)));
  }

  SECTION("Remove head stmt - success") {
    b.RemoveStmt(2);
    REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(2)));
  }

  SECTION("Remove tail stmt twice - success") {
    b.RemoveStmt(2);
    b.RemoveStmt(3);
    REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(3)));
  }
}

TEST_CASE("1.CFG.Block.Linkage") {
  SECTION ("Self-Linkage") {
    Block b;
    b.next_blocks_ = {&b}; //self-loop block
    Block* nextBlock = *b.next_blocks_.begin();

    REQUIRE(&b == nextBlock);
  }
}

TEST_CASE("1.CFG.Cluster") {
  SECTION("Basic class hierarchies") {
    SECTION("AddChildClusterToBack and AddSiblingCluster functions"){
      Cluster* outer_cluster = new Cluster();
      Cluster* child_cluster = new Cluster();
      Cluster* sibling_cluster = new Cluster();
      outer_cluster->AddChildClusterToBack(child_cluster);
      REQUIRE(child_cluster->GetParentCluster() == outer_cluster); // auto links to parent
      child_cluster->AddSiblingCluster(sibling_cluster);
      REQUIRE(sibling_cluster->GetParentCluster() == outer_cluster); // auto links to parent
      std::list<Cluster*> nested_clusters = outer_cluster->GetNestedClusters();
      REQUIRE(nested_clusters.size() == 2);
      REQUIRE(nested_clusters.front() == child_cluster);
      REQUIRE(nested_clusters.back() == sibling_cluster);
      Cluster* next_sibling = child_cluster->GetNextSiblingCluster();
      Cluster* prev_sibling = next_sibling->GetPrevSiblingCluster();
      REQUIRE(next_sibling == sibling_cluster);
      REQUIRE(prev_sibling == child_cluster);

    }
  }
}



