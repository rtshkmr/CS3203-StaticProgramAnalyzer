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
inline ConstantValue* const_0_ = new ConstantValue("0");
inline ConstantValue* const_1_ = new ConstantValue("1");
inline ConstantValue* const_2_ = new ConstantValue("2");
inline ConstantValue* const_3_ = new ConstantValue("3");

/**
 * These functions provide the ability to redefine the statements,
 * so that they do not carry modifications across test cases.
git */

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
                          std::vector<ConstantValue*>{const_1_});
}
// x=2
inline AssignEntity* GetAssign2() {
  return new AssignEntity(var_x_,
                          "2",
                          std::vector<Variable*>(),
                          std::vector<ConstantValue*>{const_2_});
}
// z=3
inline AssignEntity* GetAssign3() {
  return new AssignEntity(var_z_,
                          "3",
                          std::vector<Variable*>(),
                          std::vector<ConstantValue*>{const_3_});
}
// y=z
inline AssignEntity* GetAssign4() {
  return new AssignEntity(var_y_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<ConstantValue*>());
}
// z=y
inline AssignEntity* GetAssign5() {
  return new AssignEntity(var_z_,
                          "y",
                          std::vector<Variable*>{var_y_},
                          std::vector<ConstantValue*>());
}
// y=i
inline AssignEntity* GetAssign6() {
  return new AssignEntity(var_y_,
                          "i",
                          std::vector<Variable*>{var_i_},
                          std::vector<ConstantValue*>());
}
// i=z
inline AssignEntity* GetAssign7() {
  return new AssignEntity(var_i_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<ConstantValue*>());
}
// x=z
inline AssignEntity* GetAssign8() {
  return new AssignEntity(var_x_,
                          "z",
                          std::vector<Variable*>{var_z_},
                          std::vector<ConstantValue*>());
}
// x=x
inline AssignEntity* GetAssign9() {
  return new AssignEntity(var_x_,
                          "x",
                          std::vector<Variable*>{var_x_},
                          std::vector<ConstantValue*>());
}
// x=y
inline AssignEntity* GetAssign10() {
  return new AssignEntity(var_x_,
                          "y",
                          std::vector<Variable*>{var_y_},
                          std::vector<ConstantValue*>());
}

// if
inline IfEntity* GetIf1() {
  return new IfEntity("x==0",
                      std::vector<Variable*>{
                          var_x_
                      },
                      std::vector<ConstantValue*>{
                          const_0_
                      });
}
inline IfEntity* GetIf2() {
  return new IfEntity("y<=3",
                      std::vector<Variable*>{
                          var_y_
                      },
                      std::vector<ConstantValue*>{
                          const_3_
                      });
}
inline IfEntity* GetIf3() {
  return new IfEntity("z>1",
                      std::vector<Variable*>{
                          var_z_
                      },
                      std::vector<ConstantValue*>{
                          const_1_
                      });

}
inline IfEntity* GetIf4() {
  return new IfEntity("i!=2",
                      std::vector<Variable*>{
                          var_i_
                      },
                      std::vector<ConstantValue*>{
                          const_2_
                      });
}

inline ElseEntity* GetElse() {
  return new ElseEntity();
}

// while
inline WhileEntity* GetWhileEntity1() {
  return new WhileEntity("x==0", std::vector<Variable*>{
                             var_x_
                         },
                         std::vector<ConstantValue*>{
                             const_0_
                         });
}
inline WhileEntity* GetWhileEntity2() {
  return new WhileEntity("y<=3", std::vector<Variable*>{
                             var_y_
                         },
                         std::vector<ConstantValue*>{
                             const_3_
                         });
}
inline WhileEntity* GetWhileEntity3() {
  return new WhileEntity("z>1", std::vector<Variable*>{
                             var_z_
                         },
                         std::vector<ConstantValue*>{
                             const_1_
                         });
}

// For copying
//
//ReadEntity* read_x_ = GetReadX();
//ReadEntity* read_y_ = GetReadY();
//ReadEntity* read_z_ = GetReadZ();
//ReadEntity* read_i_ = GetReadI();
//ReadEntity* read_n_ = GetReadN();
//ReadEntity* read_m_ = GetReadM();
//PrintEntity* print_x_ = GetPrintX();
//PrintEntity* print_y_ = GetPrintY();
//PrintEntity* print_z_ = GetPrintZ();
//PrintEntity* print_i_ = GetPrintI();
//PrintEntity* print_n_ = GetPrintN();
//PrintEntity* print_m_ = GetPrintM();
//AssignEntity* assign_1_ = GetAssign1();
//AssignEntity* assign_2_ = GetAssign2();
//AssignEntity* assign_3_ = GetAssign3();
//AssignEntity* assign_4_ = GetAssign4();
//AssignEntity* assign_5_ = GetAssign5();
//AssignEntity* assign_6_ = GetAssign6();
//AssignEntity* assign_7_ = GetAssign7();
//AssignEntity* assign_8_ = GetAssign8();
//AssignEntity* assign_9_ = GetAssign9();
//AssignEntity* assign_10_ = GetAssign10();
//IfEntity* if_1_ = GetIf1();
//IfEntity* if_2_ = GetIf2();
//IfEntity* if_3_ = GetIf3();
//IfEntity* if_4_ = GetIf4();
//ElseEntity* else_1_ = GetElse();
//ElseEntity* else_2_ = GetElse();
//ElseEntity* else_3_ = GetElse();
//ElseEntity* else_4_ = GetElse();
//WhileEntity* while_1_ = GetWhileEntity3();
//WhileEntity* while_2_ = GetWhileEntity2();
//WhileEntity* while_3_ = GetWhileEntity3();

}

#endif //AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_
