cmake_minimum_required(VERSION 3.6)
project(InnoMetricsSystem)


set(SOURCE_FILES main.cpp)
add_executable(innometrics_run ${SOURCE_FILES})

include_directories(InnoMetricsSources)

add_subdirectory(InnoMetricsSources)
add_subdirectory(InnoMetricsTests)

target_link_libraries(innometrics_run innometrics_core)