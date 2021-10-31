file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(integration_testing ${srcs} src/components/TestSourceProcToPKB.cpp src/components/TestPKBToPQL.cpp src/components/TestBackendBips.cpp util/TestUtil.h)


target_link_libraries(integration_testing spa)
SET(CMAKE_BUILD_TYPE Debug)
