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

source_group(ecs                 REGULAR_EXPRESSION test/ecs/*)

add_executable(NileTest
  ${NILE_TEST_DIR}/main.cc
  ${NILE_TEST_DIR}/ecs/entity_manager.test.cc
  ${NILE_TEST_DIR}/ecs/entity.test.cc
  ${NILE_TEST_DIR}/ecs/relationship.test.cc
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

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


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

if (NOT NILE_BUILD_TARGET_ALL) 
  add_subdirectory(${NILE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nile_build)
endif()

target_link_libraries(NileTest nile_static )


