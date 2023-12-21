cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# project name
set(PROJECTNAME forest)
project(${PROJECTNAME} LANGUAGES CXX C)

# ----------------------------------------------------------------------------
# FUNCTIONS
# ----------------------------------------------------------------------------

function(build_program path prefix program_name)

  set(PROGRAM_PATH "${path}/${program_name}")
  message("BUILD PROGRAM ${PROGRAM_PATH}")

  file(GLOB_RECURSE PROGRAM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.cpp")

  include_directories("${PROGRAM_PATH}")

#   get_filename_component(PROGRAM_NAME ${PROGRAM_PATH} NAME_WE) # NAME_WE = name with extension

  set(PROGRAM_FINAL_NAME "${prefix}_${program_name}" )
  add_executable( ${PROGRAM_FINAL_NAME} ${PROGRAM_SOURCES})
  target_link_libraries( ${PROGRAM_FINAL_NAME} ${PROJECTNAME})
  install(TARGETS ${PROGRAM_FINAL_NAME})

endfunction()

# ----------------------------------------------------------------------------
# CPP CONFIG
# ----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(ENABLE_LOGS)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -D ENGINE_ENABLE_LOGS")
endif(ENABLE_LOGS)

if(ENABLE_GPU_DEBUG)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -D ENGINE_ENABLE_GPU_DEBUG")
endif(ENABLE_GPU_DEBUG)

if(ENABLE_PROFILER)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -D BUILD_WITH_EASY_PROFILER")
endif(ENABLE_PROFILER)

if(UNIX)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -mavx -Wall -Werror=return-type -Wcast-align -Wcast-qual -Wpointer-arith -Wfloat-equal")
  
  if(ENABLE_ADDRESS_SANITIZER)
    set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -fsanitize=address -static-libasan")
  endif(ENABLE_ADDRESS_SANITIZER)

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_ALL} -D ENGINE_DEBUG -ftree-vectorize")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_ALL} -O3")
endif(UNIX)

if(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_ALL} -D ENGINE_DEBUG /m /TP")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif(WIN32)

message("Compiler: ${CMAKE_CXX_COMPILER}")
message("Release: ${CMAKE_CXX_FLAGS_RELEASE}")
message("Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message("Selected Build target: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------------
# OUTPUT DIRECTORIES
# ----------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/binaries/${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------------
# DEPENDENCIES
# ----------------------------------------------------------------------------

# third party libraries
set(DEPENDENCIES_DIR "dependencies")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # see : http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${DEPENDENCIES_DIR}/glfw-3.3.4")
include_directories("${DEPENDENCIES_DIR}/glfw-3.3.4/include")

# glad
set(GLAD_SOURCE "${DEPENDENCIES_DIR}/glad/src/glad.c")
include_directories("${DEPENDENCIES_DIR}/glad/include")

# Easy Profiler
if(ENABLE_PROFILER)
    set(EASY_PROFILER_NO_GUI ON CACHE BOOL "")
    set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")
    include_directories("${DEPENDENCIES_DIR}/easy_profiler-2.1.0/easy_profiler_core/include")
    add_subdirectory("${DEPENDENCIES_DIR}/easy_profiler-2.1.0")
endif(ENABLE_PROFILER)

# JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
include_directories("${DEPENDENCIES_DIR}/json-3.9.1/single_include/nlohmann")
add_subdirectory("${DEPENDENCIES_DIR}/json-3.9.1")

# stb
include_directories("${DEPENDENCIES_DIR}/stb")

# freetype
include_directories("${DEPENDENCIES_DIR}/freetype-2.10.1/include")

# cgltf
include_directories("${DEPENDENCIES_DIR}/cgltf-1.13")

# ----------------------------------------------------------------------------
# PROJECT PACKAGES
# ----------------------------------------------------------------------------

file(GLOB PACKAGES_PATHS "code/*")
set(PACKAGES "")

foreach(PACKAGE_PATH ${PACKAGES_PATHS})
  get_filename_component(PACKAGE_NAME ${PACKAGE_PATH} NAME_WE) # NAME_WE = name with extension
  list(APPEND PACKAGES ${PACKAGE_NAME})
endforeach()

# include source code root
include_directories("code")

# object libraries
# Alternative: CMakeLists.txt with this code into source directory.
# add_subdirectory(source)
foreach(PACKAGE ${PACKAGES})
  file(GLOB_RECURSE PACKAGE_SOURCE "code/${PACKAGE}/*.cpp")
  add_library(${PACKAGE} OBJECT ${PACKAGE_SOURCE})
endforeach()

# list of objects
foreach(PACKAGE ${PACKAGES})
  list(APPEND OBJS $<TARGET_OBJECTS:${PACKAGE}>)
endforeach()

# ----------------------------------------------------------------------------
# DEFINE LIBRARY
# ----------------------------------------------------------------------------

# single library
add_library(${PROJECTNAME} ${OBJS} ${GLAD_SOURCE})

# ----------------------------------------------------------------------------
# LINK LIBRARIES
# ----------------------------------------------------------------------------

# link libraries with forest library
target_link_libraries(${PROJECTNAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECTNAME} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECTNAME} glfw)
target_link_libraries(${PROJECTNAME} nlohmann_json)
target_link_libraries(${PROJECTNAME} libpng.so)
target_link_libraries(${PROJECTNAME} freetype)
if(ENABLE_PROFILER)
    target_link_libraries(${PROJECTNAME} easy_profiler)
endif(ENABLE_PROFILER)

# ----------------------------------------------------------------------------
# TESTS
# ----------------------------------------------------------------------------

if(BUILD_UNIT_TESTS)
    set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/unit)
    file(GLOB TESTS RELATIVE ${TEST_DIR} ${TEST_DIR}/*)
    FOREACH(TEST ${TESTS})
        build_program("test/unit" "test_unit" ${TEST})
    ENDFOREACH()
endif(BUILD_UNIT_TESTS)

if(BUILD_INTEGRATION_TESTS)
    set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/integration)
    file(GLOB TESTS RELATIVE ${TEST_DIR} ${TEST_DIR}/*)
    FOREACH(TEST ${TESTS})
        build_program("test/integration" "test_integration" ${TEST})
    ENDFOREACH()
endif(BUILD_INTEGRATION_TESTS)

# ----------------------------------------------------------------------------
# TOOLS
# ----------------------------------------------------------------------------

message("Tools to build: ${TOOLS_TO_BUILD}")
FOREACH(TOOL ${TOOLS_TO_BUILD})
    build_program("tools" "tool" ${TOOL})
ENDFOREACH()
# ----------------------------------------------------------------------------
# GAMES
# ----------------------------------------------------------------------------

message("Apps to build: ${APPS_TO_BUILD}")
FOREACH(APP ${APPS_TO_BUILD})
    build_program("apps" "app" ${APP})
ENDFOREACH()