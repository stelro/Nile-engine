cmake_minimum_required(VERSION 3.11)
project(Platformer CXX)

set(PRODUCT_NAME Platformer)
set(NILE_DIR "../..")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(src REGULAR_EXPRESSION "source/*")

add_executable(${PRODUCT_NAME}
 app/main.cc
 src/platformer.hh
 src/platformer.cc
  )

target_include_directories(${PRODUCT_NAME} PRIVATE
  ${NILE_DIR}/include)

target_compile_definitions(${PRODUCT_NAME} PRIVATE
  $<$<CONFIG:DEBUG>:DEBUG>
  $<$<CONFIG:RELEASE>:NDEBUG>

  # On Windows
  $<$<PLATFORM_ID:Windows>:
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  >
)

target_compile_options(${PRODUCT_NAME} PRIVATE
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -pedantic
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Copying 'Content' directory into shippable directory after building executable
  add_custom_command(
    COMMENT "Copying contents to ${CMAKE_BINARY_DIR}/assets"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/assets
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/assets
      ${CMAKE_CURRENT_BINARY_DIR}/assets
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets
  )
endif()

if (NOT NILE_BUILD_TARGET_ALL) 
  add_subdirectory(${NILE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nile_build)
endif()

target_link_libraries(${PRODUCT_NAME} nile_static )
