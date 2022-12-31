#include <memory>
#include <benchmark/benchmark.h>

#include <memory/allocator.h>

static void STD_Allocate(benchmark::State& state) {
  for (auto _ : state){
    std::allocator<int> alloc;
    alloc.allocate(2);
  }
}

static void SQRL_Allocate(benchmark::State& state) {
  for (auto _ : state){
    sqrl::Allocator<int> alloc;
    alloc.allocate(2);
  }
}

BENCHMARK(STD_Allocate);
BENCHMARK(SQRL_Allocate);

BENCHMARK_MAIN();