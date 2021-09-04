file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(unit_testing ${srcs} src/component/SourceProcessor/TestTokenizer.cpp src/component/SourceProcessor/TestEntityFactory.cpp)

target_link_libraries(unit_testing spa)

