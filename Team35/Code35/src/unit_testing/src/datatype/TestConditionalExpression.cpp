#include "catch.hpp"
#include "datatype/ConditionalExpression.h"
#include "../../utils/EntityUtils.h"

using namespace entity_utils;

TEST_CASE("1.DataType.ConditionalExpression") {
  // TODO: Should not affect test coverage but - Add While variants.

  // if6 (1 == 2) - no variable style
  SECTION("0 variable") {
    IfEntity* if6 = GetIf6();
    REQUIRE(if6->GetCondExpr()->GetExpressionString() == "1 == 2");
    REQUIRE(if6->GetCondExpr()->GetVariableList() == std::vector<Variable*>{ });
    REQUIRE_FALSE(if6->GetCondExpr()->CheckExist(var_i_));
    REQUIRE_FALSE(if6->GetCondExpr()->CheckExist(var_x_));
  }

  // if1 (x == 0)
  SECTION("1 variable") {
    IfEntity* if1 = GetIf1();
    REQUIRE(if1->GetCondExpr()->GetExpressionString() == "x==0");
    REQUIRE(if1->GetCondExpr()->GetVariableList() == std::vector<Variable*>{ var_x_ });
    REQUIRE(if1->GetCondExpr()->CheckExist(var_x_));
    REQUIRE_FALSE(if1->GetCondExpr()->CheckExist(var_y_));
    REQUIRE_FALSE(if1->GetCondExpr()->CheckExist(var_z_));
  }

  // if7 (x == y) - 2 variable, no constant
  SECTION("2 variable, no constant") {
    IfEntity* if7 = GetIf7();
    REQUIRE(if7->GetCondExpr()->GetExpressionString() == "x == y");
    REQUIRE(if7->GetCondExpr()->GetVariableList() == std::vector<Variable*>{ var_x_, var_y_ });
    REQUIRE(if7->GetCondExpr()->CheckExist(var_x_));
    REQUIRE(if7->GetCondExpr()->CheckExist(var_y_));
    REQUIRE_FALSE(if7->GetCondExpr()->CheckExist(var_i_));
    REQUIRE_FALSE(if7->GetCondExpr()->CheckExist(var_n_));
  }

  // if8 ((z == y) && (z == x)) - 3 variable style -- Note: also test if variable is sorted. 
  SECTION("3 variable, no constant") {
    IfEntity* if8 = GetIf8();
    REQUIRE(if8->GetCondExpr()->GetExpressionString() == "(z == y) && (z == x)");
    
    // This test below is to check if variable in CondExpr is sorted. 
    // Naturally if CondExpr is not sorted, BST may not work. 
    REQUIRE(if8->GetExpressionVariables() == std::vector<Variable*> { var_z_, var_y_, var_x_ }); 
    REQUIRE(if8->GetCondExpr()->GetVariableList() == std::vector<Variable*>{ var_x_, var_y_, var_z_ });
    
    REQUIRE(if8->GetCondExpr()->CheckExist(var_x_));
    REQUIRE(if8->GetCondExpr()->CheckExist(var_y_));
    REQUIRE(if8->GetCondExpr()->CheckExist(var_z_));
    REQUIRE_FALSE(if8->GetCondExpr()->CheckExist(var_i_));
    REQUIRE_FALSE(if8->GetCondExpr()->CheckExist(var_m_));
  }

  // if9(((x == 0) && (x == y)) || ((x >= 1) && (x == z))) - 3 variable style + 2 constants
  SECTION("Mixed") {
    IfEntity* if9 = GetIf9();
    REQUIRE(if9->GetCondExpr()->GetExpressionString() == "((x == 0) && (x == y)) || ((x >= 1) && (x == z))");
    REQUIRE(if9->GetCondExpr()->GetVariableList() == std::vector<Variable*>{ var_x_, var_y_, var_z_ });
    REQUIRE(if9->GetCondExpr()->CheckExist(var_x_));
    REQUIRE(if9->GetCondExpr()->CheckExist(var_y_));
    REQUIRE(if9->GetCondExpr()->CheckExist(var_z_));
    REQUIRE_FALSE(if9->GetCondExpr()->CheckExist(var_i_));
    REQUIRE_FALSE(if9->GetCondExpr()->CheckExist(var_m_));
  }
  
}
