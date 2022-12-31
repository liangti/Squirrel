find_package(benchmark REQUIRED)

foreach(
    BENCHMARK
    string
    argument
    allocator
)

    add_executable(${BENCHMARK}.bm ${BENCHMARK}.bm.cpp)
    target_include_directories(
        ${BENCHMARK}.bm
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_ROOT_DIR}/include>
    )
    target_include_directories(
        ${BENCHMARK}.bm
        SYSTEM
        PRIVATE
        ${GTEST_INCLUDE_DIRS}
    )
    target_link_libraries(
        ${BENCHMARK}.bm
        squirrel
        benchmark::benchmark    
    )
endforeach(BENCHMARK)
