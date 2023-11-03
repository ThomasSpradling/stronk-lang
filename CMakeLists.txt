cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(StronkLang)

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/src/include/config.h)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to `Debug` as a default.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(EMSCRIPTEN)
    add_compile_options(-fexceptions)
    add_link_options(-fexceptions)
endif()

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

enable_testing()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

if(NOT DEFINED STRONK_SANITIZER)
    set(STRONK_SANITIZER address)
endif()

message("Build mode: ${CMAKE_BUILD_TYPE}")
message("${STRONK_SANITIZER} santizer will be enabled in debug mode.")

# Compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fsanitize=${STRONK_SANITIZER} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes.
set(STRONK_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(STRONK_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(STRONK_THIRD_PARTY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party)

include_directories(${BASE_DIR} ${STRONK_SRC_INCLUDE_DIR} ${STRONK_TEST_INCLUDE_DIR} ${STRONK_THIRD_PARTY_INCLUDE_DIR})

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)