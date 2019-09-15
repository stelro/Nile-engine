cmake_minimum_required(VERSION 3.11)
project(Nile CXX)

set(NILE_DIR ".")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

source_group(2d               REGULAR_EXPRESSION "(include/Nile|src)/2d/*")
source_group(core             REGULAR_EXPRESSION "(include/Nile|src)/core/*")
source_group(game             REGULAR_EXPRESSION "(include/Nile|src)/game/*")
source_group(renderer         REGULAR_EXPRESSION "(include/Nile|src)/renderer/*")
source_group(resource         REGULAR_EXPRESSION "(include/Nile|src)/resource/*")

set(NILE_SOURCES_CORE
  ${NILE_DIR}/include/Nile/2d/2d_camera.hh
  ${NILE_DIR}/include/Nile/2d/sprite_renderer.hh
  ${NILE_DIR}/include/Nile/2d/sprite_sheet.hh
  ${NILE_DIR}/include/Nile/2d/spritesheet_context.hh
  ${NILE_DIR}/include/Nile/core/engine.hh
  ${NILE_DIR}/include/Nile/core/input_manager.hh
  ${NILE_DIR}/include/Nile/core/logger.hh
  ${NILE_DIR}/include/Nile/core/nile.hh
  ${NILE_DIR}/include/Nile/core/settings.hh
  ${NILE_DIR}/include/Nile/core/signal.hh
  ${NILE_DIR}/include/Nile/game/game.hh
  ${NILE_DIR}/include/Nile/renderer/base_renderer.hh
  ${NILE_DIR}/include/Nile/renderer/font_renderer.hh
  ${NILE_DIR}/include/Nile/renderer/opengl_renderer.hh
  ${NILE_DIR}/include/Nile/renderer/shader.hh
  ${NILE_DIR}/include/Nile/renderer/texture2d.hh
  ${NILE_DIR}/include/Nile/resource/resource_manager.hh
  ${NILE_DIR}/src/2d/sprite_renderer.cc
  ${NILE_DIR}/src/2d/sprite_sheet.cc
  ${NILE_DIR}/src/2d/spritesheet_context.cc
  ${NILE_DIR}/src/core/engine.cc
  ${NILE_DIR}/src/core/input_manager.cc
  ${NILE_DIR}/src/core/logger.cc
  ${NILE_DIR}/src/core/settings.cc
  ${NILE_DIR}/src/core/signal.cc
  ${NILE_DIR}/src/game/game.cc
  ${NILE_DIR}/src/renderer/base_renderer.cc
  ${NILE_DIR}/src/renderer/font_renderer.cc
  ${NILE_DIR}/src/renderer/opengl_renderer.cc
  ${NILE_DIR}/src/renderer/shader.cc
  ${NILE_DIR}/src/renderer/texture2d.cc
  ${NILE_DIR}/src/resource/resource_manager.cc
  )

set(SOURCE_FILES ${NILE_SOURCES_CORE})

# FreeType package
find_package(Freetype REQUIRED)
if (Freetype_FOUND)
  message(STATUS ${Freetype_LIBRARY} "Freetype Library has been found!")
  include_directories(${FREETYPE_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Cannot find FreeType Library!")
endif()

if(APPLE)
  set(NILE_TARGET_PLATFORM "Mac"   CACHE STRING "Choose platform")
  set(NILE_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(NILE_TARGET_PLATFORM "Linux" CACHE STRING "Choose platform")
  set(NILE_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
elseif(WIN32)
  set(NILE_TARGET_PLATFORM "Win32" CACHE STRING "Choose platform")
  set(NILE_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
endif()


list(SORT SOURCE_FILES)

# NOTE: Static library
add_library(nile_static STATIC ${SOURCE_FILES})

if(UNIX)
  # On Unix platforms, the library is called `libnile.a`.
  set_target_properties(nile_static PROPERTIES OUTPUT_NAME nile)
endif()

target_include_directories(nile_static PRIVATE
  ${NILE_DIR}/include
  ${FREETYPE_INCUDE_DIRS}
)

target_link_libraries(nile_static INTERFACE ${FREETYPE_LIBRARIES})
