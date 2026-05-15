cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(${PROJECTNAME} LANGUAGES CXX C)

# ----------------------------------------------------------------------------
# FUNCTIONS
# ----------------------------------------------------------------------------

function(check_exist path)
    if(NOT EXISTS ${path})
        message(FATAL_ERROR "${path} doesn't exist!")
    endif()
endfunction()


function(build_program path prefix program_name)

    set(PROGRAM_PATH "${path}/${program_name}")
    message(NOTICE "BUILD PROGRAM ${PROGRAM_PATH}")

    file(GLOB_RECURSE PROGRAM_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.cpp")
    file(GLOB_RECURSE PROGRAM_SOURCES_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.c")
    set(PROGRAM_SOURCES "${PROGRAM_SOURCES_CXX};${PROGRAM_SOURCES_C}")

    include_directories("${PROGRAM_PATH}")

    set(PROGRAM_FINAL_NAME "${prefix}_${program_name}" )
    add_executable( ${PROGRAM_FINAL_NAME} ${PROGRAM_SOURCES})
    target_link_libraries( ${PROGRAM_FINAL_NAME} ${PROJECTNAME})
    install(TARGETS ${PROGRAM_FINAL_NAME})

endfunction()

# ----------------------------------------------------------------------------
# CPP CONFIG
# ----------------------------------------------------------------------------

set(CMAKE_MESSAGE_INDENT "[CMAKE]> ")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# NOTE: delete default flags!
set(CMAKE_CXX_FLAGS_DEBUG "") #-g
set(CMAKE_CXX_FLAGS_RELEASE "") #-O3 -DNDEBUG

# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -header-filter=.;
#   -checks=*;
# # -warnings-as-errors=*;
# )

set(PROJECT_COMPILER_FLAGS "")
set(PROJECT_COMPILER_FLAGS "-fdiagnostics-color=always;")
set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-DENGINE_GPU_VULKAN;")

if(ENABLE_LOGS)
set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-DENGINE_ENABLE_LOGS;")
endif(ENABLE_LOGS)

if(ENABLE_GPU_DEBUG)
set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-DENGINE_ENABLE_GPU_DEBUG;")
endif(ENABLE_GPU_DEBUG)

if(ENABLE_PROFILER)
set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-DTRACY_ENABLE;-DENGINE_ENABLE_PROFILER;")
endif(ENABLE_PROFILER)

if(UNIX)
    set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-mavx;-Wall;-Werror=return-type;-Wcast-align;-Wcast-qual;-Wpointer-arith;-Wfloat-equal;") # -flto=auto
  
    if(ENABLE_SANITIZER)
        set(PROJECT_COMPILER_FLAGS "${PROJECT_COMPILER_FLAGS}-fsanitize=address;")
    endif(ENABLE_SANITIZER)
        
    set(PROJECT_COMPILER_FLAGS_DEBUG "${PROJECT_COMPILER_FLAGS}-DENGINE_BUILD_DEBUG;-g;-ftree-vectorize;")
    set(PROJECT_COMPILER_FLAGS_RELEASE "${PROJECT_COMPILER_FLAGS}-DENGINE_BUILD_RELEASE;-O3;")

    # SHIP: RELEASE + extras
    # NDEBUG disables standard-C assertions https://en.cppreference.com/w/c/error/assert
    # set(PROJECT_COMPILER_FLAGS_SHIP "${PROJECT_COMPILER_FLAGS_RELEASE}-DENGINE_BUILD_RELEASE;-DNDEBUG;")
    
    set(PROJECT_LINKER_FLAGS_DEBUG "")
    set(PROJECT_LINKER_FLAGS_RELEASE "")
    
    if(ENABLE_SANITIZER)
        set(PROJECT_LINKER_FLAGS_DEBUG "${PROJECT_LINKER_FLAGS_DEBUG}-fsanitize=address;-static-libsan;")
        set(PROJECT_LINKER_FLAGS_RELEASE "${PROJECT_LINKER_FLAGS_RELEASE}-fsanitize=address;-static-libsan;")
    endif(ENABLE_SANITIZER)
endif(UNIX)

if(WIN32)
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_COMPILER_FLAGS} -D ENGINE_BUILD_DEBUG /m /TP")
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D ENGINE_BUILD_RELEASE")
endif(WIN32)

find_program(MOLD_PROGRAM mold)
if (MOLD_PROGRAM)
    set(CMAKE_LINKER_TYPE MOLD)
    message(NOTICE "Using MOLD: ${MOLD_PROGRAM}")
else ()
    message(NOTICE "Not Using MOLD")
endif ()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(NOTICE "Using CCACHE ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    set(CMAKE_C_COMPILER_LAUNCHER ccache)
else()
    message(NOTICE "Not Using CCACHE")
endif()

message(NOTICE "Compiler: ${CMAKE_CXX_COMPILER}")
message(NOTICE "Release: ${PROJECT_COMPILER_FLAGS_RELEASE}")
message(NOTICE "Linker Release: ${PROJECT_LINKER_FLAGS_RELEASE}")
message(NOTICE "Debug: ${PROJECT_COMPILER_FLAGS_DEBUG}")
message(NOTICE "Linker Debug: ${PROJECT_LINKER_FLAGS_DEBUG}")
message(NOTICE "Selected Build target: " ${CMAKE_BUILD_TYPE})

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

# Vulkan
# include_directories("${DEPENDENCIES_DIR}/vulkansdk/x86_64/include")

# find_library(VULKAN_LIB vulkan
#     PATHS "${DEPENDENCIES_DIR}/vulkansdk/x86_64/lib/"
#     NO_DEFAULT_PATH)

# # glslang (glsl runtime compiler)
# find_library(GLSLANG_LIB glslang
#     PATHS "${DEPENDENCIES_DIR}/vulkansdk/x86_64/lib/"
#     NO_DEFAULT_PATH)
# find_library(GLSLANG_DEFAULT_RESOURCE_LIMIT_LIB glslang-default-resource-limits
#     PATHS "${DEPENDENCIES_DIR}/vulkansdk/x86_64/lib/"
#     NO_DEFAULT_PATH)

find_package(Vulkan REQUIRED)
find_package(glslang REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # see : http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/glfw-3.4")
add_subdirectory("${DEPENDENCIES_DIR}/glfw-3.4")
include_directories("${DEPENDENCIES_DIR}/glfw-3.4/include")

if(ENABLE_PROFILER)
    # Tracy
    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
    set(LEGACY ON CACHE BOOL "" FORCE)
    set(DOWNLOAD_GLFW OFF CACHE BOOL "" FORCE)
    set(DOWNLOAD_CAPSTONE OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/tracy-0.11.1")
    include_directories("${DEPENDENCIES_DIR}/tracy-0.11.1/public")
    add_subdirectory("${DEPENDENCIES_DIR}/tracy-0.11.1")
endif(ENABLE_PROFILER)

# JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/json-3.11.3")
include_directories("${DEPENDENCIES_DIR}/json-3.11.3/single_include/nlohmann")
add_subdirectory("${DEPENDENCIES_DIR}/json-3.11.3")

# stb
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/stb-master")
include_directories("${DEPENDENCIES_DIR}/stb-master")

# freetype
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/freetype-2.14.3")
add_subdirectory("${DEPENDENCIES_DIR}/freetype-2.14.3")
include_directories("${DEPENDENCIES_DIR}/freetype-2.14.3/include")

# cgltf
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/cgltf-1.14")
include_directories("${DEPENDENCIES_DIR}/cgltf-1.14")

# fmt
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/fmt-11.1.2")
include_directories("${DEPENDENCIES_DIR}/fmt-11.1.2/include")
add_subdirectory("${DEPENDENCIES_DIR}/fmt-11.1.2")

# ----------------------------------------------------------------------------
# PROJECT PACKAGES
# ----------------------------------------------------------------------------

# include source code root
include_directories("code")

file(GLOB PACKAGES_PATHS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "code/*")
set(PACKAGES "")
set(OBJECTS "")

set(FIRST_PACKAGE "")
foreach(PACKAGE_PATH ${PACKAGES_PATHS})
    # message(NOTICE "PACKAGE " ${PACKAGE_PATH})
    get_filename_component(PACKAGE ${PACKAGE_PATH} NAME_WE) # NAME_WE = name with extension

    file(GLOB_RECURSE PACKAGE_SOURCES_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "code/${PACKAGE}/*.cpp")

    add_library(${PACKAGE} OBJECT ${PACKAGE_SOURCES_LIST})

    if(FIRST_PACKAGE STREQUAL "")
        target_precompile_headers(${PACKAGE} PRIVATE "pch/pch.h")
        set(FIRST_PACKAGE ${PACKAGE})
    else()
        # Reuse the already compiled PCH from the first package
        target_precompile_headers(${PACKAGE} REUSE_FROM ${FIRST_PACKAGE})
    endif()

    target_compile_options(${PACKAGE} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_COMPILER_FLAGS_DEBUG}>")
    target_compile_options(${PACKAGE} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_COMPILER_FLAGS_RELEASE}>")
    target_link_options(${PACKAGE} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_LINKER_FLAGS_DEBUG}>")
    target_link_options(${PACKAGE} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_LINKER_FLAGS_RELEASE}>")

    list(APPEND OBJECTS $<TARGET_OBJECTS:${PACKAGE}>)
endforeach()

# ----------------------------------------------------------------------------
# PROJECT LIBRARY
# ----------------------------------------------------------------------------

# single library
add_library(${PROJECTNAME} STATIC ${OBJECTS})

target_precompile_headers(${PROJECTNAME} REUSE_FROM ${FIRST_PACKAGE})

set_property(TARGET ${PROJECTNAME} PROPERTY CXX_STANDARD 20) # Use C++20
set_property(TARGET ${PROJECTNAME} PROPERTY CXX_STANDARD_REQUIRED TRUE) # C++20 is mandatory
set_property(TARGET ${PROJECTNAME} PROPERTY CXX_EXTENSIONS FALSE) # No extensions

target_compile_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_COMPILER_FLAGS_DEBUG}>")
target_compile_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_COMPILER_FLAGS_RELEASE}>")
target_link_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_LINKER_FLAGS_DEBUG}>")
target_link_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_LINKER_FLAGS_RELEASE}>")

# ----------------------------------------------------------------------------
# LINK EXTERNAL LIBRARIES
# ----------------------------------------------------------------------------

# link libraries with main library
# target_link_libraries(${PROJECTNAME} ${VULKAN_LIB})
# target_link_libraries(${PROJECTNAME} ${GLSLANG_LIB} ${GLSLANG_DEFAULT_RESOURCE_LIMIT_LIB})
target_link_libraries(${PROJECTNAME} Vulkan::Vulkan)
target_link_libraries(${PROJECTNAME} glslang::glslang glslang::SPIRV glslang::glslang-default-resource-limits)
target_link_libraries(${PROJECTNAME} glfw)
target_link_libraries(${PROJECTNAME} nlohmann_json)
target_link_libraries(${PROJECTNAME} freetype)
target_link_libraries(${PROJECTNAME} fmt)

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

message(NOTICE "Tools to build: ${TOOLS_TO_BUILD}")
FOREACH(TOOL ${TOOLS_TO_BUILD})
    build_program("tools" "tool" ${TOOL})
ENDFOREACH()

# ----------------------------------------------------------------------------
# APPS
# ----------------------------------------------------------------------------

message(NOTICE "Apps to build: ${APPS_TO_BUILD}")
FOREACH(APP ${APPS_TO_BUILD})
    build_program("apps" "app" ${APP})
ENDFOREACH()