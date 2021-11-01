#include <component/SourceProcessor/SourceProcessor.h>
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

  SECTION("Remove head stmt twice - success") {
    b.RemoveStmt(2);
    b.RemoveStmt(3);
    REQUIRE_FALSE(b.CheckIfStatementInRange(StatementNumber(3)));
  }
}

TEST_CASE("1.CFG.Block.Linkage") {
  SECTION ("Self-Linkage") {
    Block b;
    b.AddNextBlock(& b); //self-loop block
    Block* nextBlock = * b.GetNextBlocks().begin();

    REQUIRE(& b == nextBlock);
  }
}

TEST_CASE("1.CFG.Cluster") {
  SECTION("Basic class hierarchies") {
    SECTION("AddChildClusterToBack and AddSiblingCluster functions") {
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
      // tests get next and get prev siblings
      Cluster* next_sibling = child_cluster->GetNextSiblingCluster();
      Cluster* prev_sibling = next_sibling->GetPrevSiblingCluster();
      REQUIRE(next_sibling == sibling_cluster);
      REQUIRE(prev_sibling == child_cluster);
    }
  }

  SECTION("Navigation Convenience Helpers") {

    // set up valid if cluster with constituents
    Cluster* if_cluster = new Cluster();
    if_cluster->SetClusterTag(ClusterTag::kIfCluster);
    Cluster* if_cond = new Cluster();
    Cluster* if_body = new Cluster();
    Cluster* else_body = new Cluster();
    if_cond->SetClusterTag(ClusterTag::kIfCond);
    if_body->SetClusterTag(ClusterTag::kIfBody);
    else_body->SetClusterTag(ClusterTag::kElseBody);
    if_cluster->AddChildClusterToBack(if_cond);
    if_cluster->AddChildClusterToBack(if_body);
    if_cluster->AddChildClusterToBack(else_body);


    // setup valid while cluster with constituents
    Cluster* while_cluster = new Cluster();
    Cluster* while_cond = new Cluster();
    Cluster* while_body = new Cluster();
    while_cluster->SetClusterTag(ClusterTag::kWhileCluster);
    while_cond->SetClusterTag(ClusterTag::kWhileCond);
    while_body->SetClusterTag(ClusterTag::kWhileBody);
    while_cluster->AddChildClusterToBack(while_cond);
    while_cluster->AddChildClusterToBack(while_body);

    SECTION("Getting Cluster Constituents") {
      bool correctly_found_if_cond_constituent = if_cond == if_cluster->GetClusterConstituent(ClusterTag::kIfCond);
      bool correctly_found_if_body_constituent = if_body == if_cluster->GetClusterConstituent(ClusterTag::kIfBody);
      bool
          correctly_found_else_body_constituent = else_body == if_cluster->GetClusterConstituent(ClusterTag::kElseBody);
      REQUIRE(correctly_found_if_cond_constituent);
      REQUIRE(correctly_found_if_body_constituent);
      REQUIRE(correctly_found_else_body_constituent);

      // while cluster with valid constituents:
      bool correctly_found_while_cond_constituent =
          while_cond == while_cluster->GetClusterConstituent(ClusterTag::kWhileCond);
      bool correctly_found_while_body_constituent =
          while_body == while_cluster->GetClusterConstituent(ClusterTag::kWhileBody);
      REQUIRE(correctly_found_while_cond_constituent);
      REQUIRE(correctly_found_while_body_constituent);
    }

    SECTION("Find next sibling via tags") {
      bool correctly_found_if_body_from_cond = if_body == if_cond->FindNextSibling(ClusterTag::kIfBody);
      bool correctly_found_else_body_from_cond = else_body == if_cond->FindNextSibling(ClusterTag::kElseBody);
      bool correctly_found_else_body_from_if_body = else_body == if_body->FindNextSibling(ClusterTag::kElseBody);
      REQUIRE(correctly_found_if_body_from_cond);
      REQUIRE(correctly_found_else_body_from_cond);
      REQUIRE(correctly_found_else_body_from_if_body);
    }

  }

  SECTION("Traversal Helper Functions") {
    // set up pkb and everything using
    PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops_source.txt");
    Procedure* proc = dynamic_cast<Procedure*>(pkb->GetDesignEntities(DesignEntity::kProcedure).front());

    // reusable objects:
    Cluster* inner_cluster_4_5 = proc->GetInnermostCluster(4, 5, nullptr);

    SECTION("Getting innermost cluster") {
      Cluster* inner_cluster_4_7 = proc->GetInnermostCluster(4, 7, nullptr);
      auto range_for_4_5 = inner_cluster_4_5->GetStartEndRange();
      bool output_4_5 = (range_for_4_5.first == 4 && range_for_4_5.second == 17);
      REQUIRE(output_4_5);
      auto range_for_4_7 = inner_cluster_4_7->GetStartEndRange();
      bool output_4_7 = (range_for_4_7.first == 4 && range_for_4_7.second == 17); // it's the entire if cluster
      REQUIRE(output_4_7);
      Cluster* inner_cluster_13_15 = proc->GetInnermostCluster(13, 15, nullptr);
      auto range_for_13_15 = inner_cluster_13_15->GetStartEndRange();
      bool output_13_15 = range_for_13_15.first == 11 && range_for_13_15.second == 15;
      Cluster* inner_cluster_16_19 = proc->GetInnermostCluster(16, 19, nullptr);
      auto range_for_16_19 = inner_cluster_16_19->GetStartEndRange();
      bool output_16_19 = range_for_16_19.first == 1 && range_for_16_19.second == 23;
      REQUIRE(output_16_19);
    }
    SECTION("Affects Traversal Helper for HasAffects(#,#)") {
//      bool valid_unmod_chara_4_5 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects,
//                                                        inner_cluster_4_5,
//                                                        std::make_pair(4, 5),
//                                                        pkb,
//                                                        "chara");
//      REQUIRE(valid_unmod_chara_4_5);

//      Cluster* inner_cluster_5_10 = proc->GetInnermostCluster(5, 10, nullptr);
//      bool invalid_unmod_procedure_5_10 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_5_10,
//                                                                    std::make_pair(5, 10),
//                                                                    pkb,
//                                                                    "procedure");
//      REQUIRE_FALSE(invalid_unmod_procedure_5_10);
    }
  }
}



