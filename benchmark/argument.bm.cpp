#include <benchmark/benchmark.h>

#include <metaprogramming/tuple.h>

#ifdef __GNUC__
#define CALL_CONV __attribute__((cdecl))
#warning "Using call convention cdecl"
#else
#define CALL_CONV
#warning "Not GCC compiler, will ignore call convention macro"
#endif

template<typename T>
auto CALL_CONV value(T t){
  return t;
}

template<typename T>
auto CALL_CONV ref(const T& t){
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