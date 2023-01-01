#include <benchmark/benchmark.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>

#include <container/vector.h>

#define KB(x) ((size_t)x << 10)
static void Memory_Access(benchmark::State &state) {
  auto size = state.range(0);
  std::uniform_int_distribution<> dis(0, KB(size) - 1);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<char> memory(KB(size));
  std::fill(memory.begin(), memory.end(), 1);
  int dummy = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(dummy += memory[dis(gen)]);
  }
}

BENCHMARK(Memory_Access)
    ->Arg(1)
    ->Arg(1 << 2)
    ->Arg(1 << 3)
    ->Arg(1 << 4)
    ->Arg(1 << 5)
    ->Arg(1 << 6)
    ->Arg(1 << 7)
    ->Arg(1 << 8)
    ->Arg(1 << 9)
    ->Arg(1 << 10)
    ->Arg(1 << 11)
    ->Arg(1 << 12)
    ->Arg(1 << 13)
    ->Arg(1 << 14)
    ->Arg(1 << 15)
    ->Arg(1 << 16)
    ->Arg(1 << 17);

BENCHMARK_MAIN();