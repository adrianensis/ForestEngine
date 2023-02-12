# ----------------------------------------------------------------------------
# FOREST ENGINE
# ----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# project name
set(PROJECTNAME forest)
project(${PROJECTNAME} LANGUAGES CXX C)

# ----------------------------------------------------------------------------
# FUNCTIONS
# ----------------------------------------------------------------------------

function(build_programs path prefix)
  
  message("BUILD PROGRAMS (${prefix}): " ${path})

  file(GLOB PROGRAM_PATHS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${path}/*")

  message("${PROGRAM_PATHS}")

  foreach(PROGRAM_PATH ${PROGRAM_PATHS})

    file(GLOB_RECURSE PROGRAM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.cpp" "${PROGRAM_PATH}/*.hpp")
    file(GLOB_RECURSE GENERATED_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${GENERATED_CODE_FOLDER}/${PROGRAM_PATH}/*.cpp" "${GENERATED_CODE_FOLDER}/${PROGRAM_PATH}/*.hpp")

    include_directories("${PROGRAM_PATH}")

    get_filename_component(PROGRAM_NAME ${PROGRAM_PATH} NAME_WE) # NAME_WE = name with extension

    set(PROGRAM_FINAL_NAME "${prefix}_${PROGRAM_NAME}" )
    add_executable( ${PROGRAM_FINAL_NAME} ${PROGRAM_SOURCES} ${GENERATED_SOURCES} )
    target_link_libraries( ${PROGRAM_FINAL_NAME} ${PROJECTNAME})
    install(TARGETS ${PROGRAM_FINAL_NAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

  endforeach()
endfunction()

# ----------------------------------------------------------------------------
# CPP CONFIG
# ----------------------------------------------------------------------------
# set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 23)

# set(CMAKE_CXX_FLAGS_ALL "-std=c++17 -Wall") # this macro was invented by me
set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -Wall")

if(ENABLE_LOGS)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -D DE_ENABLE_LOGS")
endif(ENABLE_LOGS)

if(ENABLE_PROFILER)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -D BUILD_WITH_EASY_PROFILER")
endif(ENABLE_PROFILER)

if(UNIX)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} -Werror=return-type -fmodules-ts")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_ALL} -D DE_DEBUG -mavx -ftree-vectorize -ffast-math")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_ALL} -O3 -mavx")
endif(UNIX)

if(WIN32)
  set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS_ALL} /m /TP")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_ALL} -D DE_DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_ALL}")
endif(WIN32)

# ----------------------------------------------------------------------------
# OUTPUT DIRECTORIES
# ----------------------------------------------------------------------------

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/compiledLibs")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/binaries")
set(GENERATED_CODE_FOLDER "generated-code")

# ----------------------------------------------------------------------------

message("Compiler: ${CMAKE_CXX_COMPILER}")
message("Release: ${CMAKE_CXX_FLAGS_RELEASE}")
message("Debug: ${CMAKE_CXX_FLAGS_DEBUG}")

message("Selected Build target: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------------
# DEPENDENCIES
# ----------------------------------------------------------------------------

# third party libraries
set(DEPENDENCIES_DIR "dependencies")

# ZLib
include_directories("${DEPENDENCIES_DIR}/zlib-1.2.11")
add_subdirectory("${DEPENDENCIES_DIR}/zlib-1.2.11")

# # ZLib (libpng dependency)
# if(WIN32)
#   # Requires Zlib -> http://gnuwin32.sourceforge.net/packages/zlib.htm
#   set(ZLIB_ROOT "C:/Program Files (x86)/GnuWin32")
  
#   find_package(ZLIB REQUIRED)
  
#   if(ZLIB_FOUND)
#     message(FATAL_ERROR "ZLIB not found!!")
#   endif(ZLIB_FOUND)
# endif(WIN32)

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
include_directories("${DEPENDENCIES_DIR}/easy_profiler-2.1.0/easy_profiler_core/include")
add_subdirectory("${DEPENDENCIES_DIR}/easy_profiler-2.1.0")

# JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
include_directories("${DEPENDENCIES_DIR}/json-3.9.1/single_include/nlohmann")
add_subdirectory("${DEPENDENCIES_DIR}/json-3.9.1")

# libpng
# set(PNG_BUILD_ZLIB TRUE CACHE BOOL "" FORCE)
# set(PNG_SHARED OFF CACHE BOOL "" FORCE)
# set(PNG_STATIC ON CACHE BOOL "" FORCE)
# set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE) # we only want lib
# set(PNG_TESTS OFF CACHE BOOL "" FORCE) # we only want lib
# set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE) # we only want lib

# include_directories("${DEPENDENCIES_DIR}/libpng-1.6.37/build/include")
# link_directories("${DEPENDENCIES_DIR}/libpng-1.6.37/build/lib")
# add_subdirectory("${DEPENDENCIES_DIR}/libpng-1.6.37")

# SDL2_image
include_directories("${DEPENDENCIES_DIR}/stb")

# Duktape
# set(DUKTAPE_SOURCE "${DEPENDENCIES_DIR}/duktape-2.6.0/src/duktape.c")
# include_directories("${DEPENDENCIES_DIR}/duktape-2.6.0/src")

# Assimp
set(ASSIMP_LIBRARY "assimp")

SET(BUILD_SHARED_LIBS OFF)
SET(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(ASSIMP_NO_EXPORT TRUE CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

# Assmip importers
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MD5_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON CACHE BOOL "" FORCE)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/build/assimp-5.2.3/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${DEPENDENCIES_DIR}/assimp-5.2.3/include")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DEPENDENCIES_DIR}/assimp-5.2.3/cmake-modules")
add_subdirectory("${DEPENDENCIES_DIR}/assimp-5.2.3")

set_target_properties(${ASSIMP_LIBRARY} PROPERTIES IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.so")

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/build/${DEPENDENCIES_DIR}/assimp-5.2.3/revision.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${DEPENDENCIES_DIR}/assimp-5.2.3/revision.h
)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/build/${DEPENDENCIES_DIR}/assimp-5.2.3/include/assimp/config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${DEPENDENCIES_DIR}/assimp-5.2.3/include/assimp/config.h
)

# Pyhon
# find_package (Python3 COMPONENTS Interpreter Development)
# message("Python3 found: ${Python3_FOUND}")
# include_directories(${Python3_INCLUDE_DIRS})
# message(${Python3_INCLUDE_DIRS})
# message(${Python3_LIBRARIES})

# FreeType
#find_package(Freetype REQUIRED)
#include_directories(${FREETYPE_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# ENGINE PACKAGES
# ----------------------------------------------------------------------------

file(GLOB PACKAGES_PATHS "code/*")
set(PACKAGES "")

foreach(PACKAGE_PATH ${PACKAGES_PATHS})
  get_filename_component(PACKAGE_NAME ${PACKAGE_PATH} NAME_WE) # NAME_WE = name with extension
  list(APPEND PACKAGES ${PACKAGE_NAME})
endforeach()

# ----------------------------------------------------------------------------
# PACKAGES INCLUDE DIRECTORIES
# ----------------------------------------------------------------------------

include_directories("code")

# needed for generated code
include_directories(".")
include_directories("${GENERATED_CODE_FOLDER}")
# include_directories("generated-code/code")
#include_directories("generated-code/final")
#include_directories("generated-code/final/code")

# file(GLOB INCLUDE_PATHS "code/*")
#
# foreach(INCLUDE_PATH ${INCLUDE_PATHS})
#   get_filename_component(INCLUDE_NAME ${INCLUDE_PATH} NAME_WE) # NAME_WE = name with extension
#   include_directories("code/${INCLUDE_NAME}")
# endforeach()

# ----------------------------------------------------------------------------
# LIBRARY OBJECTS
# ----------------------------------------------------------------------------

# object libraries
# Alternative: CMakeLists.txt with this code into source directory.
# add_subdirectory(source)
foreach(PACKAGE ${PACKAGES})
  #file(GLOB_RECURSE PACKAGE_SOURCE "generated-code/final/code/${PACKAGE}/*")
  file(GLOB_RECURSE PACKAGE_SOURCE "code/${PACKAGE}/*")
  add_library(${PACKAGE} OBJECT ${PACKAGE_SOURCE})
endforeach()

# list of objects
foreach(PACKAGE ${PACKAGES})
  list(APPEND OBJS $<TARGET_OBJECTS:${PACKAGE}>)
endforeach()

# ----------------------------------------------------------------------------
# FOREST ENGINE LIBRARY
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
target_link_libraries(${PROJECTNAME} ${ASSIMP_LIBRARY})
target_link_libraries(${PROJECTNAME} ${ZLIB_LIBRARY} )
target_link_libraries(${PROJECTNAME} easy_profiler)

#target_link_libraries(${PROJECTNAME} ${FREETYPE_LIBRARIES})
#target_include_directories(${PROJECTNAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})
#target_link_libraries( ${PROJECTNAME} ${Python3_LIBRARIES})

# SOIL
#target_link_libraries( ${PROJECTNAME} SOIL )

# ----------------------------------------------------------------------------
# TESTS
# ----------------------------------------------------------------------------

if(BUILD_UNIT_TESTS)
  build_programs("test/unit" "test_unit")
endif(BUILD_UNIT_TESTS)

if(BUILD_INTEGRATION_TESTS)
  build_programs("test/integration" "test_integration")
endif(BUILD_INTEGRATION_TESTS)

# ----------------------------------------------------------------------------
# TOOLS
# ----------------------------------------------------------------------------

if(BUILD_TOOLS)
  build_programs("tools" "tool")
endif(BUILD_TOOLS)
# ----------------------------------------------------------------------------
# GAMES
# ----------------------------------------------------------------------------

build_programs("games" "games")