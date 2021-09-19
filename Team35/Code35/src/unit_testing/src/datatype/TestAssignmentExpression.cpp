#include "catch.hpp"
#include "datatype/AssignmentExpression.h"

TEST_CASE("1.DataType.AssignmentExpression") {
  SECTION("Check Postfix Expression") {

    /// 0 NO OPERATOR TEST
    REQUIRE(AssignmentExpression("02").GetExpressionString()
                == "02"); // this case should not happen as syntax will flag leading 0 as error
    REQUIRE(AssignmentExpression("102").GetExpressionString() == "102");
    REQUIRE(AssignmentExpression("Weeeeew").GetExpressionString() == "Weeeeew");
    REQUIRE(AssignmentExpression("AssignmentExpression").GetExpressionString() == "AssignmentExpression");

    /// 1 OPERATOR TEST
    REQUIRE(AssignmentExpression("a+b").GetExpressionString() == "a b +");
    REQUIRE(AssignmentExpression("a-b").GetExpressionString() == "a b -");
    REQUIRE(AssignmentExpression("a*b").GetExpressionString() == "a b *");
    REQUIRE(AssignmentExpression("a/b").GetExpressionString() == "a b /");
    REQUIRE(AssignmentExpression("a%b").GetExpressionString() == "a b %");
    REQUIRE(AssignmentExpression("a^b").GetExpressionString() == "a b ^");
    REQUIRE(AssignmentExpression("(a+b)").GetExpressionString() == "a b +");

    /// 2 SAME RANK OPERATOR (CHECK FOR LEFT-ASSOC)
    REQUIRE(AssignmentExpression("a+b+c").GetExpressionString() == "a b + c +");
    REQUIRE(AssignmentExpression("a+b-c").GetExpressionString() == "a b + c -");
    REQUIRE(AssignmentExpression("a-b+c").GetExpressionString() == "a b - c +");
    REQUIRE(AssignmentExpression("a*b*c").GetExpressionString() == "a b * c *");
    REQUIRE(AssignmentExpression("a/b/c").GetExpressionString() == "a b / c /");
    REQUIRE(AssignmentExpression("a%b%c").GetExpressionString() == "a b % c %");
    REQUIRE(AssignmentExpression("a/b*c").GetExpressionString() == "a b / c *");
    REQUIRE(AssignmentExpression("a^b^c").GetExpressionString() == "a b ^ c ^");

    /// 3 SMALL EQUATION, NO BRACKETS
    REQUIRE(AssignmentExpression("a+b/c").GetExpressionString() == "a b c / +");
    REQUIRE(AssignmentExpression("a*b+c/d*e").GetExpressionString() == "a b * c d / e * +");
    REQUIRE(AssignmentExpression("a^b+c/d/e%f^g").GetExpressionString() == "a b ^ c d / e / f g ^ % +");
    REQUIRE(AssignmentExpression("a*b*c*d/e*f*g").GetExpressionString() == "a b * c * d * e / f * g *");

    /// 4 SMALL EQUATION WITH BRACKETS
    REQUIRE(AssignmentExpression("(a+b)/c").GetExpressionString() == "a b + c /");
    REQUIRE(AssignmentExpression("a*(b+c/d)*e").GetExpressionString() == "a b c d / + * e *");
    REQUIRE(AssignmentExpression("a^(b+c)/(d-e)%f^g").GetExpressionString() == "a b c + ^ d e - / f g ^ %");
    REQUIRE(AssignmentExpression("(a*b*c*d)/(e*f*g)").GetExpressionString() == "a b * c * d * e f * g * /");
    REQUIRE(AssignmentExpression("a*b+(c+d*e-f)-g/h").GetExpressionString() == "a b * c d e * + f - + g h / -");

    /// 5 NAMES WITH DIFFERENT LENGTH
    REQUIRE(AssignmentExpression("You + Me + Who").GetExpressionString() == "You Me + Who +");
    REQUIRE(AssignmentExpression("What * The + (Weird + Equation * This - Is) - Aint % it").GetExpressionString()
                == "What The * Weird Equation This * + Is - + Aint it % -");
    REQUIRE(
        AssignmentExpression("never + g0nna * Give - u ^ up").GetExpressionString() == "never g0nna Give * + u up ^ -");
    REQUIRE(AssignmentExpression("never / g0nna * let - u - down").GetExpressionString()
                == "never g0nna / let * u - down -");
    REQUIRE(AssignmentExpression("(never + g0nNa) / (run * ar0und + n + dese3t) * u").GetExpressionString()
                == "never g0nNa + run ar0und * n + dese3t + / u *");

    /// 6 NESTED BRACKETS
    REQUIRE(AssignmentExpression("(((Young) + (Man) * (u) + dont / have * to + feel) + down)").GetExpressionString()
                == "Young Man u * + dont have / to * + feel + down +");
    REQUIRE(AssignmentExpression("(I) + (Said) * ((1) + was)").GetExpressionString() == "I Said 1 was + * +");

    /// 7 VARIABLE + NUMBERS
    REQUIRE(AssignmentExpression("Y * M ^ 3 + A").GetExpressionString() == "Y M 3 ^ * A +");
    REQUIRE(AssignmentExpression("It2+(fun/2)*sTay+at^the/Y+M-C*A").GetExpressionString()
                == "It2 fun 2 / sTay * + at the ^ Y / + M + C A * -");
  }

  SECTION("Check Equation Exact") {

    /// CHECK EXACT - EXACT
    REQUIRE(AssignmentExpression("102").CheckExact("102"));
    REQUIRE(AssignmentExpression("hotd0g").CheckExact("hotd0g"));
    REQUIRE(AssignmentExpression("a+b").CheckExact("a+b"));
    REQUIRE(AssignmentExpression("(a+b)").CheckExact("(a+b)"));
    REQUIRE(AssignmentExpression("1 * (5 + xyz) / q * LOL").CheckExact("1 * (5 + xyz) / q * LOL"));

    /// CHECK EXACT AFTER REMOVE BRACKET
    /// IF CHECKEXACT IS TRUE AFTER REMOVING BRACKET, THEN THE BRACKET IS USELESS.
    ///    OTHERWISE, IT IS NOT EXACT
    REQUIRE(AssignmentExpression("(102)").CheckExact("102"));
    REQUIRE(AssignmentExpression("a+b").CheckExact("(a+b)"));
    REQUIRE(AssignmentExpression("(a+b)").CheckExact("a+b"));
    REQUIRE(AssignmentExpression("(a+b)+c").CheckExact("a+b+c"));

    REQUIRE_FALSE(AssignmentExpression("(a+b)*c").CheckExact("a+b*c"));

    /// LECTURE 1 EXAMPLE
    REQUIRE(AssignmentExpression("v + x * y + z * t").CheckExact("v + x * y + z * t"));
    REQUIRE_FALSE(AssignmentExpression("v + x * y + z * t").CheckExact("v"));
  }

  SECTION("Check Equation Contains") {
    REQUIRE(AssignmentExpression("102").CheckExact("102"));
    REQUIRE_FALSE(AssignmentExpression("102").CheckExact("10"));
    REQUIRE_FALSE(AssignmentExpression("hotd0g").CheckExist("d0g"));
    REQUIRE_FALSE(AssignmentExpression("hotd0g").CheckExist("hot"));
    REQUIRE(AssignmentExpression("a+b").CheckExist("a"));
    REQUIRE(AssignmentExpression("a+b").CheckExist("b"));
    REQUIRE(AssignmentExpression("a+b").CheckExist("a+b"));
    REQUIRE(AssignmentExpression("x + j + z").CheckExist("z"));
    REQUIRE_FALSE(AssignmentExpression("byte").CheckExist("0"));
    REQUIRE_FALSE(AssignmentExpression("a+b").CheckExist("ab")); // do not allow blurring of variable in postfix check

    /// LECTURE 1 EXAMPLE
    REQUIRE(AssignmentExpression("v + x * y + z * t").CheckExist("v"));
    REQUIRE(AssignmentExpression("v + x * y + z * t").CheckExist("x * y"));
    REQUIRE_FALSE(AssignmentExpression("v + x * y + z * t").CheckExist("v + x"));
    REQUIRE(AssignmentExpression("v + x * y + z * t").CheckExist("v + x * y"));
    REQUIRE_FALSE(AssignmentExpression("v + x * y + z * t").CheckExist("y + z * t"));
    REQUIRE_FALSE(AssignmentExpression("v + x * y + z * t").CheckExist("x * y + z * t"));
    REQUIRE(AssignmentExpression("v + x * y + z * t").CheckExist("v + x * y + z * t"));
  }

}