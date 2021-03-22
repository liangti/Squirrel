#include <benchmark/benchmark.h>
#include <string/string.h>

static void SQL_StringCreate(benchmark::State& state) {
  for (auto _ : state)
    sql::String str("hi");
}

static void STD_StringCreate(benchmark::State& state) {
  for (auto _ : state)
    std::string str("hi");
}

static void SQL_StringCopy(benchmark::State& state) {
  sql::String x("hello");
  for (auto _ : state)
    sql::String copy(x);
}

static void STD_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}

BENCHMARK(SQL_StringCreate);
BENCHMARK(STD_StringCreate);
BENCHMARK(SQL_StringCopy);
BENCHMARK(STD_StringCopy);

BENCHMARK_MAIN();