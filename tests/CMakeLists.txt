 cmake_minimum_required(VERSION 3.11)
project(NileTest CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(NILE_DIR "..")
set(NILE_TEST_DIR "${NILE_DIR}/tests")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(2d                 REGULAR_EXPRESSION test/2d/*)
source_group(core               REGULAR_EXPRESSION test/core/*)
source_group(game               REGULAR_EXPRESSION test/game/*)
source_group(renderer           REGULAR_EXPRESSION test/renderer/*)
source_group(resource           REGULAR_EXPRESSION test/resoure/*)

add_executable(NileTest
  ${NILE_TEST_DIR}/main.cc
  )
  
target_include_directories(NileTest PRIVATE
  ${NILE_DIR}/include
  ${NILE_DIR}/external/catch2/single_include/catch2
)

target_compile_definitions(NileTest PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG;DEBUG=1>
  $<$<CONFIG:RELEASE>:NDEBUG>

  # On Windows
  $<$<PLATFORM_ID:Windows>:
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  >
)

target_compile_options(NileTest PRIVATE
  # Clang
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wcomma
    -Wextra-semi
    -Wmost
    -Wmove
    -Wnon-virtual-dtor
    -Wimplicit-fallthrough
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    -Wimplicit-fallthrough
  >
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wall
    -Werror
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /WX
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)
