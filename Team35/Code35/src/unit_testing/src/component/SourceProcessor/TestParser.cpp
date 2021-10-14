#include "catch.hpp"
#include "component/SourceProcessor/Parser.h"

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
    Cluster* if2 = *iter2_start;
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
    Cluster* if2_body = *iter3_start;
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
    Cluster* if3_body = *iter4_start;
    std::pair<int, int> start_end6 = if3_body->GetStartEndRange();
    CHECK(start_end6.first == 6);
    CHECK(start_end6.second == 6);
    Cluster* if3_else = if3_clusters.back();
    std::pair<int, int> start_end7 = if3_else->GetStartEndRange();
    CHECK(start_end7.first == 7);
    CHECK(start_end7.second == 7);
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
