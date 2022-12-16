#include <benchmark/benchmark.h>

#include <string/string.h>

static void SQRL_StringCreate(benchmark::State& state) {
  for (auto _ : state){
    sqrl::String str("hi");
  }
}

static void STD_StringCreate(benchmark::State& state) {
  for (auto _ : state){
    sqrl::String str("hi");
  }
}

static void SQRL_StringCopy(benchmark::State& state) {
  sqrl::String x("hello");
  for (auto _ : state){
    sqrl::String copy(x);
  }
}

static void STD_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state){
    std::string copy(x);
  }
}

BENCHMARK(SQRL_StringCreate);
BENCHMARK(STD_StringCreate);
BENCHMARK(SQRL_StringCopy);
BENCHMARK(STD_StringCopy);

BENCHMARK_MAIN();