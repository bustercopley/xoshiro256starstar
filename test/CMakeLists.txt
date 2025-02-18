cmake_minimum_required(VERSION 3.14)

project(xoshiro256starstarTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(xoshiro256starstar REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(xoshiro256starstar_test source/xoshiro256starstar_test.cpp)
target_link_libraries(xoshiro256starstar_test PRIVATE xoshiro256starstar::xoshiro256starstar)
target_compile_features(xoshiro256starstar_test PRIVATE cxx_std_20)

add_test(NAME xoshiro256starstar_test COMMAND xoshiro256starstar_test)

# ---- End-of-file commands ----

add_folders(Test)
