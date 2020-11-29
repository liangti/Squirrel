find_package(GTest REQUIRED)

configure_file(
    testconfig.h.in
    testconfig.h
)

foreach(
    TEST
    01-memory-allocator
    02-shared-pointer
    03-string-basic
    04-threadpool
    05-tuple
)
    add_executable(${TEST}.t ${TEST}.t.cpp)
    target_include_directories(
        ${TEST}.t
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_ROOT_DIR}/include>
    )
    target_include_directories(
        ${TEST}.t
        SYSTEM
        PRIVATE
        ${GTEST_INCLUDE_DIRS}
    )
    target_link_libraries(
        ${TEST}.t
        squirrel
        ${GTEST_BOTH_LIBRARIES}
    )
    add_test(
        NAME ${TEST}
        COMMAND ${TEST}.t
    )
endforeach(TEST)