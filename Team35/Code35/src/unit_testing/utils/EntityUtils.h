#ifndef AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_
#define AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_

#include <model/Entity.h>
#include <model/Statement.h>

/**
 * This class provides pre built entities for testing.
 */
namespace entity_utils {

// variables
static inline Variable* var_x_ = new Variable(new VariableName("x"));
static inline Variable* var_y_ = new Variable(new VariableName("y"));
static inline Variable* var_z_ = new Variable(new VariableName("z"));
static inline Variable* var_i_ = new Variable(new VariableName("i"));

// constant values
static inline ConstantValue* const_0_ = new ConstantValue("0");
static inline ConstantValue* const_1_ = new ConstantValue("1");
static inline ConstantValue* const_2_ = new ConstantValue("2");
static inline ConstantValue* const_3_ = new ConstantValue("3");

// assignments
// z = 1
static inline AssignEntity* assign_1_ = new AssignEntity(var_z_,
                                                         "1",
                                                         std::vector<Variable*>(),
                                                         std::vector<ConstantValue*>{const_1_});
// z = 3
static inline AssignEntity* assign_3_ = new AssignEntity(var_z_,
                                                         "3",
                                                         std::vector<Variable*>(),
                                                         std::vector<ConstantValue*>{const_3_});
// y = z
static inline AssignEntity* assign_4_ = new AssignEntity(var_y_,
                                                         "z",
                                                         std::vector<Variable*>{var_z_},
                                                         std::vector<ConstantValue*>());
// z = y
static inline AssignEntity* assign_5_ = new AssignEntity(var_z_,
                                                         "y",
                                                         std::vector<Variable*>{var_y_},
                                                         std::vector<ConstantValue*>());
// y = i
static inline AssignEntity* assign_6_ = new AssignEntity(var_y_,
                                                         "i",
                                                         std::vector<Variable*>{var_i_},
                                                         std::vector<ConstantValue*>());
// i = z
static inline AssignEntity* assign_7_ = new AssignEntity(var_i_,
                                                         "z",
                                                         std::vector<Variable*>{var_z_},
                                                         std::vector<ConstantValue*>());
// x = z
static inline AssignEntity* assign_8_ = new AssignEntity(var_x_,
                                                         "z",
                                                         std::vector<Variable*>{var_z_},
                                                         std::vector<ConstantValue*>());

// if
static inline IfEntity* if_1_ = new IfEntity("x==0",
                                             std::vector<Variable*>{
                                                 var_x_
                                             },
                                             std::vector<ConstantValue*>{
                                                 const_0_
                                             });
static inline IfEntity* if_2_ = new IfEntity("y<=3",
                                             std::vector<Variable*>{
                                                 var_y_
                                             },
                                             std::vector<ConstantValue*>{
                                                 const_3_
                                             });
static inline IfEntity* if_3_ = new IfEntity("z>1",
                                             std::vector<Variable*>{
                                                 var_z_
                                             },
                                             std::vector<ConstantValue*>{
                                                 const_1_
                                             });
static inline ElseEntity* else_1_ = new ElseEntity();
static inline ElseEntity* else_2_ = new ElseEntity();
static inline ElseEntity* else_3_ = new ElseEntity();

// while
static inline WhileEntity* while_1_ = new WhileEntity("x==0", std::vector<Variable*>{
                                                          var_x_
                                                      },
                                                      std::vector<ConstantValue*>{
                                                          const_0_
                                                      });
static inline WhileEntity* while_2_ = new WhileEntity("y<=3", std::vector<Variable*>{
                                                          var_y_
                                                      },
                                                      std::vector<ConstantValue*>{
                                                          const_3_
                                                      });
static inline WhileEntity* while_3_ = new WhileEntity("z>1", std::vector<Variable*>{
                                                          var_z_
                                                      },
                                                      std::vector<ConstantValue*>{
                                                          const_1_
                                                      });
}

#endif //AUTOTESTER_SRC_UNIT_TESTING_UTILS_ENTITYUTILS_H_