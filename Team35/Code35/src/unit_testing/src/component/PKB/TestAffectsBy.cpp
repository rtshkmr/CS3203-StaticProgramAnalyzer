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

TEST_CASE("2.PKB.Affects.Negative Testing") {

  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/adv_spa_lecture_source.txt");
  DBManager* dbm = new DBManager(pkb);

  SECTION ("0 - Negative Testing") {
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "-1", "0"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "0", "100"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "99", "100"));

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "-1", "0"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "0", "100"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "99", "100"));

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "-1"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "0"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "100"));

    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "-1"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "0"));
    REQUIRE_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "100"));

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "-1").empty());
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "0").empty());
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffects, "100").empty());

    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "-1").empty());
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "0").empty());
    REQUIRE(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "100").empty());
  }
}

TEST_CASE("2.PKB.Affects.adv_spa_lecture") {
  
   /// region Affects
   /// (1,4), (1,8), (1,10), (1,12),
   /// (2,6), (2,10),
   /// (4,4), (4,8), (4,10), (4,12),
   /// (6,6), (6,10),
   /// (8,10), (8,12),
   /// (9,10),
   /// (10,11), (10,12)
   /// (11,12),
   /// (13,14)
   ///endregion
   
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/adv_spa_lecture_source.txt");
  DBManager* dbm = new DBManager(pkb);

  SECTION ("1 - Has Affects : (#,#)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "6", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "10"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "11"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "12"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "2", "3"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "6"));

    //TODO: Lack of cross-procedure tests
  }

  SECTION ("2 - Has AffectedBy, reverse of section 1") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "6"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "2"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "9"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "11", "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "3", "2"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6", "9"));
  }
  
  SECTION ("3 - Has Affects : (#,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "2"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "3")); //[not assign stmt] while cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "4"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "5")); //[not assign stmt] call stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "6"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "7")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "8"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "10"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "11"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "12")); //last stmt, not affecting

    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "13"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "14")); // no assign variable uses it. print uses in it Line 15
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "15")); //[not assign stmt] print stmt

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "16")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "17")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "18")); //[not assign stmt] call stmt
  }

  SECTION ("4 - Has AffectedBy : (_,#)") {
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "1")); // first x stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "2"));  // first i stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "3")); //[not assign stmt] while cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4")); //Affects(1,4), Affects(4,4) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "5")); //[not assign stmt] call stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6")); //Affects(6,6), Affects(2,6) is true.
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "7")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8")); //Affects(1,8), Affects(4,8) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9")); // first z stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10")); //Affects(9,10), Affects(8,10), Affects(2,10), Affects(6,10) is true
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "11")); //Affects(10,11) is true
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12")); //Affects(8,12), Affects(4,12), Affects(1,12), Affects(11,12), Affects(10, 12) is true

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "13")); // first z stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "14")); //Affects(13,14) is true
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "15")); //[not assign stmt] print stmt

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "16")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "17")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "18")); //[not assign stmt] call stmt
  }
  
  SECTION ("5 - Has Affects : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects));
  }
  
  SECTION ("6 - Has AffectedBy : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy));
  }
  
  SECTION ("7 - Get Affects : (#, a1) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "1").size() == 4); // {4,8,10,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "2").size() == 2); // {6,10}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "3").empty()); //[not assign stmt] while cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "4").size() == 4); // {4,8,10,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "5").empty()); //[not assign stmt] call stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "6").size() == 2); // {6,10}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "7").empty()); //[not assign stmt] if cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "8").size() == 2); // {10,12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "9").size() == 1); // {10}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "10").size() == 2); // {11, 12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "11").size() == 1); // {12}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "12").empty()); //last stmt, not affecting

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "13").size() == 1); // {14}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "14").empty()); // no assign variable uses it. print uses in it Line 15
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "15").empty()); //[not assign stmt] print stmt

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "16").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "17").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "18").empty()); //[not assign stmt] call stmt
  }
  
  SECTION ("8 - Get AffectedBy : (a2, #) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "1").empty()); // first x stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "2").empty()); // first i stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "3").empty()); //[not assign stmt] while cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "4").size() == 2); //{1,4}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5").empty()); //[not assign stmt] call stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "6").size() == 2); // {2,6}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "7").empty()); //[not assign stmt] if cond
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "8").size() == 2); // {4,1}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "9").empty()); // first z stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "10").size() == 6); // {9,8,4,1,6,2}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "11").size() == 1); //{10}
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "12").size() == 5); // {8,4,1,11,10}

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "13").empty()); // first z stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "14").size() == 1); //Affects(13,14) is true
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "15").empty()); //[not assign stmt] print stmt

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "16").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "17").empty()); //[not assign stmt] read stmt
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "18").empty()); //[not assign stmt] call stmt
  }
  
  SECTION ("9 - Get Affects : (a1, a2) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kAssign).size() == 19);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kStmt).size() == 19);
  }
  
  SECTION ("10 - Get AffectedBy : (a1, a2) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kAssign).size() == 19);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kStmt).size() == 19);
  }

  SECTION ("11 - Get Affects / AffectedBy GetRelationshipByTypes") {
    // TODO
  }
}

TEST_CASE("2.PKB.Affects.Non_BIP_Tester") {
// The classic test given in lecture where Modifies is abstracted out into another procedure
PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/affects_source.txt");
DBManager* dbm = new DBManager(pkb);

CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "5"));
CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "6", "8"));
}

TEST_CASE("2.PKB.Affects.mixed_loops_source") {
  PKB* pkb = sp::SourceProcessor::ProcessSourceFile("./../../../tests/integration_test_files/mixed_loops_source.txt");
  DBManager* dbm = new DBManager(pkb);

  /// region Affects
  /// (1, 12)
  /// (4, 9)
  /// (9, 9)
  /// (12, 12)
  /// (14, 9)
  /// (16, 16)
  /// endregion

  SECTION ("1 - Has Affects : (#,#)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "12", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "14", "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "16", "16"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "1"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "1", "23"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "23", "1"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "4", "14"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "14", "4"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "9", "14"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "3", "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "3", "10"));
  }

  SECTION ("2 - Has AffectedBy, reverse of section 1") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "1"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9", "4"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9", "9"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12", "12"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9", "14"));
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "16", "16"));

    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "1", "1"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "1", "23"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "23", "1"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4", "14"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "14", "4"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "14", "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9", "3"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10", "3"));
  }

  SECTION ("3 - Has Affects : (#,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "1"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "2")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "3")); //[not assign stmt] while cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "4"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "5")); // byte overwritten by read.
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "6")); //[not assign stmt] while cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "7")); //[not assign stmt] read cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "8")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "10")); //lastByte declared but not used.
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "11")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "12"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "13")); //[not assign stmt] print stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "14"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "15")); //[not assign stmt] print stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects, "16"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "17")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "18")); //[not assign stmt] call stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "19")); //[not assign stmt] if cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "20")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "21")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "22")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffects, "23")); //last stmt
  }

  SECTION ("4 - Has AffectedBy : (_,#)") {
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "1")); // first psubsystem var
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "2"));  //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "3")); //[not assign stmt] if cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "4")); //first chara var
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "5")); //first byte var
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "6")); //[not assign stmt] while cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "7")); //[not assign stmt] read stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "8")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "9"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "10")); //byte was overwritten by read
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "11")); //[not assign stmt] if cond
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "12"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "13")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "14")); //redeclared chara
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "15")); //[not assign stmt] print stmt
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "16"));
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "17")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "18")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "19")); //[not assign stmt] if cond
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "20")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "21")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "22")); //[not assign stmt] print stmt
    CHECK_FALSE(dbm->HasRelationship(PKBRelRefs::kAffectedBy, "23")); //redeclared psubsystem
  }

  SECTION ("5 - Has Affects : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffects));
  }

  SECTION ("6 - Has AffectedBy : (_,_)") {
    CHECK(dbm->HasRelationship(PKBRelRefs::kAffectedBy));
  }

  SECTION ("7 - Get Affects : (#, a1) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "1").size() == 1);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "4").size() == 1);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "9").size() == 1);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "12").size() == 1);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "14").size() == 1);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "16").size() == 1);

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "2").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "3").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "5").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "6").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "7").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "8").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "10").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "11").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "13").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "15").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "17").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "18").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "19").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "20").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "21").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "22").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffects, "23").empty());

  }

  SECTION ("8 - Get AffectedBy : (a2, #) ") {
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "9").size() == 3);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "12").size() == 2);
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "16").size() == 1);

    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "1").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "2").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "3").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "4").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "5").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "6").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "7").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "8").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "10").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "11").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "13").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "14").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "15").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "17").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "18").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "19").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "20").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "21").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "22").empty());
    CHECK(dbm->GetRelationship(PKBRelRefs::kAffectedBy, "23").empty());
  }

  SECTION ("9 - Get Affects : (a1, a2) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kAssign).size() == 6);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffects, DesignEntity::kStmt).size() == 6);
  }

  SECTION ("10 - Get AffectedBy : (a1, a2) ") {
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kAssign).size() == 6);
    CHECK(dbm->GetFirstEntityOfRelationship(PKBRelRefs::kAffectedBy, DesignEntity::kStmt).size() == 6);
  }

  SECTION ("11 - Get Affects / AffectedBy GetRelationshipByTypes") {
    // TODO
  }
}
