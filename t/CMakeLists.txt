find_package(GTest REQUIRED)

configure_file(
    testconfig.h.in
    testconfig.h
)

foreach(
    TEST
    memory-allocator-impl
    memory-block
    destroy
    dyn-cast
    function
    future
    itanium-vtable
    smart-pointer
    string-basic
    thread
    threadpool
    tuple
    types
    vector
    queue
    # FIXME
    # safe-queue
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
    set(USER_LIBS_DIR /usr/local/lib)
    target_link_libraries(
        ${TEST}.t
        squirrel
        ${GTEST_BOTH_LIBRARIES}
        ${USER_LIBS_DIR}/libgmock.a
        ${USER_LIBS_DIR}/libgmock_main.a
    )
    target_compile_options(${TEST}.t PRIVATE -Wall -Werror)
    add_test(
        NAME ${TEST}
        COMMAND ${TEST}.t
    )
endforeach(TEST)