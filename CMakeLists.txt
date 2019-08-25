#This file specifies how the project should be built, using CMake.
#If you are unfamiliar with CMake, don't worry about all the details.
#The sections you might want to edit are marked as such, and
#the comments should hopefully make most of it clear.
#
#For many purposes, you may not need to change anything about this file.

cmake_minimum_required(VERSION 3.8.2)

#Set project name, version and laguages here.( change as needed )
#Version numbers are available by including "exampleConfig.h" in
#the source.See exampleConfig.h.in for some more details.
project(nile VERSION 0.0.0.1 LANGUAGES CXX)

#Options : Things you can set via commandline options to cmake( e.g.- DENABLE_LTO = [ON | OFF] )
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code.
                        Tests in tests/*.cpp will still be enabled." ON)

# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Documentation)
include(LTO)
include(Misc)
include(Warnings)
include_directories(external)
include_directories(external/glm)
include_directories(external/stb)
add_subdirectory(external/glfw-3.3)
add_subdirectory(external/glad)

#Source Files
include(src/core/CMakeLists.txt)
include(src/renderer/CMakeLists.txt)

set(NILE_SRC
  ${NILE_CORE_SRC}
  ${NILE_RENDERER_SRC}
  )

# --------------------------------------------------------------------------------
#                         Add Shaders folder
# --------------------------------------------------------------------------------
set(SHADER_DIR shaders)
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.glsl" "${SHADER_DIR}/*.tesc" "${SHADER_DIR}/*.tese")
source_group("Shaders" FILES ${SHADERS})
 

# Check for LTO support.
find_lto(CXX)
# --------------------------------------------------------------------------------
#                         Locate files (change as needed).
# --------------------------------------------------------------------------------
# set(SOURCES          # All .cpp files in src/
#   ${NILE_SRC}
# )
set(TESTFILES        # All .cpp files in tests/
    tests/main.cpp
)
set(LIBRARY_NAME engine)  # Default name for the library built from src/*.cpp (change if you wish)

# find packages needed by this project
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

IF (SDL2_FOUND)
  message(STATUS ${SDL2_LIBRARY})
ELSE()
  message(FATAL_ERROR "Could not find SDL2 library")
ENDIF()


IF (SDL2_MIXER_FOUND)
  message(STATUS ${SDL2_MIXER_LIBRARIES}) 
ELSE()
  message(FATAL_ERROR "Could not find SDL2_MIXER library")
ENDIF()


# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library.
add_library(${LIBRARY_NAME} SHARED ${NILE_SRC})

# Lib needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(engine PRIVATE external/glfw-3.3/include ${GLEW_INCLUDE_DIRS})

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${LIBRARY_NAME} PUBLIC doctest ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES} glfw
  glad GLEW)

# Set the compile options you want (change as needed).
target_set_warnings(${LIBRARY_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
# target_compile_options(${LIBRARY_NAME} ... )  # For setting manually.

# Add an executable for the file app/main.cpp.
# If you add more executables, copy these lines accordingly.
add_executable(main app/main.cpp)   # Name of exec. and location of file.
target_link_libraries(main PRIVATE ${LIBRARY_NAME})  # Link the executable to library (if it uses it).
target_set_warnings(main ENABLE ALL AS_ERROR ALL DISABLE Annoying) # Set warnings (if needed).
target_enable_lto(main optimized)  # enable link-time-optimization if available for non-debug configurations

# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main (change as needed).
set_target_properties(
    ${LIBRARY_NAME} main 
      PROPERTIES 
        CXX_STANDARD 17 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)
set(${PROJECT_NAME}_DEFINITIONS CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)

# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

# Set up tests (see tests/CMakeLists.txt).
add_subdirectory(tests)
