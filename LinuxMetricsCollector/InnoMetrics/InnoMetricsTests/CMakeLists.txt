cmake_minimum_required(VERSION 3.6)
project(InnoMetricsTests)


#include(CTest)
#enable_testing()

add_subdirectory(lib/googletest-master)

include_directories("lib/googletest-master/googletest")
include_directories("lib/googletest-master/googlemock")

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(runUnitTests gtest.cpp Tests/basic_check.cpp Tests/helper_check.cpp)

target_link_libraries(runUnitTests gtest gmock innometrics_core)
#include_directories("lib/googletest-master/googletest")
#include_directories("lib/googletest-master/googlemock")