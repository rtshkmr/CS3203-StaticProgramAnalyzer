file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers}
        src/model/Entity.h src/model/Entity.cpp
        src/datatype/DataType.cpp src/datatype/DataType.h
        src/datatype/AssignmentExpression.cpp src/datatype/AssignmentExpression.h
        src/datatype/ConditionalExpression.cpp src/datatype/ConditionalExpression.h
        src/model/Statement.h src/model/Statement.cpp
        src/component/PKB/PKB.cpp src/component/PKB/PKB.h
        src/component/SourceProcessor/DesignExtractor.cpp
        src/component/SourceProcessor/Parser.h src/component/SourceProcessor/Parser.cpp
        )
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


