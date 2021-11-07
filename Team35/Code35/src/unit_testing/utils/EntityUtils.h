#ifndef AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_
#define AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_

#include <model/Entity.h>
#include <model/Statement.h>

/**
 * This class provides pre built entities for testing. When using these, note that changes might persist across test
 * cases because of the inline nature.
 */
namespace entity_utils {

// variables
inline Variable* var_x_ = new Variable(new VariableName("x"));
inline Variable* var_y_ = new Variable(new VariableName("y"));
inline Variable* var_z_ = new Variable(new VariableName("z"));
inline Variable* var_i_ = new Variable(new VariableName("i"));
inline Variable* var_n_ = new Variable(new VariableName("n"));
inline Variable* var_m_ = new Variable(new VariableName("m"));

// constant values
inline Constant* const_0_ = new Constant(new ConstantValue("0"));
inline Constant* const_1_ = new Constant(new ConstantValue("1"));
inline Constant* const_2_ = new Constant(new ConstantValue("2"));
inline Constant* const_3_ = new Constant(new ConstantValue("3"));

/**
 * These functions provide the ability to redefine the statements,
 * so that they do not carry modifications across test cases.
 */
// variables
inline Variable* GetVarX() {
  return var_x_;
}
inline Variable* GetVarY() {
  return var_y_;
}
inline Variable* GetVarZ() {
  return var_z_;
}
inline Variable* GetVarI() {
  return var_i_;
}
inline Variable* GetVarN() {
  return var_n_;
}
inline Variable* GetVarM() {
  return var_m_;
}

// Constants
inline Constant* GetConst0() {
  return const_0_;
}
inline Constant* GetConst1() {
  return const_1_;
}
inline Constant* GetConst2() {
  return const_2_;
}
inline Constant* GetConst3() {
  return const_3_;
}
inline Constant* GetPlaceholderEntity() {
  return const_0_;
}

// Procedures
inline Procedure* GetProc1() {
  return new Procedure(new ProcedureName("proc1"));
}
inline Procedure* GetProc2() {
  return new Procedure(new ProcedureName("proc2"));
}
inline Procedure* GetProc3() {
  return new Procedure(new ProcedureName("proc3"));
}
inline Procedure* GetProc4() {
  return new Procedure(new ProcedureName("proc4"));
}
inline Procedure* GetProc5() {
  return new Procedure(new ProcedureName("proc5"));
}

// read
inline ReadEntity* GetReadX() {
  return new ReadEntity(var_x_);
}
inline ReadEntity* GetReadY() {
  return new ReadEntity(var_y_);
}
inline ReadEntity* GetReadZ() {
  return new ReadEntity(var_z_);
}
inline ReadEntity* GetReadI() {
  return new ReadEntity(var_i_);
}
inline ReadEntity* GetReadN() {
  return new ReadEntity(var_n_);
}
inline ReadEntity* GetReadM() {
  return new ReadEntity(var_m_);
}

// print
inline PrintEntity* GetPrintX() {
  return new PrintEntity(var_x_);
}
inline PrintEntity* GetPrintY() {
  return new PrintEntity(var_y_);
}
inline PrintEntity* GetPrintZ() {
  return new PrintEntity(var_z_);
}
inline PrintEntity* GetPrintI() {
  return new PrintEntity(var_i_);
}
inline PrintEntity* GetPrintN() {
  return new PrintEntity(var_n_);
}
inline PrintEntity* GetPrintM() {
  return new PrintEntity(var_m_);
}

// assign
// z=1
inline AssignEntity* GetAssign1() {
  return new AssignEntity(var_z_,
                          "1",
                          std::vector<Variable*>(),
                          std::vector<Constant*>{const_1_});
}
// x=2
inline AssignEntity* GetAssign2() {
  return new AssignEntity(var_x_,
                          "2",
                          std::vector<Variable*>(),
                          std::vector<Constant*>{const_2_});
}
// z=3
inline AssignEntity* GetAssign3() {
  return new AssignEntity(var_z_,
                          "3",
                          std::vector<Variable*>(),
                          std::vector<Constant*>{const_3_});
}
// y=z
inline AssignEntity* GetAssign4() {
  return new AssignEntity(var_y_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<Constant*>());
}
// z=y
inline AssignEntity* GetAssign5() {
  return new AssignEntity(var_z_,
                          "y",
                          std::vector<Variable*>{var_y_},
                          std::vector<Constant*>());
}
// y=i
inline AssignEntity* GetAssign6() {
  return new AssignEntity(var_y_,
                          "i",
                          std::vector<Variable*>{var_i_},
                          std::vector<Constant*>());
}
// i=z
inline AssignEntity* GetAssign7() {
  return new AssignEntity(var_i_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<Constant*>());
}
// x=z
inline AssignEntity* GetAssign8() {
  return new AssignEntity(var_x_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<Constant*>());
}
// x=x
inline AssignEntity* GetAssign9() {
  return new AssignEntity(var_x_,
                          "x",
                          std::vector<Variable*>{var_x_},
                          std::vector<Constant*>());
}
// x=y
inline AssignEntity* GetAssign10() {
  return new AssignEntity(var_x_,
                          "y",
                          std::vector<Variable*>{var_y_},
                          std::vector<Constant*>());
}
// m=n
inline AssignEntity* GetAssign11() {
  return new AssignEntity(var_m_,
                          "n",
                          std::vector<Variable*>{var_n_},
                          std::vector<Constant*>());
}

// if
inline IfEntity* GetIf1() {
  return new IfEntity("x==0",
                      std::vector<Variable*>{
                          var_x_
                      },
                      std::vector<Constant*>{
                          const_0_
                      });
}
inline IfEntity* GetIf2() {
  return new IfEntity("y<=3",
                      std::vector<Variable*>{
                          var_y_
                      },
                      std::vector<Constant*>{
                          const_3_
                      });
}
inline IfEntity* GetIf3() {
  return new IfEntity("z>1",
                      std::vector<Variable*>{
                          var_z_
                      },
                      std::vector<Constant*>{
                          const_1_
                      });

}
inline IfEntity* GetIf4() {
  return new IfEntity("i!=2",
                      std::vector<Variable*>{
                          var_i_
                      },
                      std::vector<Constant*>{
                          const_2_
                      });
}

inline ElseEntity* GetElse() {
  return new ElseEntity();
}

// no variable style
// if (1 == 2) { }
inline IfEntity* GetIf6() {
  return new IfEntity("1 == 2",
                      std::vector<Variable*>{ },
                      std::vector<Constant*>{
                          const_1_,
                          const_2_
                      });
}

// 2 variable style
// if (x == y) { }
inline IfEntity* GetIf7() {
  return new IfEntity("x == y",
                      std::vector<Variable*>{
                          var_x_,
                          var_y_
                      },
                      std::vector<Constant*>{});
}

// 3 variable style
// if ((z == y) && (z == x)) { }
inline IfEntity* GetIf8() {
  return new IfEntity("(z == y) && (z == x)",
                      std::vector<Variable*>{
                          var_z_,
                          var_y_,
                          var_x_
                      },
                      std::vector<Constant*>{});
}

// 3 variable style + 2 constants
// if (((x == 0) && (x == y)) || ((x >= 1) && (x == z))) { }
inline IfEntity* GetIf9() {
  return new IfEntity("((x == 0) && (x == y)) || ((x >= 1) && (x == z))",
                      std::vector<Variable*>{
                          var_x_,
                          var_y_,
                          var_z_
                      },
                      std::vector<Constant*>{
                          const_1_,
                          const_2_
                      });
}


// while
inline WhileEntity* GetWhileEntity1() {
  return new WhileEntity("x==0", std::vector<Variable*>{
                             var_x_
                         },
                         std::vector<Constant*>{
                             const_0_
                         });
}
inline WhileEntity* GetWhileEntity2() {
  return new WhileEntity("y<=3", std::vector<Variable*>{
                             var_y_
                         },
                         std::vector<Constant*>{
                             const_3_
                         });
}
inline WhileEntity* GetWhileEntity3() {
  return new WhileEntity("z>1", std::vector<Variable*>{
                             var_z_
                         },
                         std::vector<Constant*>{
                             const_1_
                         });
}

// util
inline Statement* CreateStatement(Entity* entity, int stmt_num) {
  Statement* stmt = dynamic_cast<Statement*>(entity);
  stmt->SetStatementNumber(new StatementNumber(stmt_num));
  return stmt;
}

// For copying
//
//Procedure* proc1 = GetProc1();
//Procedure* proc2 = GetProc2();
//Procedure* proc3 = GetProc3();
//Procedure* proc4 = GetProc4();
//Procedure* proc5 = GetProc5();
//ReadEntity* read_x = GetReadX();
//ReadEntity* read_y = GetReadY();
//ReadEntity* read_z = GetReadZ();
//ReadEntity* read_i = GetReadI();
//ReadEntity* read_n = GetReadN();
//ReadEntity* read_m = GetReadM();
//PrintEntity* print_x = GetPrintX();
//PrintEntity* print_y = GetPrintY();
//PrintEntity* print_z = GetPrintZ();
//PrintEntity* print_i = GetPrintI();
//PrintEntity* print_n = GetPrintN();
//PrintEntity* print_m = GetPrintM();
//AssignEntity* assign_1 = GetAssign1();
//AssignEntity* assign_2 = GetAssign2();
//AssignEntity* assign_3 = GetAssign3();
//AssignEntity* assign_4 = GetAssign4();
//AssignEntity* assign_5 = GetAssign5();
//AssignEntity* assign_6 = GetAssign6();
//AssignEntity* assign_7 = GetAssign7();
//AssignEntity* assign_8 = GetAssign8();
//AssignEntity* assign_9 = GetAssign9();
//AssignEntity* assign_10 = GetAssign10();
//IfEntity* if_1 = GetIf1();
//IfEntity* if_2 = GetIf2();
//IfEntity* if_3 = GetIf3();
//IfEntity* if_4 = GetIf4();
//ElseEntity* else_1 = GetElse();
//ElseEntity* else_2 = GetElse();
//ElseEntity* else_3 = GetElse();
//ElseEntity* else_4 = GetElse();
//WhileEntity* while_1 = GetWhileEntity1();
//WhileEntity* while_2 = GetWhileEntity2();
//WhileEntity* while_3 = GetWhileEntity3();

}

#endif //AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_
