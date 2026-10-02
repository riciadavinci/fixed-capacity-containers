cmake_minimum_required(VERSION 3.14)

project(fixed_capacity_containers 
    VERSION 1.0.0
    DESCRIPTION "Header-only C++20 fixed-capacity containers with zero heap allocation for embedded and bare-metal systems (also usable on desktop): ring buffer, queue, lock-free queue, and pluggable mutex policies."
    LANGUAGES CXX
)

# Fallback to Release build if no CMAKE_BUILD_TYPE is explicitly specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_library(fcc INTERFACE)
add_library(fcc::fcc ALIAS fcc)

target_compile_features(fcc INTERFACE cxx_std_20)

target_include_directories(fcc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(FCC_BUILD_TESTS "Build unit tests for fixed-capacity-containers" ON)

if (FCC_BUILD_TESTS AND (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME))
    include(CTest)
    add_subdirectory(tests)
endif()
