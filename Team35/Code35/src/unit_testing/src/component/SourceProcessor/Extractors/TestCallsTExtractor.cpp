#include <exception/SpaException.h>
#include "catch.hpp"
#include "component/SourceProcessor/Extractors/CallsTExtractor.h"
#include "../../../../utils/EntityUtils.h"
#include "../../../../utils/TestUtils.h"

using namespace entity_utils;

TEST_CASE("1.CallsTExtractor.Extract CallsT positive tests") {
  Deliverable deliverable;
  Procedure* proc1 = GetProc1();
  Procedure* proc2 = GetProc2();
  Procedure* proc3 = GetProc3();
  Procedure* proc4 = GetProc4();

  SECTION("No Calls") {
    CallsTExtractor calls_t_extractor{};
    calls_t_extractor.Extract(&deliverable);
    CHECK(deliverable.calls_T_hash_.empty());
  }

  SECTION("No transitive Calls") {
    /*
     * proc1
     *  call2
     *  call4
     * proc2
     * proc3
     *  call4
     *  call2
     * proc4
     */
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc1, proc4);
    deliverable.AddCallsRelationship(proc3, proc2);
    deliverable.AddCallsRelationship(proc3, proc4);

    CallsTExtractor calls_t_extractor{};
    calls_t_extractor.Extract(&deliverable);

    std::list<Procedure*>* actual_proc_list1 = deliverable.calls_T_hash_.find(proc1)->second;
    std::list<Procedure*>* actual_proc_list3 = deliverable.calls_T_hash_.find(proc3)->second;
    std::list<Procedure*> expected_proc_list1 = std::list<Procedure*>{
        proc2, proc4
    };
    std::list<Procedure*> expected_proc_list3 = std::list<Procedure*>{
        proc2, proc4
    };

    CHECK(*actual_proc_list1 == expected_proc_list1);
    CHECK(deliverable.calls_T_hash_.count(proc2) == 0);
    CHECK(*actual_proc_list3 == expected_proc_list3);
    CHECK(deliverable.calls_T_hash_.count(proc4) == 0);

    // reverse
    std::list<Procedure*> expected_proc_list4 = std::list<Procedure*>{
        proc1, proc3
    };
    CHECK(TestUtils::AreListsEqual(*deliverable.called_by_T_hash_.find(proc2)->second, expected_proc_list4));
    CHECK(TestUtils::AreListsEqual(*deliverable.called_by_T_hash_.find(proc4)->second, expected_proc_list4));
  }

  SECTION("Transitive Calls") {
    /*
     * proc1
     *  call2
     *  call4
     *  call2
     * proc2
     *  call3
     *  call3
     *  call3
     * proc3
     *  call4
     * proc4
     */
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc1, proc4);
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc2, proc3);
    deliverable.AddCallsRelationship(proc2, proc3);
    deliverable.AddCallsRelationship(proc2, proc3);
    deliverable.AddCallsRelationship(proc3, proc4);

    CallsTExtractor calls_t_extractor{};
    calls_t_extractor.Extract(&deliverable);

    std::list<Procedure*>* actual_proc_list1 = deliverable.calls_T_hash_.find(proc1)->second;
    std::list<Procedure*>* actual_proc_list2 = deliverable.calls_T_hash_.find(proc2)->second;
    std::list<Procedure*>* actual_proc_list3 = deliverable.calls_T_hash_.find(proc3)->second;
    std::list<Procedure*> expected_proc_list1 = std::list<Procedure*>{
        proc4, proc3, proc2
    };
    std::list<Procedure*> expected_proc_list2 = std::list<Procedure*>{
        proc4, proc3
    };
    std::list<Procedure*> expected_proc_list3 = std::list<Procedure*>{
        proc4
    };

    CHECK(*actual_proc_list1 == expected_proc_list1);
    CHECK(*actual_proc_list2 == expected_proc_list2);
    CHECK(*actual_proc_list3 == expected_proc_list3);
    CHECK(deliverable.calls_T_hash_.count(proc4) == 0);

    // reverse
    std::list<Procedure*> expected_proc_list4 = std::list<Procedure*>{
        proc3, proc2, proc1
    };
    std::list<Procedure*> expected_proc_list5 = std::list<Procedure*>{
        proc2, proc1
    };
    CHECK(deliverable.called_by_T_hash_.count(proc1) == 0);
    CHECK(*deliverable.called_by_T_hash_.find(proc2)->second == std::list<Procedure*>{proc1});
    CHECK(*deliverable.called_by_T_hash_.find(proc3)->second == expected_proc_list5);
    CHECK(*deliverable.called_by_T_hash_.find(proc4)->second == expected_proc_list4);
  }
}

TEST_CASE("1.CallsTExtractor.Extract CallsT negative tests") {
  Deliverable deliverable;
  Procedure* proc1 = GetProc1();
  Procedure* proc2 = GetProc2();
  Procedure* proc3 = GetProc3();
  Procedure* proc4 = GetProc4();
  Procedure* proc5 = GetProc5();

  SECTION("Immediate cyclic Call") {
    /*
     * proc1
     *  call2
     * proc2
     *  call1
     */
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc2, proc1);
    CallsTExtractor calls_t_extractor{};
    CHECK_THROWS_AS(calls_t_extractor.Extract(&deliverable), SemanticException);
  }

  SECTION("Transitive cyclic Call 1") {
    /*
     * proc1
     *  call2
     * proc2
     *  call3
     * proc3
     *  call1
     */
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc2, proc3);
    deliverable.AddCallsRelationship(proc3, proc1);
    CallsTExtractor calls_t_extractor{};
    CHECK_THROWS_AS(calls_t_extractor.Extract(&deliverable), SemanticException);
  }

  SECTION("Transitive cyclic Call 2") {
    /*
     * proc1
     *  call2
     *  call3
     * proc2
     *  call4
     * proc3
     *  call5
     * proc4
     *  call3
     * proc5
     *  call2
     */
    deliverable.AddCallsRelationship(proc1, proc2);
    deliverable.AddCallsRelationship(proc1, proc3);
    deliverable.AddCallsRelationship(proc2, proc4);
    deliverable.AddCallsRelationship(proc3, proc5);
    deliverable.AddCallsRelationship(proc4, proc3);
    deliverable.AddCallsRelationship(proc5, proc2);
    CallsTExtractor calls_t_extractor{};
    CHECK_THROWS_AS(calls_t_extractor.Extract(&deliverable), SemanticException);
  }
}
