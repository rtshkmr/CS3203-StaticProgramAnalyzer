#include "catch.hpp"
#include <component/SourceProcessor/SourceProcessor.h>
#include <component/PKB/DBManager.h>
#include "../../../utils/TestUtils.h"


TEST_CASE("2.PKB.AffectsT.adv_spa_lecture") {
  /**
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
   * Transitive:
   * (1,11),
   * (2,11), (2,12),
   * (6,11), (6,12),
   * (8,11),
   * (9,11), (9,12)
   */

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
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "8", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "11"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9", "12"));
  }

  SECTION ("2.1 - Has AffectedByT, reverse of section 1.1") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "6", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "8", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "10", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "6", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "4", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "8", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "10", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "12", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "10", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "10", "9"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "3", "2"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "6", "9"));
  }

  SECTION ("1.2 - Has AffectedByT, reverse of section 1.2") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "12", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "12", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "12", "9"));
  }
}
