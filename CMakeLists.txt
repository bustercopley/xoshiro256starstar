cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    xoshiro256starstar
    VERSION 0.1.0
    DESCRIPTION "C++20 std::uniform_random_bit_generator using xoshiro256**"
    HOMEPAGE_URL "https://github.com/bustercopley/xoshiro256starstar"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(xoshiro256starstar_xoshiro256starstar INTERFACE)
add_library(xoshiro256starstar::xoshiro256starstar ALIAS xoshiro256starstar_xoshiro256starstar)

set_property(
    TARGET xoshiro256starstar_xoshiro256starstar PROPERTY
    EXPORT_NAME xoshiro256starstar
)

target_include_directories(
    xoshiro256starstar_xoshiro256starstar ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(xoshiro256starstar_xoshiro256starstar INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT xoshiro256starstar_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of xoshiro256starstar"
  )
endif()

include(cmake/dev-mode.cmake)
