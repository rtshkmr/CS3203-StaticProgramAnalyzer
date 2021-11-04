#include "catch.hpp"
#include <component/SourceProcessor/SourceProcessor.h>
#include <component/PKB/DBManager.h>
#include "../../../utils/TestUtils.h"


TEST_CASE("2.PKB.AffectsT.adv_spa_lecture") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/adv_spa_lecture_source.txt");
  DBManager* dbm = new DBManager(pkb);

  SECTION ("1.1 - Has Affects : (#,#)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "4", "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "4", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "10"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "3"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "6"));

    //TODO: Lack of cross-procedure tests
  }

  SECTION ("1.2 - Has AffectsT : (#,#)") {
    /**
     * Extracted from Section 7
     * Affects:
     * (1,4), (1,8), (1,10), (1,12),
     * (2,6), (2,10),
     * (4,4), (4,8), (4,10), (4,12),
     * (6,6), (6,10),
     * (8,10), (8,12),
     * (9,10),
     * (10,11), (10,12)
     * (11,12),
     * (13,14)
     */

    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "8", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "12"));
  }
}
