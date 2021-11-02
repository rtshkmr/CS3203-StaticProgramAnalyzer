#include "catch.hpp"
#include <component/PKB/extractor/NextTExtractor.h>
#include <component/SourceProcessor/SourceProcessor.h>
#include <component/PKB/DBManager.h>
#include "../../../utils/EntityUtils.h"
#include "../../../utils/TestUtils.h"

// todo: rename this file into just TestAffectsExtractor (idw merge conflict first)
TEST_CASE("2.PKB.AffectsBy") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  DBManager* dbm = new DBManager(pkb);
  std::vector<Entity*> rs = dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5");
  REQUIRE(rs.size() == 1);
  REQUIRE(dynamic_cast<AssignEntity*>(rs[0])->GetStatementNumber()->GetNum() == 3);
}

TEST_CASE("2.PKB.Affects") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/basic_source.txt");
  DBManager* dbm = new DBManager(pkb);
  AffectsExtractor* affects_extractor = new AffectsExtractor();
  affects_extractor->SetPKB(pkb);

//  SECTION("AffectsBy()") {
//    std::vector<Entity*> rs = dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5");
//    REQUIRE(rs.size() == 1);
//    REQUIRE(dynamic_cast<AssignEntity*>(rs[0])->GetStatementNumber()->GetNum() == 3);
//  }

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

TEST_CASE("2.PKB.Affects.adv_spa_lecture") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/adv_spa_lecture_source.txt");
  DBManager* dbm = new DBManager(pkb);

  SECTION ("1 - Has Affects : (#,#)") {
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "6"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "8"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "10"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "6", "6"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "4"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "8"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "10"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "12"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "10"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "10"));

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "11"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "12"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "3"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "6"));

    //TODO: Lack of cross-procedure tests
  }

  SECTION ("2 - Has AffectedBy, reverse of section 1") {
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "2"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8", "4"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "4"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "6"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4", "1"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8", "1"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "1"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "1"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "2"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "9"));

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "11", "9"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "9"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "3", "2"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "9"));
  }

  /*
  SECTION ("3 - Has Affects : (#,_)") {
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "2"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "3")); //[not assign stmt] while cond
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "4"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "5")); //[not assign stmt] call stmt
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "6"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "7")); //[not assign stmt] if cond
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "8"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "9"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "10"));
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "11"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "12")); //last stmt, not affecting

    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "13"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "14")); // no assign variable uses it. print uses in it Line 15
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "15")); //[not assign stmt] print stmt

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "16")); //[not assign stmt] read stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "17")); //[not assign stmt] read stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "18")); //[not assign stmt] call stmt
  }
  */

  SECTION ("4 - Has AffectedBy : (_,#)") {
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "1")); // first x stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "2"));  // first i stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "3")); //[not assign stmt] while cond
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4")); //Affects(1,4), Affects(4,4) is true
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "5")); //[not assign stmt] call stmt
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6")); //Affects(6,6), Affects(2,6) is true.
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "7")); //[not assign stmt] if cond
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8")); //Affects(1,8), Affects(4,8) is true
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9")); // first z stmt
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10")); //Affects(9,10), Affects(8,10), Affects(2,10), Affects(6,10) is true
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "11")); //Affects(10,11) is true
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12")); //Affects(8,12), Affects(4,12), Affects(1,12), Affects(11,12), Affects(10, 12) is true

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "13")); // first z stmt
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "14")); //Affects(13,14) is true
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "15")); //[not assign stmt] print stmt

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "16")); //[not assign stmt] read stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "17")); //[not assign stmt] read stmt
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "18")); //[not assign stmt] call stmt
  }

  /* Depends on Section 3
  SECTION ("5 - Has Affects : (_,_)") {
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects));
  }
  */

  /* Depends on Section 3
  SECTION ("6 - Has AffectedBy : (_,_)") {
    REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffectedBy));
  }
  */

  /* Depends on Section 3
  SECTION ("7 - Get Affects : (a1, _) ") {
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "1").size() == 4); // {4,8,10,12}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "2").size() == 2); // {6,10}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "3").empty()); //[not assign stmt] while cond
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "4").size() == 4); // {4,8,10,12}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "5").empty()); //[not assign stmt] call stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "6").size() == 2); // {6,10}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "7").empty()); //[not assign stmt] if cond
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "8").size() == 2); // {10,12}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "9").size() == 1); // {10}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "10").size() == 2); // {11, 12}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "11").size() == 1); // {12}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "12").empty()); //last stmt, not affecting

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "13").size() == 1); // {14}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "14").empty()); // no assign variable uses it. print uses in it Line 15
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "15").empty()); //[not assign stmt] print stmt

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "16").empty()); //[not assign stmt] read stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "17").empty()); //[not assign stmt] read stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "18").empty()); //[not assign stmt] call stmt
  }
*/

  /* TODO: Assertion error : scoped_cluster->CheckIfStmtNumInRange(target_range.second)
  SECTION ("8 - Get AffectedBy : (_, a2) ") {
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "1").empty()); // first x stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "2").empty()); // first i stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "3").empty()); //[not assign stmt] while cond
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "4").size() == 2); //{1,4}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5").empty()); //[not assign stmt] call stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "6").size() == 2); // {2,6}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "7").empty()); //[not assign stmt] if cond
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "8").size() == 2); // {4,1}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "9").empty()); // first z stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "10").size() == 6); // {9,8,4,1,6,2}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "11").size() == 1); //{10}
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "12").size() == 5); // {8,4,1,11,10}

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "13").empty()); // first z stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "14").size() == 1); //Affects(13,14) is true
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "15").empty()); //[not assign stmt] print stmt

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "16").empty()); //[not assign stmt] read stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "17").empty()); //[not assign stmt] read stmt
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "18").empty()); //[not assign stmt] call stmt
  }
  */

//  //Depends on Section 3
//  SECTION ("9 - Get Affects : (a1, a2) ") {
//    /**
//     * Extracted from Section 7
//     * Affects:
//     * (1,4), (1,8), (1,10), (1,12),
//     * (2,6), (2,10),
//     * (4,4), (4,8), (4,10), (4,12),
//     * (6,6), (6,10),
//     * (8,10), (8,12),
//     * (9,10),
//     * (10,11), (10,12)
//     * (11,12),
//     * (13,14)
//     */
//
//    REQUIRE(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kAssign).size() == 19);
//    REQUIRE(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kStmt).size() == 19);
//  }

  /* Dependent on Section 8
  SECTION ("10 - Get AffectedBy : (a1, a2) ") {
    /// Refer to Section 9 on where 19 comes from.
    REQUIRE(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kAssign).size() == 19);
    REQUIRE(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kStmt).size() == 19);
  }
  */

  SECTION ("11 - Get Affects / AffectedBy GetRelationshipByTypes") {
    // TODO
  }
}

TEST_CASE("2.PKB.Affects.Non_BIP_Tester") {
// The classic test given in lecture where Modifies is abstracted out into another procedure
PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/affects_source.txt");
DBManager* dbm = new DBManager(pkb);

REQUIRE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "5"));
REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "6", "8"));
}
