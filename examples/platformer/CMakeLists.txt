cmake_minimum_required(VERSION 3.11)
project(Platformer CXX)

set(PRODUCT_NAME Platformer)
set(NILE_DIR "../..")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_subdirectory(${NILE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nile_build)

source_group(src REGULAR_EXPRESSION "source/*")

add_executable(${PRODUCT_NAME}
 src/main.cc
  )

target_include_directories(${PRODUCT_NAME} PRIVATE
  ${NILE_DIR}/include)

target_link_libraries(${PRODUCT_NAME} nile_static)
