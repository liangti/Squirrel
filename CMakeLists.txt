cmake_minimum_required(VERSION 3.6)

project(Squirrel CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    squirrel
    src/empty.cpp
)

target_include_directories(
    squirrel
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

include(CTest)

set(CMAKE_ROOT_DIR ${CMAKE_SOURCE_DIR})
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(t)
endif()