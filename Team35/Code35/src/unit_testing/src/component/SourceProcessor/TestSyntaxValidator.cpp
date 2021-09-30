#include "catch.hpp"

#include <component/SourceProcessor/Tokenizer.h>
#include <component/SourceProcessor/SyntaxValidator.h>
#include <regex>
#include <datatype/RegexPatterns.h>
#include <util/TimeUtil.h>

using std::string;
using std::vector;

static vector<string> valid_program_lines = {
    // procedure:
    R"(procedure         Week4{)", // idx 0
    // macro functions:
    R"(read x;)",
    R"(    print x; )",
    R"(    call hackTheG; )",

    // if line:
    R"(   if (x >= 1) then { )", // idx: 4
    R"(   if (1 >= x) then { )",
    R"(   if (1 >= 1) then { )", // valid



    //else statement positive:
    R"(   else {)", // idx: 7
    R"(   else{)", //


    // while statement:
    R"(  while (x >= 1) then { )", // idx: 9

    R"(})",

    // assignment:
    R"( y = 1;)", // idx: 11
    R"(z                                                                                                   = 3;)",
};

static vector<string> invalid_program_lines = {
    // procedure:
    R"(procedure       {)", // no procname
    R"(procedure       )",
    // macro functions:
    R"(read ;)", // idx 2
    R"(read x)",
    R"(    print  )",
    R"(    print x )",
    R"(    print ; )",
    R"(    call hackTheG )",
    R"(    call ; )",

    // if line:
    R"(   if x >= 1) then { )", // idx: 9
    R"(   if x >= 1 then { )",
    R"(   if (x = 1) then { )",
    R"(   if (x >= 1) then  )",
    R"(   if (x >= 1) { )",
    R"(   if then { )",
    R"(   if (x) then { )",
    R"(   if () then { )",

    // else line:
    R"(   else )", //idx: 17

    // while statement:
    R"(   while x >= 1) then { )", // idx: 18
    R"(   while x >= 1 then { )",
    R"(   while (x = 1) then { )",
    R"(   while (x >= 1) then  )",
    R"(   while x >= 1 then { )", // repeated test
    R"(   while then { )",
    R"(   while (x) then { )",
    R"(   while () then { )",

//    // assignment:
    R"( y = 1 == 1;)", //  idx:26
    R"(z                                                                                                   = 3)",
    R"(z                                                                                                   = 3 > 1)",
};

/**
 *  Test helper function to run Validation against sample lines and see if it matches an expected_validation_output
 * @param start inclusive boundary for the lines to select
 * @param end
 * @param lines a common list of sentences in their string format
 * @param expected_validation_output an expected_validation_output outpuut value to be compared against
 * @return
 */
static bool CheckAgainstSampleLines(int start, int end, vector<string>& lines, bool expected_validation_output) {
  bool result = true;
  for (int i = start; i <= end; i++) {
    string line = lines.at(i);
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    bool output = SyntaxValidator::ValidateSyntax(tokens);
    if (output != expected_validation_output) {
      return false; // break early
    }
  }
  return result;
}

TEST_CASE("1.SyntaxValidator.Test helper functions") {
  SECTION("Testing time util") {
    std::string date_time = TimeUtil::GetDateTimeStr();
    std::string file_name_date_time = TimeUtil::GetDateTimeFileNameStr();
  }

  SECTION("Test count tokens") {
    string input = "> > +++ ??? nothingElseMattersssss trust ! 5eek I find In U  >= || &&  ||  <= ! &&  || <= hello ";
    vector<Token> tokens = Tokenizer::CreateTokens(input);
    Token target_token = Token("&&", TokenTag::kBooleanOperator);
//    Token* target_token = new Token("&&", TokenTag::kBooleanOperator);
    auto iterator = std::find_if(tokens.begin(), tokens.end(), [target_token](auto elem) {
      return elem == target_token;
    });
    int num_boolean_operators = Token::CountTokens(tokens, TokenTag::kBooleanOperator);
    int num_not_operators = Token::CountTokens(tokens, TokenTag::kBooleanOperator, "!");
    SECTION("only with token tags") {
      REQUIRE(num_boolean_operators == 7); // hardcoded test
    }
    SECTION("with token tags and string") {
      REQUIRE(num_not_operators == 2);
    }
  }

  SECTION("Is Factor function check") {
    string line = "0 1 2 3 4 5 varName (x + 1)";
    vector<Token> tokens = Tokenizer::CreateTokens(line);
//    [0] = {Token} {token_string_="0", token_tag_=kInteger}
//    [1] = {Token} {token_string_="1", token_tag_=kInteger}
//    [2] = {Token} {token_string_="2", token_tag_=kInteger}
//    [3] = {Token} {token_string_="3", token_tag_=kInteger}
//    [4] = {Token} {token_string_="4", token_tag_=kInteger}
//    [5] = {Token} {token_string_="5", token_tag_=kInteger}
//    [6] = {Token} {token_string_="varName", token_tag_=kName}
//    [7] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [8] = {Token} {token_string_="x", token_tag_=kName}
//    [9] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [10] = {Token} {token_string_="1", token_tag_=kInteger}
//    [11] = {Token} {token_string_=")", token_tag_=kCloseBracket}
    REQUIRE_FALSE(SyntaxValidator::IsFactor(tokens, 0, 3)); // because only accepts if it's a single int
    REQUIRE(SyntaxValidator::IsFactor(tokens, 0, 0)); // because only accepts if it's a single int
    REQUIRE(SyntaxValidator::IsFactor(tokens, 6, 6)); // because only accepts if it's a single varName
    REQUIRE(SyntaxValidator::IsFactor(tokens, 7, 11)); // an expression is also a factor
  }
  SECTION("Is Term function check") {
    string line = "VarNameIsASingleFactor "
                  "var1 * var2 % var3 / var4 % var5 * var6 var7 + var8 % ";

    vector<Token> tokens = Tokenizer::CreateTokens(line);

//    [0] = {Token} {token_string_="VarNameIsASingeFactor", token_tag_=kName}
//    [1] = {Token} {token_string_="var1", token_tag_=kName}
//    [2] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [3] = {Token} {token_string_="var2", token_tag_=kName}
//    [4] = {Token} {token_string_="%", token_tag_=kBinaryArithmeticOperator}
//    [5] = {Token} {token_string_="var3", token_tag_=kName}
//    [6] = {Token} {token_string_="/", token_tag_=kBinaryArithmeticOperator}
//    [7] = {Token} {token_string_="var4", token_tag_=kName}
//    [8] = {Token} {token_string_="%", token_tag_=kBinaryArithmeticOperator}
//    [9] = {Token} {token_string_="var5", token_tag_=kName}
//    [10] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [11] = {Token} {token_string_="var6", token_tag_=kName}
//    [12] = {Token} {token_string_="var7", token_tag_=kName}
//    [13] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [14] = {Token} {token_string_="var8", token_tag_=kName}
//    [15] = {Token} {token_string_="%", token_tag_=kBinaryArithmeticOperator}

    REQUIRE(SyntaxValidator::IsTerm(tokens, 0, 0)); // justa factor
    REQUIRE(SyntaxValidator::IsTerm(tokens, 1, 3)); // delim by *
    REQUIRE(SyntaxValidator::IsTerm(tokens, 7, 9)); // delim by %
    REQUIRE(SyntaxValidator::IsTerm(tokens, 1, 9)); // delim by %
    REQUIRE_FALSE(SyntaxValidator::IsTerm(tokens, 10, 12)); // invalid start with a delim
    REQUIRE_FALSE(SyntaxValidator::IsTerm(tokens, 1, 10)); // invalid ends with a delim
    REQUIRE_FALSE(SyntaxValidator::IsTerm(tokens, 2, 10)); // invalid starts and ends with a delim
    REQUIRE_FALSE(SyntaxValidator::IsTerm(tokens, 12, 14)); // invalid delim
  }


  // expr: expr ‘+’ term | expr ‘-’ term | term
  SECTION("Is Expr Function Check") {
    string line = "HelloWorld "
                  "var1 + var2 - var3 - var4 + var5 - var6  var7 / var8";

    vector<Token> tokens = Tokenizer::CreateTokens(line);

//    [0] = {Token} {token_string_="VarNameIsASingeFactor", token_tag_=kName}
//    [1] = {Token} {token_string_="var1", token_tag_=kName}
//    [2] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [3] = {Token} {token_string_="var2", token_tag_=kName}
//    [4] = {Token} {token_string_="-", token_tag_=kBinaryArithmeticOperator}
//    [5] = {Token} {token_string_="var3", token_tag_=kName}
//    [6] = {Token} {token_string_="-", token_tag_=kBinaryArithmeticOperator}
//    [7] = {Token} {token_string_="var4", token_tag_=kName}
//    [8] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [9] = {Token} {token_string_="var5", token_tag_=kName}
//    [10] = {Token} {token_string_="-", token_tag_=kBinaryArithmeticOperator}
//    [11] = {Token} {token_string_="var6", token_tag_=kName}
//    [12] = {Token} {token_string_="var7", token_tag_=kName}
//    [13] = {Token} {token_string_="/", token_tag_=kBinaryArithmeticOperator}
//    [14] = {Token} {token_string_="var8", token_tag_=kName}

    REQUIRE(SyntaxValidator::IsExpr(tokens, 0, 0)); // justa factor
    REQUIRE(SyntaxValidator::IsExpr(tokens, 1, 3)); // delim by +
    REQUIRE(SyntaxValidator::IsExpr(tokens, 7, 9)); // delim by +
    REQUIRE(SyntaxValidator::IsExpr(tokens, 1, 9)); // a bunch of + and - done correctly
    REQUIRE_FALSE(SyntaxValidator::IsExpr(tokens, 10, 13)); // invalid start with a delim
    REQUIRE_FALSE(SyntaxValidator::IsExpr(tokens, 1, 10)); // invalid start with a delim
    REQUIRE_FALSE(SyntaxValidator::IsExpr(tokens, 2, 10)); // invalid starts and ends with a delim
  }

  // expr: expr ‘+’ term | expr ‘-’ term | term
  SECTION("Is RelFactor Function Check") {
    string line = "if (myLifeSavings >= (myBeginningBalance + 3 * myStripperFees)) {";
//      [0] = {Token} {token_string_="if", token_tag_=kIfKeyword}
//      [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//      [2] = {Token} {token_string_="myLifeSavings", token_tag_=kName}
//      [3] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//      [4] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//      [5] = {Token} {token_string_="myBeginningBalance", token_tag_=kName}
//      [6] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//      [7] = {Token} {token_string_="3", token_tag_=kInteger}
//      [8] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//      [9] = {Token} {token_string_="myStripperFees", token_tag_=kName}
//      [10] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//      [11] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//      [12] = {Token} {token_string_="{", token_tag_=kOpenBrace}
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    REQUIRE(SyntaxValidator::IsRelFactor(tokens, 2, 2)); // justa factor
    REQUIRE(SyntaxValidator::IsRelFactor(tokens, 5, 9)); // it's a relative factor cuz it's an expression
    // rel-factor can only be surrounded on one end by a boolean comparison operator and the other by an open/close bracket
    REQUIRE(SyntaxValidator::IsRelFactor(tokens, 4, 10));
    REQUIRE_FALSE(SyntaxValidator::IsRelFactor(tokens, 1, 2));
    REQUIRE_FALSE(SyntaxValidator::IsRelFactor(tokens, 4, 11)); // unequal bracketing
  }
  SECTION("Is RelExpr tests") {
    string line = "if((x>=1)&&((y>69)||(z<0))){";
    string new_line = "!(200) |  !(xyz) | (p) && (q == 2)";
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    vector<Token> new_tokens = Tokenizer::CreateTokens(new_line);
//    [0] = {Token} {token_string_="if", token_tag_=kIfKeyword}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [3] = {Token} {token_string_="x", token_tag_=kName}
//    [4] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [5] = {Token} {token_string_="1", token_tag_=kInteger}
//    [6] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [7] = {Token} {token_string_="&&", token_tag_=kBooleanOperator}
//    [8] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [9] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [10] = {Token} {token_string_="y", token_tag_=kName}
//    [11] = {Token} {token_string_=">", token_tag_=kBinaryComparisonOperator}
//    [12] = {Token} {token_string_="69", token_tag_=kInteger}
//    [13] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [14] = {Token} {token_string_="||", token_tag_=kBooleanOperator}
//    [15] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [16] = {Token} {token_string_="z", token_tag_=kName}
//    [17] = {Token} {token_string_="<", token_tag_=kBinaryComparisonOperator}
//    [18] = {Token} {token_string_="0", token_tag_=kInteger}
//    [19] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [20] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [21] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [22] = {Token} {token_string_="{", token_tag_=kOpenBrace}
    REQUIRE(SyntaxValidator::IsRelExpr(tokens, 3, 5)); // no brackets just rel_expr
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(tokens, 2, 6)); // can't have rel_expr with extra bracket surrounding it
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(tokens, 9, 13)); // one layer of brackets around rel_expr
    REQUIRE(SyntaxValidator::IsRelExpr(tokens, 16, 18)); // one layer of brackets around rel_expr
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(tokens, 16, 19)); // uneven bracketing
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(tokens, 8, 20)); // Rel expression is not cond expr

//    [0] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="200", token_tag_=kInteger}
//    [3] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [4] = {Token} {token_string_="|", token_tag_=kInvalid}
//    [5] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [6] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [7] = {Token} {token_string_="xyz", token_tag_=kName}
//    [8] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [9] = {Token} {token_string_="|", token_tag_=kInvalid}
//    [10] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [11] = {Token} {token_string_="p", token_tag_=kName}
//    [12] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [13] = {Token} {token_string_="&&", token_tag_=kBooleanOperator}
//    [14] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [15] = {Token} {token_string_="q", token_tag_=kName}
//    [16] = {Token} {token_string_="==", token_tag_=kBinaryComparisonOperator}
//    [17] = {Token} {token_string_="2", token_tag_=kInteger}
//    [18] = {Token} {token_string_=")", token_tag_=kCloseBracket}

    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(new_tokens, 0, 3)); // uneven bracketing
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(new_tokens, 5, 8)); // uneven bracketing
    REQUIRE_FALSE(SyntaxValidator::IsRelExpr(new_tokens, 10, 18)); // uneven bracketing




  }
  SECTION("Is CondExpr tests") {
    string line =
        "if((x>=1)&&((y>69)||(z<0)) || !(3 > 1)) !!TheMatrixIsReal !!(x > 1){       while ((left==right) && (b>=c))  (k + j1k * chArlie){";
    vector<Token> tokens = Tokenizer::CreateTokens(line);
//    [0] = {Token} {token_string_="if", token_tag_=kName}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [3] = {Token} {token_string_="x", token_tag_=kName}
//    [4] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [5] = {Token} {token_string_="1", token_tag_=kInteger}
//    [6] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [7] = {Token} {token_string_="&&", token_tag_=kBooleanOperator}
//    [8] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [9] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [10] = {Token} {token_string_="y", token_tag_=kName}
//    [11] = {Token} {token_string_=">", token_tag_=kBinaryComparisonOperator}
//    [12] = {Token} {token_string_="69", token_tag_=kInteger}
//    [13] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [14] = {Token} {token_string_="||", token_tag_=kBooleanOperator}
//    [15] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [16] = {Token} {token_string_="z", token_tag_=kName}
//    [17] = {Token} {token_string_="<", token_tag_=kBinaryComparisonOperator}
//    [18] = {Token} {token_string_="0", token_tag_=kInteger}
//    [19] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [20] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [21] = {Token} {token_string_="||", token_tag_=kBooleanOperator}
//    [22] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [23] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [24] = {Token} {token_string_="3", token_tag_=kInteger}
//    [25] = {Token} {token_string_=">", token_tag_=kBinaryComparisonOperator}
//    [26] = {Token} {token_string_="1", token_tag_=kInteger}
//    [27] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [28] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [29] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [30] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [31] = {Token} {token_string_="TheMatrixIsReal", token_tag_=kName}
//    [32] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [33] = {Token} {token_string_="!", token_tag_=kBooleanOperator}
//    [34] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [35] = {Token} {token_string_="x", token_tag_=kName}
//    [36] = {Token} {token_string_=">", token_tag_=kBinaryComparisonOperator}
//    [37] = {Token} {token_string_="1", token_tag_=kInteger}
//    [38] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [39] = {Token} {token_string_="{", token_tag_=kOpenBrace}
//    [40] = {Token} {token_string_="while", token_tag_=kName}
//    [41] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [42] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [43] = {Token} {token_string_="left", token_tag_=kName}
//    [44] = {Token} {token_string_="==", token_tag_=kBinaryComparisonOperator}
//    [45] = {Token} {token_string_="right", token_tag_=kName}
//    [46] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [47] = {Token} {token_string_="&&", token_tag_=kBooleanOperator}
//    [48] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [49] = {Token} {token_string_="b", token_tag_=kName}
//    [50] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [51] = {Token} {token_string_="c", token_tag_=kName}
//    [52] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [53] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [54] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [55] = {Token} {token_string_="k", token_tag_=kName}
//    [56] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [57] = {Token} {token_string_="j1k", token_tag_=kName}
//    [58] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [59] = {Token} {token_string_="chArlie", token_tag_=kName}
//    [60] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [61] = {Token} {token_string_="{", token_tag_=kOpenBrace}
    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(tokens, 9, 13)); // not cond expr not rel expr cuz extra brackets
    REQUIRE(SyntaxValidator::IsCondExpr(tokens, 9, 19)); // (<cond_expr>) || (<cond_expr>)
    REQUIRE(SyntaxValidator::IsCondExpr(tokens, 22, 27)); // !(<cond_expr>)
//    REQUIRE(SyntaxValidator::IsCondExpr(tokens, 41, 53)); // !(<cond_expr>)
//    REQUIRE(SyntaxValidator::IsCondExpr(tokens, 54, 60)); // !(<cond_expr>)
//    REQUIRE(SyntaxValidator::IsTerm(tokens, 57, 59)); // !(<cond_expr>)

    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(tokens, 29, 31)); // !!xxx
    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(tokens, 32, 38)); // !!xxx

//    REQUIRE(SyntaxValidator::IsCondExpr(tokens, 2, 20)); // (<cond_expr>) || (<cond_expr>)
    string new_line = "!(200) |  !(xyz) | (p) && (q == 2)";
    vector<Token> new_tokens = Tokenizer::CreateTokens(new_line);
    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(new_tokens, 0, 3)); // uneven bracketing
    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(new_tokens, 5, 8)); // uneven bracketing
    REQUIRE_FALSE(SyntaxValidator::IsCondExpr(new_tokens, 10, 18)); // uneven bracketing


    string post_sub_line = " while (a>=(1*(1/(10)))) ";
    vector<Token> post_tokens = Tokenizer::CreateTokens(post_sub_line);
    int x = 1;

//    [0] = {Token} {token_string_="while", token_tag_=kName}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="a", token_tag_=kName}
//    [3] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [4] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [5] = {Token} {token_string_="1", token_tag_=kInteger}
//    [6] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [7] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [8] = {Token} {token_string_="1", token_tag_=kInteger}
//    [9] = {Token} {token_string_="/", token_tag_=kBinaryArithmeticOperator}
//    [10] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [11] = {Token} {token_string_="10", token_tag_=kInteger}
//    [12] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [13] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [14] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [15] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//     bool check_true = SyntaxValidator::IsCondExpr(post_tokens, 2, 14);
//     REQUIRE(check_true);

    string post_sub_line_2 = " while (a>=((1*1)/(10))) ";
    vector<Token> post_tokens_2 = Tokenizer::CreateTokens(post_sub_line_2);

//    [0] = {Token} {token_string_="while", token_tag_=kName}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="a", token_tag_=kName}
//    [3] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [4] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [5] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [6] = {Token} {token_string_="1", token_tag_=kInteger}
//    [7] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [8] = {Token} {token_string_="1", token_tag_=kInteger}
//    [9] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [10] = {Token} {token_string_="/", token_tag_=kBinaryArithmeticOperator}
//    [11] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [12] = {Token} {token_string_="10", token_tag_=kInteger}
//    [13] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [14] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [15] = {Token} {token_string_=")", token_tag_=kCloseBracket}
    bool check_true_2 = SyntaxValidator::IsCondExpr(post_tokens_2, 2, 14);
    REQUIRE(check_true_2);
    int y  = 1;
  }
  SECTION("Check StatementPassesCommonBlacklistRules function") {

    SECTION("positive tests") {
      vector<string> positive_statements;
      positive_statements.emplace_back("procedure procname {"); // assn
      positive_statements.emplace_back("a = b + 1;"); // assn
      positive_statements.emplace_back("call mama;"); // call
      positive_statements.emplace_back("}");
      positive_statements.emplace_back("read books;"); // read
      positive_statements.emplace_back("if(x > 12) then {");
      positive_statements.emplace_back("while (timeLeft > 0) {");

      for (const auto& statement: positive_statements) {
        vector<Token> tokenized_statement = Tokenizer::CreateTokens(statement);
        bool valid = SyntaxValidator::StatementPassesCommonBlacklistRules(tokenized_statement);
        REQUIRE(valid);
      }
    }
    SECTION("negative tests") {
      vector<string> negative_statements;
      negative_statements.emplace_back("a = b + 1");
      negative_statements.emplace_back(" mama");
      negative_statements.emplace_back("read books");
      negative_statements.emplace_back("ifx > 12) then {");
      negative_statements.emplace_back("if(x > 12 then ");
      negative_statements.emplace_back("if(x > 12)) then {");
      negative_statements.emplace_back("while (timeLeft > 0) ");
      for (const auto& statement: negative_statements) {
        vector<Token> tokenized_statement = Tokenizer::CreateTokens(statement);
        bool invalid = !SyntaxValidator::StatementPassesCommonBlacklistRules(tokenized_statement);
        REQUIRE(invalid);
      }
    }
  }
  SECTION("Test index finders (first matching token and last matching token) ") {
    string line = "if (myLifeSavings >= (myBeginningBalance + 3 * myStripperFees)) { &&";
    vector<Token> tokens = Tokenizer::CreateTokens(line);
    int first_comparator_idx = Token::GetFirstMatchingTokenIdx(tokens,
                                                               RegexPatterns::GetBinaryComparisonPattern(),
                                                               0,
                                                               tokens.size() - 1);
    int first_arithmetic_operator = Token::GetFirstMatchingTokenIdx(tokens,
                                                                    RegexPatterns::GetBinaryArithmeticOperatorPattern(),
                                                                    0,
                                                                    tokens.size() - 1);
    int first_boolean_operator = Token::GetFirstMatchingTokenIdx(tokens,
                                                                 RegexPatterns::GetBinaryBooleanOperatorPattern(),
                                                                 0,
                                                                 tokens.size() - 1);
    int last_boolean_operator = Token::GetLastMatchingTokenIdx(tokens,
                                                               RegexPatterns::GetBinaryBooleanOperatorPattern(),
                                                               0,
                                                               tokens.size() - 1);
    int last_arithmetic_operator = Token::GetLastMatchingTokenIdx(tokens,
                                                                  RegexPatterns::GetBinaryArithmeticOperatorPattern(),
                                                                  0,
                                                                  tokens.size() - 1);
    REQUIRE(first_comparator_idx == 3);
    REQUIRE(first_arithmetic_operator == 6);
    REQUIRE(last_arithmetic_operator == 8);
    REQUIRE(first_boolean_operator == 13);
    REQUIRE(last_boolean_operator == 13);

//    [0] = {Token} {token_string_="if", token_tag_=kIfKeyword}
//    [1] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [2] = {Token} {token_string_="myLifeSavings", token_tag_=kName}
//    [3] = {Token} {token_string_=">=", token_tag_=kBinaryComparisonOperator}
//    [4] = {Token} {token_string_="(", token_tag_=kOpenBracket}
//    [5] = {Token} {token_string_="myBeginningBalance", token_tag_=kName}
//    [6] = {Token} {token_string_="+", token_tag_=kBinaryArithmeticOperator}
//    [7] = {Token} {token_string_="3", token_tag_=kInteger}
//    [8] = {Token} {token_string_="*", token_tag_=kBinaryArithmeticOperator}
//    [9] = {Token} {token_string_="myStripperFees", token_tag_=kName}
//    [10] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [11] = {Token} {token_string_=")", token_tag_=kCloseBracket}
//    [12] = {Token} {token_string_="{", token_tag_=kOpenBrace}
//    [13] = {Token} {token_string_="&&", token_tag_=kBooleanOperator}
  }
}

TEST_CASE("1.SyntaxValidator.Validator handles statement validation:") {
  SECTION("handles procedure statement") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(0, 0, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(0, 1, invalid_program_lines, false));
    }
  }

  SECTION("handles macro function calls like read, print and call") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(1, 3, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(2, 8, invalid_program_lines, false));
    }
  }

  SECTION("handles assignment statements") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(11, 12, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(26, 28, invalid_program_lines, false));
    }
  }
  SECTION("handles \"if\" statements") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(4, 6, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(9, 16, invalid_program_lines, false));
    }
  }

  SECTION("handles \"else\" statements") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(7, 8, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(17, 17, invalid_program_lines, false));
    }
  }

  SECTION("handles while statements") {
    SECTION("positive cases") {
      REQUIRE(CheckAgainstSampleLines(9, 9, valid_program_lines, true));
    }
    SECTION("negative cases") {
      REQUIRE(CheckAgainstSampleLines(18, 25, invalid_program_lines, false));
    }
  }
}

TEST_CASE("1.SyntaxValidator.EdgeCases") {
  vector<string> invalid_lines;
  invalid_lines.emplace_back(R"(procedure X meow = 1;)");
  invalid_lines.emplace_back(R"(if (x==1) then while (p == q) {)"); // mixups
  invalid_lines.emplace_back(R"(x = 1 if(x>1) then { )");
  invalid_lines.emplace_back(R"(if(x>1) then{ x = x - 1; })"); // shouldn't be in same line
  invalid_lines.emplace_back(R"(if(x>1) then{ x = x - 1; } else {x = x + 1;})"); // shouldn't be in same line
  invalid_lines.emplace_back(R"(while(x>1) { x = x + 1; } then{ x = x - 1; } )"); // shouldn't be in same line
  invalid_lines.emplace_back(R"(while(x>1) then { x = x + 1; } else {x = x + 1;})"); // shouldn't be in same line
  invalid_lines.emplace_back(R"(x=x+1 y = y -1 x = x %x;)"); // shouldn't be in same line
  REQUIRE(CheckAgainstSampleLines(0, 7, invalid_lines, false));
}

TEST_CASE("1.SyntaxValidator.KeywordsUsedAsNames") {
  vector<string> valid_lines = {
      R"(procedure=1;)",
      R"(procedure procedure {)",
      R"(procedure if {)",
      R"(procedure then {)",
      R"(procedure while {)",
      R"(if = 10;)",
      R"(else = 12;)",
      R"(while = 12 + if;)",
      R"(while = 12 + then;)",
      R"(if = if + then;)",
      R"(if = if + then;)",
      R"(x = while + then * procedure;)",
  };

  SECTION("Positive tests") {
    REQUIRE(CheckAgainstSampleLines(0, valid_lines.size() - 1, valid_lines, true));
  }
}

