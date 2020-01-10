cmake_minimum_required(VERSION 3.11)
project(Nile VERSION 0.0.0.1 LANGUAGES CXX)

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
source_group(application      REGULAR_EXPRESSION "(include/Nile|src)/application/*")
source_group(platform         REGULAR_EXPRESSION "(include/Nile|src)/platform/*")
source_group(asset            REGULAR_EXPRESSION "(include/Nile|src)/asset/*")

set(NILE_SOURCES_CORE
  ${NILE_DIR}/include/Nile/2d/2d_camera.hh
  ${NILE_DIR}/include/Nile/core/input_manager.hh
  ${NILE_DIR}/include/Nile/core/helper.hh
  ${NILE_DIR}/include/Nile/core/settings.hh
  ${NILE_DIR}/include/Nile/core/signal.hh
  ${NILE_DIR}/include/Nile/core/reference.hh
  ${NILE_DIR}/include/Nile/core/types.hh
  ${NILE_DIR}/include/Nile/core/assert.hh
  ${NILE_DIR}/include/Nile/core/camera_system.hh
  ${NILE_DIR}/include/Nile/core/file_system.hh
  ${NILE_DIR}/include/Nile/core/timer.hh
  ${NILE_DIR}/include/Nile/core/error_code.hh
  ${NILE_DIR}/include/Nile/renderer/base_renderer.hh
  ${NILE_DIR}/include/Nile/renderer/opengl_renderer.hh
  ${NILE_DIR}/include/Nile/renderer/texture2d.hh
  ${NILE_DIR}/include/Nile/renderer/rendering_system.hh
  ${NILE_DIR}/include/Nile/renderer/sprite_rendering_system.hh
  ${NILE_DIR}/include/Nile/renderer/font_rendering_system.hh
  ${NILE_DIR}/include/Nile/renderer/render_primitive_system.hh
  ${NILE_DIR}/include/Nile/renderer/model.hh
  ${NILE_DIR}/include/Nile/renderer/mesh.hh
  ${NILE_DIR}/include/Nile/renderer/opengl_framebuffer.hh
  ${NILE_DIR}/include/Nile/utils/vertex.hh
  ${NILE_DIR}/include/Nile/utils/font_character.hh
  ${NILE_DIR}/include/Nile/utils/string_utils.hh
  ${NILE_DIR}/include/Nile/application/game.hh
  ${NILE_DIR}/include/Nile/platform/game_host.hh
  ${NILE_DIR}/include/Nile/platform/x11/bootstrap.hh
  ${NILE_DIR}/include/Nile/platform/x11/game_host_x11.hh
  ${NILE_DIR}/include/Nile/asset/asset_manager.hh
  ${NILE_DIR}/include/Nile/asset/asset_container.hh
  ${NILE_DIR}/include/Nile/asset/asset.hh
  ${NILE_DIR}/include/Nile/asset/builder/asset_builder.hh
  ${NILE_DIR}/include/Nile/asset/builder/shaderset_builder.hh
  ${NILE_DIR}/include/Nile/asset/builder/model_builder.hh
  ${NILE_DIR}/include/Nile/asset/subsystem/asset_loader.hh
  ${NILE_DIR}/include/Nile/asset/subsystem/texture_loader.hh
  ${NILE_DIR}/include/Nile/asset/subsystem/font_loader.hh
  ${NILE_DIR}/include/Nile/renderer/shaderset.hh
  ${NILE_DIR}/include/Nile/ecs/components/renderable.hh
  ${NILE_DIR}/include/Nile/ecs/components/transform.hh
  ${NILE_DIR}/include/Nile/ecs/components/sprite.hh
  ${NILE_DIR}/include/Nile/ecs/components/camera_component.hh
  ${NILE_DIR}/include/Nile/ecs/components/primitive.hh
  ${NILE_DIR}/include/Nile/ecs/components/mesh_component.hh
  ${NILE_DIR}/include/Nile/ecs/components/relationship.hh
  ${NILE_DIR}/include/Nile/ecs/component_manager.hh
  ${NILE_DIR}/include/Nile/ecs/component_storage.hh
  ${NILE_DIR}/include/Nile/ecs/ecs_coordinator.hh
  ${NILE_DIR}/include/Nile/ecs/ecs_system.hh
  ${NILE_DIR}/include/Nile/ecs/ecs_system_manager.hh
  ${NILE_DIR}/include/Nile/ecs/entity_manager.hh
  ${NILE_DIR}/include/Nile/ecs/handle.hh
  ${NILE_DIR}/include/Nile/ecs/handle_manager.hh
  ${NILE_DIR}/include/Nile/ecs/ecs_field_detection.hh
  ${NILE_DIR}/include/Nile/math/utils.hh
  ${NILE_DIR}/include/Nile/log/file_logger.hh
  ${NILE_DIR}/include/Nile/log/log_type.hh
  ${NILE_DIR}/include/Nile/log/log.hh
  ${NILE_DIR}/include/Nile/log/stream_logger.hh
  ${NILE_DIR}/include/Nile/scene/scene_graph.hh
  ${NILE_DIR}/include/Nile/scene/scene_node.hh
  ${NILE_DIR}/include/Nile/debug/benchmark_timer.hh
  ${NILE_DIR}/include/Nile/graphics/rendering_device.hh
  ${NILE_DIR}/include/Nile/drivers/vulkan/vulkan_rendering_device.hh
  ${NILE_DIR}/include/Nile/drivers/vulkan/vulkan_utils.hh
  ${NILE_DIR}/src/2d/2d_camera.cc
  ${NILE_DIR}/src/core/input_manager.cc
  ${NILE_DIR}/src/core/settings.cc
  ${NILE_DIR}/src/core/signal.cc
  ${NILE_DIR}/src/core/camera_system.cc
  ${NILE_DIR}/src/core/file_system.cc
  ${NILE_DIR}/src/core/timer.cc
  ${NILE_DIR}/src/core/error_code.cc
  ${NILE_DIR}/src/renderer/base_renderer.cc
  ${NILE_DIR}/src/renderer/opengl_renderer.cc
  ${NILE_DIR}/src/renderer/texture2d.cc
  ${NILE_DIR}/src/renderer/rendering_system.cc
  ${NILE_DIR}/src/renderer/sprite_rendering_system.cc
  ${NILE_DIR}/src/renderer/font_rendering_system.cc
  ${NILE_DIR}/src/renderer/render_primitive_system.cc
  ${NILE_DIR}/src/renderer/opengl_framebuffer.cc
  ${NILE_DIR}/src/application/game.cc
  ${NILE_DIR}/src/platform.x11/bootstrap.cc
  ${NILE_DIR}/src/platform.x11/game_host_x11.cc
  ${NILE_DIR}/src/asset/builder/shaderset_builder.cc
  ${NILE_DIR}/src/asset/builder/model_builder.cc
  ${NILE_DIR}/src/asset/subsystem/asset_loader.cc
  ${NILE_DIR}/src/asset/subsystem/texture_loader.cc
  ${NILE_DIR}/src/asset/subsystem/font_loader.cc
  ${NILE_DIR}/src/asset/asset_container.cc
  ${NILE_DIR}/src/asset/asset_manager.cc
  ${NILE_DIR}/src/renderer/shaderset.cc
  ${NILE_DIR}/src/ecs/component_storage.cc
  ${NILE_DIR}/src/ecs/ecs_coordinator.cc
  ${NILE_DIR}/src/ecs/ecs_system_manager.cc
  ${NILE_DIR}/src/ecs/entity_manager.cc
  ${NILE_DIR}/src/ecs/handle_manager.cc
  ${NILE_DIR}/src/math/utils.cc
  ${NILE_DIR}/src/log/log.cc
  ${NILE_DIR}/src/log/file_logger.cc
  ${NILE_DIR}/src/log/stream_logger.cc
  ${NILE_DIR}/src/scene/scene_graph.cc
  ${NILE_DIR}/src/scene/scene_node.cc
  ${NILE_DIR}/src/graphics/rendering_device.cc
  ${NILE_DIR}/src/drivers/vulkan/vulkan_rendering_device.cc
  ${NILE_DIR}/src/drivers/vulkan/vulkan_utils.cc
  )

set(NILE_SOURCE_EXPERIMENTAL 
  ${NILE_DIR}/include/Nile/experimental/asset/asset_manager_helper.hh
  ${NILE_DIR}/src/experimental/asset/asset_manager_helper.cc
  ${NILE_DIR}/include/Nile/experimental/core/ts_unordered_map.hh
  ${NILE_DIR}/src/experimental/core/ts_unordered_map.cc
  )

set(SOURCE_FILES ${NILE_SOURCES_CORE} ${NILE_SOURCE_EXPERIMENTAL})



add_subdirectory(external/freetype)
add_subdirectory(external/assimp)

# FreeType package
find_package(Freetype REQUIRED)
if (Freetype_FOUND)
  message(STATUS ${Freetype_LIBRARIES} "Freetype Library has been found!")
  else()
  message(FATAL_ERROR "Cannot find FreeType Library!")
endif()
#include_directories(${FREETYPE_INCLUDE_DIRS})

# SDL2 package
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
  message(STATUS ${SDL2_LIBRARY} " SDL2 Library has been found!")
  include_directories(${SDL2_INCLUDE_DIR})
else()
  message(FATAL_ERROR "Cannot find SDL2 Library!")
endif()

# GLEW package
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
  message(STATUS ${GLEW_LIBRARIES} " GLEW Library has been found!")
  include_directories(${GLEW_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Cannot find GLEW Library!")
endif()

# Vulkan
find_package(Vulkan)
if (Vulkan_FOUND)
  message(STATUS ${Vulkan_LIBRARY} " Vulkan library has been found!")
ELSE()
  message(FATAL_ERROR "Cannot find vulkan library!")
ENDIF()

# OpenGL package
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
  message(STATUS ${OPENGL_LIBRARY} " OpenGL Library has been found!")
  include_directories(${OPENGL_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Cannot find OpenGL Library!")
endif()

# Catch2 testing framework
add_subdirectory(external/catch2)

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

# File configurations generated by CMake
configure_file(
  "${PROJECT_SOURCE_DIR}/include/project_config.h.in"
  "${PROJECT_BINARY_DIR}/project_config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")


set(ASSIMP_INCLUDE_DIRS "external/assimp/include")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

list(SORT SOURCE_FILES)

# NOTE: Static library
add_library(nile_static STATIC ${SOURCE_FILES})

if(UNIX)
  # On Unix platforms, the library is called `libnile.a`.
  set_target_properties(nile_static PROPERTIES OUTPUT_NAME nile)
endif()


target_include_directories(nile_static PRIVATE
  ${NILE_DIR}/include
  ${NILE_DIR}/lib
  ${FREETYPE_INCUDE_DIRS}
  ${NILE_DIR}/external/stb
  Vulkan::Vulkan
)


target_compile_definitions(nile_static PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG;DEBUG=1>
  $<$<CONFIG:RELEASE>:NDEBUG>
)

target_compile_options(nile_static PRIVATE
  # Clang
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wcomma
    -Wextra-semi
    #    -Wimplicit-fallthrough
    -Wmost
    -Wmove
    -Wnewline-eof

    # FIXME: Use -Wextra instead of the following options.
    -Wignored-qualifiers
    -Winitializer-overrides
    -Wmissing-field-initializers
    -Wmissing-method-return-type
    -Wsemicolon-before-method-body
    -Wsign-compare
    # -Wunused-parameter
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    -pedantic

    # FIXME: Use -Wextra instead of the following options.
    -Wclobbered
    -Wcast-function-type
    -Wempty-body
    -Wignored-qualifiers
    -Wimplicit-fallthrough=3
    -Wmissing-field-initializers
    # -Wtype-limits
    -Wuninitialized
    -Wshift-negative-value
  >
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wall
    -Werror
    -Wnon-virtual-dtor
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    # NOTE: See https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
    /W4
    /WX
    /we4263
    /we4264
    /we4266
    /we4287
    /we4289
    /we4412
    /we4431
    /we4686
    /we4928
    /we4987
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)

add_compile_definitions(TOOLS_ENABLED)

if(NILE_TARGET_PLATFORM MATCHES "Linux")
  target_link_libraries(nile_static INTERFACE
    pthread
  )
endif()

target_include_directories(nile_static PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(nile_static PUBLIC ${ASSIMP_INCLUDE_DIRS})


target_link_libraries(nile_static PUBLIC ${SDL2_LIBRARY})
target_link_libraries(nile_static INTERFACE ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES}
  Vulkan::Vulkan)
target_link_libraries(nile_static PUBLIC ${FREETYPE_LIBRARIES} assimp )

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
