#include "catch.hpp"
#include "component/QueryProcessor/QueryExtractor/QueryExtractor.h"
#include "component/QueryProcessor/QueryExtractor/QueryOptimizer.h"
#include "component/QueryProcessor/types/Types.h"
#include <sstream>

std::string GetMemoryAddress(Group* g) {
  const void * address = static_cast<const void*>(g);
  std::stringstream ss;
  ss << address;
  return ss.str();
}

TEST_CASE("3.QueryOptimizer.All non-boolean groups") {
  std::string query = "stmt s1; assign a1; if if1, if2; variable v1; procedure pr1, pr2; prog_line pl;"
                      "Select <pr1, if2> such that Uses(a1, v1) and Calls(pr1, pr2) and Modifies(pr1, v1)"
                      "pattern a1(v1, _\"i\"_) pattern if1(v1, _, _) and a1(_, _) and if2(_,_,_)";
  auto query_extractor1 = QueryExtractor(& query);
  query_extractor1.ExtractQuery(true);
  // 2 groups, both are non-boolean since they contain target synonym.
  std::vector<Group*> groups = query_extractor1.GetGroupsList();
  REQUIRE(groups.size() == 2);
  // QueryOptimizer should not modify the group order of non-boolean groups (does nothing for this groups list)
  std::vector<std::string> memory_addresses;
  for (Group* g : groups) {
    std::string mem_addr = GetMemoryAddress(g);
    memory_addresses.push_back(mem_addr);
  }
  for (int i = 0; i < groups.size(); i++) {
    REQUIRE(GetMemoryAddress(groups[i]) == memory_addresses[i]);
  }
}

TEST_CASE("3.QueryOptimizer.All boolean groups of different sizes; reordering occurs") {
  std::string query = "stmt s1; assign a1; if if1; variable v1; procedure pr1, pr2; prog_line pl;"
                      "Select BOOLEAN such that Modifies(pr1, v1) pattern if1(v1, _, _) such that Uses(a1, v1)"
                      "pattern a1(_, _) such that Calls(pr1, pr2) pattern if1(_,_,_) such that Next(2, 3)";
  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery(true);
  // 2 groups, both are boolean since they don't contain target synonym.
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  REQUIRE(groups.size() == 2);
  REQUIRE(groups[0]->GetGroupSize() < groups[1]->GetGroupSize());
}

TEST_CASE("3.QueryOptimizer.Some boolean and some non-boolean groups; boolean groups should come first") {
  std::string query = "stmt s1; assign a1; if if1; variable v1; procedure pr1, pr2, pr3, pr4; prog_line pl;"
                      "Select <pr1, pr3> such that Uses(a1, v1) and Calls(pr1, pr2) and Next (2, 3) and Modifies(pr1, v1)"
                      "pattern a1(v1, _\"i\"_) such that Calls(pr3, _) pattern if1(v1, _, _) and a1(_, _)"
                      "and if1(_,_,_) such that Calls(pr4, _)";
  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery(true);
  // 4 groups, 2 are boolean (calls pr3,_ & next 2,3) and 2 are non-boolean (contain tgt syn)
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  REQUIRE(groups.size() == 4);
  // QueryOptimizer should ensure that boolean groups come before non-boolean groups.
  int bool_count = 0;
  for (int i = 0; i < groups.size(); i++) {
    if (!groups[i]->ContainsTargetSynonym()) {
      bool_count++;
      continue;
    }
    REQUIRE(bool_count == 2);
  }
}
