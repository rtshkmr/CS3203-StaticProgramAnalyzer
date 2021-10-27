#include "catch.hpp"
#include <component/PKB/extractor/NextTExtractor.h>
#include <component/SourceProcessor/SourceProcessor.h>
#include <component/PKB/DBManager.h>
#include "../../../utils/EntityUtils.h"
#include "../../../utils/TestUtils.h"

TEST_CASE("2.PKB.AffectsBy") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  DBManager* dbm = new DBManager(pkb);
  std::vector<Entity*> rs = dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5");
  REQUIRE(rs.size() == 1);
  REQUIRE(dynamic_cast<AssignEntity*>(rs[0])->GetStatementNumber()->GetNum() == 3);
}
