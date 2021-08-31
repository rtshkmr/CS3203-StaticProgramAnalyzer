file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} src/datatype/Identifier.cpp src/datatype/Identifier.h src/datatype/Deliverable.cpp src/datatype/Deliverable.h src/component/SourceProcessor/SourceProcessor.cpp src/component/SourceProcessor/SourceProcessor.h src/component/SourceProcessor/Parser.cpp src/component/SourceProcessor/Parser.h src/component/SourceProcessor/PSubsystem.cpp src/component/SourceProcessor/PSubsystem.h src/component/SourceProcessor/DesignExtractor.cpp src/component/SourceProcessor/DesignExtractor.h)
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


