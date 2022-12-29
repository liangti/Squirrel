#include <benchmark/benchmark.h>

#include <string/string.h>

#ifdef __GNUC__
#define CALL_CONV __attribute__((cdecl))
#warning "Using call convention cdecl"
#else
#define CALL_CONV
#warning "Not GCC compiler, will ignore call convention macro"
#endif

template <typename T> auto CALL_CONV value(T t) { return t; }

template <typename T> auto CALL_CONV ref(const T &t) { return t; }

struct PodObj {
  char c;
};

static void PassByValuePOD(benchmark::State &state) {
  auto param = PodObj();
  for (auto _ : state) {
    value(param);
  }
}

static void PassByRefPOD(benchmark::State &state) {
  auto param = PodObj();
  for (auto _ : state) {
    ref(param);
  }
}

static void PassByValueNotPOD(benchmark::State &state) {
  auto param = sqrl::String("Hello");
  for (auto _ : state) {
    value(param);
  }
}

static void PassByRefNotPOD(benchmark::State &state) {
  auto param = sqrl::String("Hello");
  for (auto _ : state) {
    ref(param);
  }
}

BENCHMARK(PassByValuePOD);
BENCHMARK(PassByRefPOD);
BENCHMARK(PassByValueNotPOD);
BENCHMARK(PassByRefNotPOD);

BENCHMARK_MAIN();