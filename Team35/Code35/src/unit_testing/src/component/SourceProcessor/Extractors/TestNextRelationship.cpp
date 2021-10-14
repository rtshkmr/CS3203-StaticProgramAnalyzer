#include "catch.hpp"
#include "component/SourceProcessor/Extractors/NextExtractor.h"
#include "../../../../utils/EntityUtils.h"

using namespace entity_utils;

//TEST_CASE("1.NextExtractor.basic conditions") {
//  Deliverable deliverable;
//
//  SECTION("1 statement") {
//    Procedure* proc1 = GetProc1();
//    Statement* s1 = CreateStatement(GetAssign1(), 1);
//    proc1->AddStatement(s1);
//    proc1->SetBlockRoot();
//    deliverable.AddStatement(s1);
//    NextExtractor next_extractor{};
//    next_extractor.Extract(&deliverable);
//    CHECK(deliverable.next_hash_.count(s1) == 0);
//  }
//
//  SECTION("single level statement list") {}
//  SECTION("1 if") {}
//  SECTION("1 while with next") {}
//  SECTION("1 while without next") {}
//  SECTION("multiple procedures") {}
//}
//
//TEST_CASE("1.NextExtractor.nested containers") {
//  SECTION("if x3") {}
//  SECTION("while x3") {}
//  SECTION("if and while") {}
//  SECTION("multiple nesting of if and while") {}
//}
