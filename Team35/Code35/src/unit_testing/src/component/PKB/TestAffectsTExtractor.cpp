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
   * (4,11),
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
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectsT, "4", "11"));
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
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedByT, "11", "4"));
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

  SECTION ("7 - Get AffectsT : (#, a1) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "1").size() == 5); // {4,8,10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "2").size() == 4); // {6,10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "3").empty()); //[not assign stmt] while cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "4").size() == 5); // {4,8,10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "5").empty()); //[not assign stmt] call stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "6").size() == 4); // {6,10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "7").empty()); //[not assign stmt] if cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "8").size() == 3); // {10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "9").size() == 3); // {10,11,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "10").size() == 2); // {11, 12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "11").size() == 1); // {12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "12").empty()); //last stmt, not affecting

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "13").size() == 1); // {14}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "14").empty()); // no assign variable uses it. print uses in it Line 15
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "15").empty()); //[not assign stmt] print stmt

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "16").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "17").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectsT, "18").empty()); //[not assign stmt] call stmt
  }

  SECTION ("8 - Get AffectedByT : (a2, #) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "1").empty()); // first x stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "2").empty()); // first i stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "3").empty()); //[not assign stmt] while cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "4").size() == 2); //{1,4}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "5").empty()); //[not assign stmt] call stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "6").size() == 2); // {2,6}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "7").empty()); //[not assign stmt] if cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "8").size() == 2); // {4,1}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "9").empty()); // first z stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "10").size() == 6); // {9,8,4,1,6,2}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "11").size() == 7); //{1,2,4,6,8,9,10}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "12").size() == 8); // {2,6,9,8,4,1,11,10}

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "13").empty()); // first z stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "14").size() == 1); //Affects(13,14) is true
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "15").empty()); //[not assign stmt] print stmt

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "16").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "17").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedByT, "18").empty()); //[not assign stmt] call stmt
  }

  SECTION ("9 - Get AffectsT : (a1, _) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectsT, DesignEntity::kAssign).size() == 9);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectsT, DesignEntity::kStmt).size() == 9);
  }

  SECTION ("10 - Get AffectedByT : (_, a2) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedByT, DesignEntity::kAssign).size() == 7);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedByT, DesignEntity::kStmt).size() == 7);
  }


}
