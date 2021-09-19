#include "catch.hpp"
#include "datatype/DataType.h"

TEST_CASE("1.DataType.ValidateName") {
  REQUIRE(ValidateName("aaa"));
  REQUIRE(ValidateName("KKKK"));
  REQUIRE(ValidateName("NuSSu"));
  REQUIRE(ValidateName("Nu99ets"));
  REQUIRE(ValidateName("s9p"));
  REQUIRE(ValidateName("Q"));
  REQUIRE(ValidateName("r00Tb33rISn0tBEER"));
  REQUIRE(ValidateName("asrhaerbadfhnnmsryjtdnhbsjzdgmzdgmnazedtgmzgdntmzdtjzedgmzfdhaewrbh"));

  REQUIRE_FALSE(ValidateName(""));
  REQUIRE_FALSE(ValidateName("    "));
  REQUIRE_FALSE(ValidateName("(wow"));
  REQUIRE_FALSE(ValidateName("Noo$e"));
  REQUIRE_FALSE(ValidateName("999"));
  REQUIRE_FALSE(ValidateName("9fish"));
  REQUIRE_FALSE(ValidateName("r00Tb33rISn0tBEER_"));
}

TEST_CASE("1.DataType.StatementNumber") {
  SECTION("Creation and Getters") {
    StatementNumber sn = StatementNumber(39);
    StatementNumber sn2(10);

    REQUIRE(sn.GetNum() == 39);
    REQUIRE(sn2.GetNum() == 10);
    REQUIRE_THROWS_AS(StatementNumber(0), std::invalid_argument);
    REQUIRE_THROWS_AS(StatementNumber(-28), std::invalid_argument);
  }

  SECTION("Operators") {
    StatementNumber sn = StatementNumber(50);
    StatementNumber sn2 = StatementNumber(50);
    StatementNumber sn3 = StatementNumber(51);

    ///operator=
    REQUIRE(sn == sn2);
    REQUIRE(sn2 == sn);
    REQUIRE(!(sn == sn3));
    REQUIRE(!(sn3 == sn2));

    ///operator<
    REQUIRE(!(sn < sn2)); //equality should not hold
    REQUIRE(!(sn2 < sn));
    REQUIRE(sn < sn3);
    REQUIRE(sn2 < sn3);
  }
}

TEST_CASE("1.DataType.LineNumber") {
  SECTION("Creation and Getters") {
    LineNumber ln = LineNumber(39);
    LineNumber ln2(10);

    REQUIRE(ln.getNum() == 39);
    REQUIRE(ln2.getNum() == 10);
    REQUIRE_THROWS_AS(LineNumber(0), std::invalid_argument);
    REQUIRE_THROWS_AS(LineNumber(-28), std::invalid_argument);
  }

  SECTION("Operators") {
    LineNumber ln = LineNumber(50);
    LineNumber ln2 = LineNumber(50);
    LineNumber ln3 = LineNumber(51);

    ///operator=
    REQUIRE(ln == ln2);
    REQUIRE(ln2 == ln);
    REQUIRE(!(ln == ln3));
    REQUIRE(!(ln3 == ln2));

    ///operator<
    REQUIRE(!(ln < ln2)); //equality should not hold
    REQUIRE(!(ln2 < ln));
    REQUIRE(ln < ln3);
    REQUIRE(ln2 < ln3);
  }
}

TEST_CASE("1.DataType.ProcedureName") {

  SECTION("Creation and Getters") {
    ProcedureName pn = ProcedureName("helloW0r1d");
    ProcedureName pn2 = ProcedureName("M");
    ProcedureName pn3 = ProcedureName("Unc13S00");

    REQUIRE(pn.getName() == "helloW0r1d");
    REQUIRE(pn2.getName() == "M");
    REQUIRE(pn3.getName() == "Unc13S00");
    REQUIRE_THROWS_AS(ProcedureName(""), std::invalid_argument); //blank name
    REQUIRE_THROWS_AS(ProcedureName("          "), std::invalid_argument); //blank name with space
    REQUIRE_THROWS_AS(ProcedureName("!@#$"), std::invalid_argument); //name with symbols
    REQUIRE_THROWS_AS(ProcedureName("1234"), std::invalid_argument); //name with numbers only
    REQUIRE_THROWS_AS(ProcedureName("Uncle SO0"), std::invalid_argument); //name with space and numbers
    REQUIRE_THROWS_AS(ProcedureName("Uncle S00 + friends"), std::invalid_argument); //name with space, symbol
    REQUIRE_THROWS_AS(ProcedureName("1pie"), std::invalid_argument); //name starting with numbers
  }

  SECTION("Operators") {
    ProcedureName pn = ProcedureName("WHEREgotTIMEtoDOthis");
    ProcedureName pn2 = ProcedureName("StillGotTODOthis");
    ProcedureName pn3 = ProcedureName("StillGotTODOthis");
    ProcedureName pn4 = ProcedureName("stillGotTODOthis"); //small s in front

    ///operator=
    REQUIRE(pn3 == pn2);
    REQUIRE(pn2 == pn3);
    REQUIRE(!(pn == pn3));
    REQUIRE(!(pn2 == pn));
    REQUIRE(!(pn4 == pn2));

    ///operator<
    REQUIRE(!(pn < pn2));
    REQUIRE(!(pn2 < pn3)); //equality should not hold
    REQUIRE(pn3 < pn);
    REQUIRE(pn2 < pn4);
  }
}

TEST_CASE("1.DataType.VariableName") {
  SECTION("Creation and Getters") {
    VariableName vn = VariableName("helloW0r1d");
    VariableName vn2 = VariableName("M");
    VariableName vn3 = VariableName("Unc13S00");

    REQUIRE(vn.getName() == "helloW0r1d");
    REQUIRE(vn2.getName() == "M");
    REQUIRE(vn3.getName() == "Unc13S00");
    REQUIRE_THROWS_AS(VariableName(""), std::invalid_argument); //blank name
    REQUIRE_THROWS_AS(VariableName("          "), std::invalid_argument); //blank name with space
    REQUIRE_THROWS_AS(VariableName("!@#$"), std::invalid_argument); //name with symbols
    REQUIRE_THROWS_AS(VariableName("1234"), std::invalid_argument); //name with numbers only
    REQUIRE_THROWS_AS(VariableName("Uncle SO0"), std::invalid_argument); //name with space and numbers
    REQUIRE_THROWS_AS(VariableName("Uncle S00 + friends"), std::invalid_argument); //name with space, symbol
    REQUIRE_THROWS_AS(VariableName("1pie"), std::invalid_argument); //name starting with numbers
  }

  SECTION("Operators") {
    VariableName vn = VariableName("WHEREgotTIMEtoDOthis");
    VariableName vn2 = VariableName("StillGotTODOthis");
    VariableName vn3 = VariableName("StillGotTODOthis");
    VariableName vn4 = VariableName("stillGotTODOthis"); //small s in front

    ///operator=
    REQUIRE(vn3 == vn2);
    REQUIRE(vn2 == vn3);
    REQUIRE(!(vn == vn3));
    REQUIRE(!(vn2 == vn));
    REQUIRE(!(vn4 == vn2));

    ///operator<
    REQUIRE(!(vn < vn2));
    REQUIRE(!(vn2 < vn3)); //equality should not hold
    REQUIRE(vn3 < vn);
    REQUIRE(vn2 < vn4);
  }
}

TEST_CASE("1.DataType.ConstantValue") {
  SECTION("Creation and Getters") {
    ConstantValue cv = ConstantValue("39");
    ConstantValue cv2("10000");
    ConstantValue cv3 = ConstantValue("0");
    ConstantValue cv4 = ConstantValue("00000000000");
    ConstantValue cv5 = ConstantValue("-12345678");

    REQUIRE(cv.get() == 39);
    REQUIRE(cv2.get() == 10000);
    REQUIRE(cv3.get() == 0);
    REQUIRE(cv4.get() == 0);
    REQUIRE(cv5.get() == -12345678);

    REQUIRE_THROWS_AS(ConstantValue("999999999999999999999999999999999999999"), std::out_of_range);
    REQUIRE_THROWS_AS(ConstantValue("9hello"), std::invalid_argument);
    REQUIRE_THROWS_AS(ConstantValue("he11o"), std::invalid_argument);
    REQUIRE_THROWS_AS(ConstantValue("hell0"), std::invalid_argument);
  }

  SECTION("Operators") {
    ConstantValue cv = ConstantValue("0");
    ConstantValue cv2 = ConstantValue("00000000000");
    ConstantValue cv3 = ConstantValue("-12345678");

    ///operator=
    REQUIRE(cv == cv2);
    REQUIRE(cv2 == cv);
    REQUIRE(!(cv == cv3));
    REQUIRE(!(cv3 == cv2));

    ///operator<
    REQUIRE(!(cv < cv2)); //equality should not hold
    REQUIRE(!(cv2 < cv));
    REQUIRE(cv3 < cv);
    REQUIRE(cv3 < cv2);
  }
}

TEST_CASE("1.Datatype.TokenTag") {
  SECTION("Operators") {
    Token token1 = Token("hello", TokenTag::kName);
    Token token1_duplicate = Token("hello", TokenTag::kName);
    Token token2 = Token("procedure", TokenTag::kProcedureKeyword);
    Token token2_different_tags = Token("procedure", TokenTag::kName);
    REQUIRE(token1==token1_duplicate);
    REQUIRE_FALSE(token2==token2_different_tags);
  }
}

