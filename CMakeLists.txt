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

# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -header-filter=.;
#   -checks=*;
# # -warnings-as-errors=*;
# )

if(ENABLE_LOGS)
    set(PROJECT_COMMON_CXX_FLAGS "${PROJECT_COMMON_CXX_FLAGS} -D ENGINE_ENABLE_LOGS")
endif(ENABLE_LOGS)

if(ENABLE_GPU_DEBUG)
    set(PROJECT_COMMON_CXX_FLAGS "${PROJECT_COMMON_CXX_FLAGS} -D ENGINE_ENABLE_GPU_DEBUG")
endif(ENABLE_GPU_DEBUG)

if(ENABLE_PROFILER)
    set(PROJECT_COMMON_CXX_FLAGS "${PROJECT_COMMON_CXX_FLAGS} -D TRACY_ENABLE")
endif(ENABLE_PROFILER)

if(UNIX)
    set(PROJECT_COMMON_CXX_FLAGS "${PROJECT_COMMON_CXX_FLAGS} -mavx -Wall -Werror=return-type -Wcast-align -Wcast-qual -Wpointer-arith -Wfloat-equal -flto=auto")
  
    if(ENABLE_ADDRESS_SANITIZER)
        set(PROJECT_COMMON_CXX_FLAGS "${PROJECT_COMMON_CXX_FLAGS} -fsanitize=address -static-libasan")
    endif(ENABLE_ADDRESS_SANITIZER)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_COMMON_CXX_FLAGS} -D ENGINE_DEBUG -ftree-vectorize")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${PROJECT_COMMON_CXX_FLAGS} -O3")
endif(UNIX)

if(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_COMMON_CXX_FLAGS} -D ENGINE_DEBUG /m /TP")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif(WIN32)

# find_program(CCACHE_PROGRAM ccache)
# if(CCACHE_PROGRAM)
#     set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
#     set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}") # CMake 3.9+
#     message("Using CCACHE")
# else()
#     message("Not Using CCACHE")
# endif()

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
include_directories("${DEPENDENCIES_DIR}")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # see : http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${DEPENDENCIES_DIR}/glfw-3.3.4")
include_directories("${DEPENDENCIES_DIR}/glfw-3.3.4/include")

# glew
add_compile_definitions(GLEW_STATIC)
add_subdirectory("${DEPENDENCIES_DIR}/glew-2.2.0/build/cmake")
include_directories("${DEPENDENCIES_DIR}/glew-2.2.0/include")

if(ENABLE_PROFILER)
    # Tracy
    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
    set(LEGACY ON CACHE BOOL "" FORCE)
    set(DOWNLOAD_GLFW OFF CACHE BOOL "" FORCE)
    set(DOWNLOAD_CAPSTONE OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    include_directories("${DEPENDENCIES_DIR}/tracy-0.11.0/public")
    add_subdirectory("${DEPENDENCIES_DIR}/tracy-0.11.0")
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

# TBB
find_package(TBB REQUIRED)

# ----------------------------------------------------------------------------
# PROJECT PACKAGES
# ----------------------------------------------------------------------------

# include source code root
include_directories("code")

file(GLOB PACKAGES_PATHS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "code/*")
set(PACKAGES "")
set(OBJECTS "")

foreach(PACKAGE_PATH ${PACKAGES_PATHS})
    message("PACKAGE " ${PACKAGE_PATH})
    get_filename_component(PACKAGE ${PACKAGE_PATH} NAME_WE) # NAME_WE = name with extension

    file(GLOB_RECURSE PACKAGE_SOURCES_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "code/${PACKAGE}/*.cpp")

    foreach(PACKAGE_SOURCE ${PACKAGE_SOURCES_LIST})
        get_filename_component(PACKAGE_SOURCE_WE ${PACKAGE_SOURCE} NAME_WE) # NAME_WE = name with extension
        message("OBJECT " ${PACKAGE_SOURCE_WE})
    endforeach()

    add_library(${PACKAGE} OBJECT ${PACKAGE_SOURCES_LIST})

    list(APPEND OBJECTS $<TARGET_OBJECTS:${PACKAGE}>)
endforeach()

# ----------------------------------------------------------------------------
# PROJECT LIBRARY
# ----------------------------------------------------------------------------

# single library
add_library(${PROJECTNAME} STATIC ${OBJECTS})

# ----------------------------------------------------------------------------
# LINK EXTERNAL LIBRARIES
# ----------------------------------------------------------------------------

# link libraries with forest library
target_link_libraries(${PROJECTNAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECTNAME} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECTNAME} glew_s)
target_link_libraries(${PROJECTNAME} glfw)
target_link_libraries(${PROJECTNAME} nlohmann_json)
target_link_libraries(${PROJECTNAME} libpng.so)
target_link_libraries(${PROJECTNAME} freetype)
# Link against the dependency of Intel TBB (for parallel C++17 algorithms)
target_link_libraries(${PROJECTNAME} TBB::tbb)
if(ENABLE_PROFILER)
    target_link_libraries(${PROJECTNAME} Tracy::TracyClient)
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