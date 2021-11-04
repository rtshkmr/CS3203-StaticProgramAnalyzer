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
      // ========================= USE THIS TO VERIFY / ADD TEST CASES ==================================
      //      procedure Parser {
      //          1.    psubsystem = 0;
      //          2.    read fileName;
      //
      //          3.    if (fileName != 0) then {
      //            4.       chara = 0;
      //            5.       byte = 0;
      //
      //            6.       while (fileName > 1) {
      //              7.           read byte;
      //              8.           if (byte != 99) then {
      //                    9.               chara = chara + byte;
      //                    10.               lastByte = byte;
      //                    11.               if (byte == 88) then {
      //                      12.                   psubsystem = psubsystem + 1;
      //                      13.                   print ProcessStmtNow;
      //                      14.                   chara = 0;
      //                    } else {
      //                      15.                   print cont1nueREad1ng;
      //                    }
      //                    16.               fileName = fileName - 1;
      //                } else {
      //                17.               print SyntaxErr0rFound;
      //              }
      //            }
      //          } else {
      //            18.        print InvalidFileName;
      //          }
      //
      //          19.    if (fileName == 1) then {
      //            20.        print pr0cessSuccessfuI;
      //          } else {
      //            21.        print SyntaxErr0rFound;
      //          }
      //
      //          22.    print psubsystem;
      //          23.    psubsystem = 0;
      //      }
      // ================================================================================================
      bool valid_unmod_chara_4_5 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects,
                                                                  inner_cluster_4_5,
                                                                  std::make_pair(4, 5),
                                                                  pkb,
                                                                  "chara");
      REQUIRE(valid_unmod_chara_4_5);

      Cluster* inner_cluster_5_10 = proc->GetInnermostCluster(5, 10, nullptr);
      bool invalid_unmod_procedure_5_10 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_5_10,
                                                                         std::make_pair(5, 10),
                                                                         pkb,
                                                                         "byte");
      REQUIRE_FALSE(invalid_unmod_procedure_5_10);
      Cluster* inner_cluster_9_14 = proc->GetInnermostCluster(9, 14, nullptr);
      bool valid_unmod_procedure_9_14 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_9_14,
                                                                       std::make_pair(9, 14),
                                                                       pkb,
                                                                       "chara");
      REQUIRE(valid_unmod_procedure_9_14);
      Cluster* inner_cluster_9_15 = proc->GetInnermostCluster(9, 15, nullptr);
      bool valid_unmod_procedure_9_15 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_9_15,
                                                                       std::make_pair(9, 15),
                                                                       pkb,
                                                                       "chara");
      REQUIRE(inner_cluster_9_15 == inner_cluster_9_14);
      REQUIRE(valid_unmod_procedure_9_15);
      Cluster* inner_cluster_1_18 = proc->GetInnermostCluster(1, 18, nullptr);
      bool valid_unmod_procedure_1_18 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_1_18,
                                                                       std::make_pair(1, 18),
                                                                       pkb,
                                                                       "psubsystem");
      REQUIRE(valid_unmod_procedure_1_18);

      /// This traversal function will work on non-assign statements also. Note that the rule of "both start and end stmt have to be assign stmt" is handled upstream from this function call
      Cluster* inner_cluster_2_16 = proc->GetInnermostCluster(2, 16, nullptr);
      bool valid_unmod_procedure_2_16 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_2_16,
                                                                       std::make_pair(2, 16),
                                                                       pkb,
                                                                       "fileName");
      REQUIRE(valid_unmod_procedure_2_16);
      Cluster* inner_cluster_7_16 = proc->GetInnermostCluster(7, 16, nullptr);

      /// Child is if-cluster, case 2, where have to find either branch at least one of them to clear that if cluster and move onto a statment after that
      bool valid_unmod_procedure_7_16 = Cluster::TraverseScopedCluster(PKBRelRefs::kAffects, inner_cluster_7_16,
                                                                       std::make_pair(7, 16),
                                                                       pkb,
                                                                       "fileName");
      REQUIRE(valid_unmod_procedure_7_16);
    }
  }

  SECTION("Nested while") {
    PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/nested_while.txt");
    Entity* proc_entity = pkb->GetDesignEntities(DesignEntity::kProcedure).front();
    auto* procedure = dynamic_cast<Procedure*>(proc_entity);
    auto* root_cluster = const_cast<Cluster*>(procedure->GetClusterRoot());
    Cluster* w1_cluster = root_cluster->GetNestedClusters().front();
    Cluster* w1_cond_cluster = w1_cluster->GetNestedClusters().front();
    auto* w1_block =dynamic_cast<Block*>(w1_cond_cluster);
    REQUIRE(w1_block->isWhile);
    Cluster* w2_cluster = w1_cond_cluster->GetNextSiblingCluster()->GetNestedClusters().front();
    Cluster* w2_cond_cluster = w2_cluster->GetNestedClusters().front();
    auto* w2_block =dynamic_cast<Block*>(w2_cond_cluster);
    REQUIRE(w2_block->isWhile);
    Cluster* w3_cluster = w2_cond_cluster->GetNextSiblingCluster()->GetNestedClusters().front();
    Cluster* w3_cond_cluster = w3_cluster->GetNestedClusters().front();
    auto* w3_block =dynamic_cast<Block*>(w3_cond_cluster);
    REQUIRE(w3_block->isWhile);
  }
}

