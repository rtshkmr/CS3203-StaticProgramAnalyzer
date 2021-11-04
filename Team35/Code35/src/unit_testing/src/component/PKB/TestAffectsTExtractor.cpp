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

  SECTION ("3 - Has AffectsT : (#,_)") {
    // not changed from kAffects test case. 
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "2"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "3")); //[not assign stmt] while cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "4"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "5")); //[not assign stmt] call stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "6"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "7")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "11"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "12")); //last stmt, not affecting

    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "13"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "14")); // no assign variable uses it. print uses in it Line 15
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "15")); //[not assign stmt] print stmt

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "16")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "17")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectsT, "18")); //[not assign stmt] call stmt
  }

  SECTION ("4 - Has AffectedByT : (_,#)") {
    // not changed from kAffects test case.
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "1")); // first x stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "2"));  // first i stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "3")); //[not assign stmt] while cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "4")); //Affects(1,4), Affects(4,4) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "5")); //[not assign stmt] call stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "6")); //Affects(6,6), Affects(2,6) is true.
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "7")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "8")); //Affects(1,8), Affects(4,8) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "9")); // first z stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "10")); //Affects(9,10), Affects(8,10), Affects(2,10), Affects(6,10) is true
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11")); //Affects(10,11) is true
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "12")); //Affects(8,12), Affects(4,12), Affects(1,12), Affects(11,12), Affects(10, 12) is true

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "13")); // first z stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "14")); //Affects(13,14) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "15")); //[not assign stmt] print stmt

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "16")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "17")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "18")); //[not assign stmt] call stmt
  }


  SECTION ("5 - Has AffectsT : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT));
  }

  SECTION ("6 - Has AffectedByT : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT));
  }
}
