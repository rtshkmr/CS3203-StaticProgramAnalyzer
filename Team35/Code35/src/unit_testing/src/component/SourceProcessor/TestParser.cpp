#include "catch.hpp"
#include "component/SourceProcessor/Parser.h"

bool IsCorrectRange(std::pair<int, int> range, int start, int end) {
  return range.first == start && range.second == end;
}

TEST_CASE("1.Parser.Cluster") {
  SECTION("test source 7") {
    par::Parser p = par::Parser();
    p.Parse("./../../../../Tests35/test_source_7.txt");
    Deliverable* d = p.GetDeliverables();
    Procedure* p1 = d->proc_list_.front();
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

    std::list<Cluster*> if2_clusters = if2->GetNestedClusters();
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

    std::list<Cluster*> if3_clusters = if3->GetNestedClusters();
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
    Procedure* p1 = d->proc_list_.front();

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

    std::list<Cluster*> if3_clusters = if3->GetNestedClusters();
    CHECK(if3_clusters.size() == 4);
    Cluster* if3_cond = if3_clusters.front();
    CHECK(IsCorrectRange(if3_cond->GetStartEndRange(), 3, 3));

    auto iter2_start = if3_clusters.begin();
    std::advance(iter2_start, 1);
    Cluster* if3_body_1 = * iter2_start;
    CHECK(IsCorrectRange(if3_body_1->GetStartEndRange(), 4, 5));

    std::advance(iter2_start, 1);
    Cluster* while6_cluster = * iter2_start;
    CHECK(IsCorrectRange(while6_cluster->GetStartEndRange(), 6, 17));
    int while_6_nested_blocks_size = while6_cluster->GetNestedClusters().size();
    CHECK(while_6_nested_blocks_size == 3);
    Cluster* if8_cluster = while6_cluster->GetNestedClusters().back();
    int if8_cluster_nested_blocks_size = if8_cluster->GetNestedClusters().size();
    CHECK(IsCorrectRange(if8_cluster->GetStartEndRange(), 8, 17));
    CHECK(if8_cluster_nested_blocks_size == 4);
    auto if8_nested_iter = if8_cluster->GetNestedClusters().begin();
//    std::advance(if8_nested_iter, 3);
//    Cluster* if11_cluster = * if8_nested_iter;
//    CHECK(IsCorrectRange(if11_cluster->GetStartEndRange(), 11, 15));
//    CHECK(if11_cluster->GetNestedClusters().size() == 3);
//    std::advance(if8_nested_iter, 1);
    // todo: complete test case, there's likely a bug because 16 doesn't exist;
  }
}

TEST_CASE("1.Parser.Blocks") {
  SECTION("test source 7") {
    par::Parser p = par::Parser();
    p.Parse("./../../../../Tests35/test_source_7.txt");
    Deliverable* d = p.GetDeliverables();
    Procedure* p1 = d->proc_list_.front();
    Block* root_block = const_cast<Block*>(p1->GetBlockRoot());
    std::pair<int, int> range = root_block->GetStartEndRange();
    CHECK(range.first == 1);
    CHECK(range.second == 1);

    std::set<Block*> if1_blocks = root_block->next_blocks_;
    CHECK(if1_blocks.size() == 2);

    for (Block* block: if1_blocks) {
      std::set<Block*> if_blocks = block->next_blocks_;
      CHECK(if_blocks.size() == 2);
    }
  }
}
