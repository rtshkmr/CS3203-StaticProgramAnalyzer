file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(unit_testing ${srcs}
        src/component/QueryProcessor/types/TestQueryEvaluatorTable.cpp
        src/component/QueryProcessor/TestQueryProjector.cpp
        src/component/QueryProcessor/QueryExtractor/TestQueryExtractor.cpp
        src/component/QueryProcessor/QueryExtractor/TestQueryGrouper.cpp
        src/component/QueryProcessor/QueryEvaluator/TestQueryEvaluator.cpp
        src/component/SourceProcessor/TestTokenizer.cpp
        src/component/SourceProcessor/TestSyntaxValidator.cpp
        src/component/SourceProcessor/TestEntityFactory.cpp
        utils/TestUtils.cpp utils/TestUtils.h
        utils/TestUtils.cpp src/datatype/TestDataType.cpp
        src/component/SourceProcessor/TestPsubsystem.cpp
        src/component/PKB/TestPKB.cpp src/datatype/TestAssignmentExpression.cpp
        src/datatype/TestDeliverable.cpp utils/EntityUtils.h
        src/component/SourceProcessor/Extractors/TestUsesExtractor.cpp src/component/SourceProcessor/Extractors/TestModifiesExtractor.cpp
        src/component/SourceProcessor/Extractors/TestFollowsTExtractor.cpp src/component/SourceProcessor/Extractors/TestParentTExtractor.cpp
        ../spa/src/exception/SyntaxException.h utils/EntityUtils.h src/util/TestCommonUtils.cpp src/model/TestEntity.cpp src/model/TestStatement.cpp
        src/datatype/TestConditionalExpression.cpp src/model/TestCFG.cpp src/component/SourceProcessor/TestParser.cpp
        src/component/SourceProcessor/Extractors/TestCallsTExtractor.cpp src/model/TestVariableMapping.cpp)

target_link_libraries(unit_testing spa)
SET(CMAKE_BUILD_TYPE Debug)
