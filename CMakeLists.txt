cmake_minimum_required(VERSION 3.6)

project(Squirrel CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    squirrel
    src/memory/allocator.cpp
    src/memory/allocator_utils.cpp
    src/string/string.cpp
    src/thread/threadpool.cpp
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