cmake_minimum_required(VERSION 3.14)
project(persistent_cp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(segments_above INTERFACE)
target_include_directories(segments_above INTERFACE include)

add_executable(solution main.cpp)
target_link_libraries(solution PRIVATE segments_above)

add_executable(naive naive.cpp)

add_executable(bench bench.cpp)
target_link_libraries(bench PRIVATE segments_above)

add_executable(solution_contest solution_contest.cpp)
