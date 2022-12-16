#include <benchmark/benchmark.h>

#include <metaprogramming/tuple.h>

template<typename T>
auto value(T t){
  return t;
}

template<typename T>
auto ref(const T& t){
  return t;
}

static void PassByValuePOD1(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<1>();
  for (auto _ : state){
    value(param);
  }
}

static void PassByRefPOD1(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<1>();
  for (auto _ : state){
    ref(param);
  }
}

static void PassByValuePOD2(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<2>();
  for (auto _ : state){
    value(param);
  }
}

static void PassByRefPOD2(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<2>();
  for (auto _ : state){
    ref(param);
  }
}

static void PassByValuePOD3(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<3>();
  for (auto _ : state){
    value(param);
  }
}

static void PassByRefPOD3(benchmark::State& state){
  auto param = sqrl::make_repeat_integer_tuple<3>();
  for (auto _ : state){
    ref(param);
  }
}

BENCHMARK(PassByValuePOD1);
BENCHMARK(PassByRefPOD1);
BENCHMARK(PassByValuePOD2);
BENCHMARK(PassByRefPOD2);
BENCHMARK(PassByValuePOD3);
BENCHMARK(PassByRefPOD3);

BENCHMARK_MAIN();