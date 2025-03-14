cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# project name
set(PROJECTNAME forest)
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
    message("BUILD PROGRAM ${PROGRAM_PATH}")

    file(GLOB_RECURSE PROGRAM_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.cpp")
    file(GLOB_RECURSE PROGRAM_SOURCES_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROGRAM_PATH}/*.c")
    set(PROGRAM_SOURCES "${PROGRAM_SOURCES_CXX};${PROGRAM_SOURCES_C}")

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

# NOTE: delete default flags!
set(CMAKE_CXX_FLAGS_DEBUG "") #-g
set(CMAKE_CXX_FLAGS_RELEASE "") #-O3 -DNDEBUG

# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -header-filter=.;
#   -checks=*;
# # -warnings-as-errors=*;
# )

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
    
    set(PROJECT_LINKER_FLAGS_DEBUG "${PROJECT_LINKER_FLAGS_DEBUG}${PROJECT_LINKER_FLAGS_DEBUG}")
    set(PROJECT_LINKER_FLAGS_RELEASE "${PROJECT_LINKER_FLAGS_RELEASE}${PROJECT_LINKER_FLAGS_RELEASE}")
    
    if(ENABLE_SANITIZER)
        set(PROJECT_LINKER_FLAGS_DEBUG "${PROJECT_LINKER_FLAGS_DEBUG}${PROJECT_LINKER_FLAGS_DEBUG}-fsanitize=address;-static-libasan;")
        set(PROJECT_LINKER_FLAGS_RELEASE "${PROJECT_LINKER_FLAGS_RELEASE}${PROJECT_LINKER_FLAGS_RELEASE}-fsanitize=address;-static-libasan;")
    endif(ENABLE_SANITIZER)
endif(UNIX)

if(WIN32)
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_COMPILER_FLAGS} -D ENGINE_BUILD_DEBUG /m /TP")
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D ENGINE_BUILD_RELEASE")
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
# find_package(OpenGL REQUIRED)
# include_directories(${OPENGL_INCLUDE_DIR})

# Vulkan
find_package(Vulkan REQUIRED)

# glslang (glsl runtime compiler)
find_package(glslang REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # see : http://www.glfw.org/docs/latest/build_guide.html
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/glfw-3.4")
add_subdirectory("${DEPENDENCIES_DIR}/glfw-3.4")
include_directories("${DEPENDENCIES_DIR}/glfw-3.4/include")

# glew
# set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
# check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/glew-2.2.0")
# add_subdirectory("${DEPENDENCIES_DIR}/glew-2.2.0/build/cmake")

# include_directories("${DEPENDENCIES_DIR}/glew-2.2.0/include")

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
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/freetype-2.13.2")
include_directories("${DEPENDENCIES_DIR}/freetype-2.13.2/include")

# cgltf
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/cgltf-1.14")
include_directories("${DEPENDENCIES_DIR}/cgltf-1.14")

# fmt
check_exist("${CMAKE_SOURCE_DIR}/${DEPENDENCIES_DIR}/fmt-11.1.2")
include_directories("${DEPENDENCIES_DIR}/fmt-11.1.2/include")
add_subdirectory("${DEPENDENCIES_DIR}/fmt-11.1.2")

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
    # message("PACKAGE " ${PACKAGE_PATH})
    get_filename_component(PACKAGE ${PACKAGE_PATH} NAME_WE) # NAME_WE = name with extension

    file(GLOB_RECURSE PACKAGE_SOURCES_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "code/${PACKAGE}/*.cpp")

    foreach(PACKAGE_SOURCE ${PACKAGE_SOURCES_LIST})
        get_filename_component(PACKAGE_SOURCE_WE ${PACKAGE_SOURCE} NAME_WE) # NAME_WE = name with extension
        # message("OBJECT " ${PACKAGE_SOURCE_WE})
    endforeach()

    add_library(${PACKAGE} OBJECT ${PACKAGE_SOURCES_LIST})

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
target_compile_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_COMPILER_FLAGS_DEBUG}>")
target_compile_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_COMPILER_FLAGS_RELEASE}>")
target_link_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:DEBUG>:${PROJECT_LINKER_FLAGS_DEBUG}>")
target_link_options(${PROJECTNAME} PUBLIC "$<$<CONFIG:RELEASE>:${PROJECT_LINKER_FLAGS_RELEASE}>")

# ----------------------------------------------------------------------------
# LINK EXTERNAL LIBRARIES
# ----------------------------------------------------------------------------

# target_include_directories(${PROJECT_NAME} PUBLIC
#     ${Vulkan_INCLUDE_DIRS}
# )

# target_link_directories(${PROJECT_NAME} PUBLIC
#     ${Vulkan_LIBRARIES}
# )

# link libraries with forest library
# target_link_libraries(${PROJECTNAME} ${OPENGL_gl_LIBRARY})
# target_link_libraries(${PROJECTNAME} ${OPENGL_glu_LIBRARY})
target_link_libraries(${PROJECTNAME} Vulkan::Vulkan)
target_link_libraries(${PROJECTNAME} glslang glslang-default-resource-limits)
# target_link_libraries(${PROJECTNAME} glew)
target_link_libraries(${PROJECTNAME} glfw)
target_link_libraries(${PROJECTNAME} nlohmann_json)
target_link_libraries(${PROJECTNAME} libpng.so)
target_link_libraries(${PROJECTNAME} freetype)
target_link_libraries(${PROJECTNAME} fmt)
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