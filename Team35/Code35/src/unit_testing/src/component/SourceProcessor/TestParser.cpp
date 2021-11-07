#include "catch.hpp"
#include "component/SourceProcessor/Parser.h"
#include "model/CFG.h"

bool IsCorrectRange(std::pair<int, int> range, int start, int end) {
  return range.first == start && range.second == end;
}

TEST_CASE("1.Parser.Cluster") {
  SECTION("test source 7") {
    par::Parser p = par::Parser();
    p.Parse("./../../../../Tests35/simple_lib/basic/test_7.txt");
    Deliverable* d = p.GetDeliverables();
    Procedure* p1 = d->GetProcList()->front();
    Cluster* root_cluster = const_cast<Cluster*>(p1->GetClusterRoot());
    std::pair<int, int> range = root_cluster->GetStartEndRange();
    CHECK(range.first == 1);
    CHECK(range.second == 7);

    std::list<Cluster*> root_clusters = root_cluster->GetNestedClusters();
    Cluster* if1 = root_clusters.front();
    std::pair<int, int> range1 = if1->GetStartEndRange();
    CHECK(range1.first == 1);
    CHECK(range1.second == 7);

    std::list<Cluster*> if1_clusters = if1->GetNestedClusters();
    auto iter2_start = if1_clusters.begin();
    std::advance(iter2_start, 1);
    Cluster* if2 = * iter2_start;
    std::pair<int, int> range2 = if2->GetStartEndRange();
    CHECK(range2.first == 2);
    CHECK(range2.second == 4);
    Cluster* if3 = if1_clusters.back();
    std::pair<int, int> range3 = if3->GetStartEndRange();
    CHECK(range3.first == 5);
    CHECK(range3.second == 7);

    std::list<Cluster*> if2_clusters = if2->GetNestedClusters().front()->GetNestedClusters();
    auto iter3_start = if2_clusters.begin();
    std::advance(iter3_start, 1);
    Cluster* if2_body = * iter3_start;
    std::pair<int, int> start_end3 = if2_body->GetStartEndRange();
    CHECK(start_end3.first == 3);
    CHECK(start_end3.second == 3);
    Cluster* if2_else = if2_clusters.back();
    std::pair<int, int> start_end4 = if2_else->GetStartEndRange();
    CHECK(start_end4.first == 4);
    CHECK(start_end4.second == 4);

    std::list<Cluster*> if3_clusters = if3->GetNestedClusters().front()->GetNestedClusters();
    auto iter4_start = if3_clusters.begin();
    std::advance(iter4_start, 1);
    Cluster* if3_body = * iter4_start;
    std::pair<int, int> start_end6 = if3_body->GetStartEndRange();
    CHECK(start_end6.first == 6);
    CHECK(start_end6.second == 6);
    Cluster* if3_else = if3_clusters.back();
    std::pair<int, int> start_end7 = if3_else->GetStartEndRange();
    CHECK(start_end7.first == 7);
    CHECK(start_end7.second == 7);
  }

  SECTION("mixed loops") {
    par::Parser p = par::Parser();
    p.Parse("./../../../tests/integration_test_files/mixed_loops_source.txt");
    Deliverable* d = p.GetDeliverables();
    Procedure* p1 = d->GetProcList()->front();

    Cluster* root_cluster = const_cast<Cluster*>(p1->GetClusterRoot());
    CHECK(IsCorrectRange(root_cluster->GetStartEndRange(), 1, 23));

    std::list<Cluster*> root_clusters = root_cluster->GetNestedClusters();
    CHECK(root_clusters.size() == 4);
    Cluster* c1 = root_clusters.front();
    CHECK(IsCorrectRange(c1->GetStartEndRange(), 1, 2));

    auto iter1_start = root_clusters.begin();
    std::advance(iter1_start, 1);
    Cluster* if3 = * iter1_start;
    CHECK(IsCorrectRange(if3->GetStartEndRange(), 3, 18));

    std::advance(iter1_start, 1);
    Cluster* if19 = * iter1_start;
    int if19_nested_cluster_size = if19->GetNestedClusters().size();
    CHECK(IsCorrectRange(if19->GetStartEndRange(), 19, 21));
    CHECK(if19_nested_cluster_size == 3);

    std::advance(iter1_start, 1);
    Cluster* c22 = * iter1_start;
    CHECK(IsCorrectRange(c22->GetStartEndRange(), 22, 23));

    // if3
    std::list<Cluster*> if3_clusters = if3->GetNestedClusters();
    CHECK(if3_clusters.size() == 3);
    Cluster* if3_cond = if3_clusters.front();
    CHECK(IsCorrectRange(if3_cond->GetStartEndRange(), 3, 3));

    auto iter2_start = if3_clusters.begin();
    std::advance(iter2_start, 1);
    Cluster* c4 = * iter2_start;
    CHECK(IsCorrectRange(c4->GetStartEndRange(), 4, 17));
    CHECK(IsCorrectRange(c4->GetNestedClusters().front()->GetStartEndRange(), 4, 5));
    Cluster* w6 = c4->GetNestedClusters().back();
    CHECK(IsCorrectRange(w6->GetStartEndRange(), 6, 17));

    std::advance(iter2_start, 1);
    Cluster* if3_else = *iter2_start;
    CHECK(IsCorrectRange(if3_else->GetStartEndRange(), 18, 18));

    // while 6
    std::list<Cluster*> w6_clusters = w6->GetNestedClusters();
    CHECK(w6_clusters.size() == 2);
    Cluster* w6_cond = w6_clusters.front();
    CHECK(IsCorrectRange(w6_cond->GetStartEndRange(), 6, 6));

    std::list<Cluster*> w6_body_cluster = w6_clusters.back()->GetNestedClusters();
    CHECK(w6_body_cluster.size() == 2);
    auto iter3_start = w6_body_cluster.begin();
    Cluster* c7 = * iter3_start;
    CHECK(IsCorrectRange(c7->GetStartEndRange(), 7, 7));

    std::advance(iter3_start, 1);
    Cluster* if8 = *iter3_start;
    CHECK(IsCorrectRange(if8->GetStartEndRange(), 8, 17));

    // if8
    std::list<Cluster*> if8_clusters = if8->GetNestedClusters();
    CHECK(if8_clusters.size() == 3);
    Cluster* if8_cond = if8_clusters.front();
    CHECK(IsCorrectRange(if8_cond->GetStartEndRange(), 8, 8));

    auto iter4_start = if8_clusters.begin();
    std::advance(iter4_start, 1);
    Cluster* c9 = * iter4_start;
    CHECK(IsCorrectRange(c9->GetStartEndRange(), 9, 16));

    std::list<Cluster*> c9_clusters = c9->GetNestedClusters();
    auto iter4_1_start = c9_clusters.begin();
    CHECK(c9_clusters.size() == 3);
    Cluster* c9_1 = * iter4_1_start;
    CHECK(IsCorrectRange(c9_1->GetStartEndRange(), 9, 10));

    std::advance(iter4_1_start, 1);
    Cluster* if11 = *iter4_1_start;
    CHECK(IsCorrectRange(if11->GetStartEndRange(), 11, 15));

    std::advance(iter4_1_start, 1);
    Cluster* c16 = *iter4_1_start;
    CHECK(IsCorrectRange(c16->GetStartEndRange(), 16, 16));

    std::advance(iter4_start, 1);
    Cluster* if8_else = *iter4_start;
    CHECK(IsCorrectRange(if8_else->GetStartEndRange(), 17, 17));

    // if11
    std::list<Cluster*> if11_clusters = if11->GetNestedClusters();
    CHECK(if11_clusters.size() == 3);
    Cluster* if11_cond = if11_clusters.front();
    CHECK(IsCorrectRange(if11_cond->GetStartEndRange(), 11, 11));

    auto iter5_start = if11_clusters.begin();
    std::advance(iter5_start, 1);
    Cluster* c12 = * iter5_start;
    CHECK(IsCorrectRange(c12->GetStartEndRange(), 12, 14));

    std::advance(iter5_start, 1);
    Cluster* c15 = *iter5_start;
    CHECK(IsCorrectRange(c15->GetStartEndRange(), 15, 15));
  }
}

TEST_CASE("1.Parser.Blocks") {
  SECTION("test source 7") {
    par::Parser p = par::Parser();
    p.Parse("./../../../../Tests35/simple_lib/basic/test_7.txt");
    Deliverable* d = p.GetDeliverables();
    Procedure* p1 = d->GetProcList()->front();
    Block* root_block = const_cast<Block*>(p1->GetBlockRoot());
    std::pair<int, int> range = root_block->GetStartEndRange();
    CHECK(range.first == 1);
    CHECK(range.second == 1);

    std::set<Block*> if1_blocks = root_block->GetNextBlocks();
    CHECK(if1_blocks.size() == 2);

    for (Block* block: if1_blocks) {
      std::set<Block*> if_blocks = block->GetNextBlocks();
      CHECK(if_blocks.size() == 2);
    }
  }
}
