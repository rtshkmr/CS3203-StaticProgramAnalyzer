#include "catch.hpp"
#include <component/PKB/extractor/NextTExtractor.h>
#include <component/SourceProcessor/SourceProcessor.h>
#include <component/PKB/DBManager.h>
#include "../../../utils/EntityUtils.h"
#include "../../../utils/TestUtils.h"

// todo: rename this file into just TestAffectsExtractor (idw merge conflict first)
//TEST_CASE("2.PKB.AffectsBy") {
//  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
//  DBManager* dbm = new DBManager(pkb);
//  std::vector<Entity*> rs = dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5");
//  REQUIRE(rs.size() == 1);
//  REQUIRE(dynamic_cast<AssignEntity*>(rs[0])->GetStatementNumber()->GetNum() == 3);
//}

TEST_CASE("2.PKB.Affects") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  DBManager* dbm = new DBManager(pkb);
  AffectsExtractor* affects_extractor = new AffectsExtractor();
  affects_extractor->SetPKB(pkb);

  SECTION("HasAffects(#,#)") {
    std::vector<Entity*> rs_1 = pkb->GetPatternEntities(DesignEntity::kAssign, "3");
    AssignEntity* first = dynamic_cast<AssignEntity*>(rs_1[0]);
    std::vector<Entity*> rs_2 = pkb->GetPatternEntities(DesignEntity::kAssign, "5");
    AssignEntity* second = dynamic_cast<AssignEntity*>(rs_2[0]);
    bool has_affects_3_5 = affects_extractor->HasAffects(first, second);
    REQUIRE(rs_2.size() == 1);
    REQUIRE(rs_1.size() == 1);
    REQUIRE(has_affects_3_5);
  }
}
