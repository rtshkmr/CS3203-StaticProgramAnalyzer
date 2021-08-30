file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} src/model/Entity.h src/datatype/DataType.cpp src/datatype/AssignmentExpression.cpp src/datatype/AssignmentExpression.h src/datatype/DataType.h src/model/Statement.h src/datatype/ConditionalExpression.cpp src/datatype/ConditionalExpression.h src/model/Entity.cpp src/model/Statement.cpp)
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


