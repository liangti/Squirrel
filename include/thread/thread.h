#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <pthread.h>

#include <memory/allocator.h>
#include <metaprogramming/tuple.h>

namespace sqrl {

namespace details {

template <bool AutoJoin> class ThreadImpl {
private:
  pthread_t worker;
  bool created;
  bool joined;

public:
  template <typename Callable, typename... Args>
  explicit ThreadImpl(Callable &&fp, Args &&...args) noexcept
      : worker(), joined(false) {
    typedef sqrl::Tuple<Callable, Args...> fp_pack_t;

    struct Helper {
      static void *executor(void *args) {
        // local allocator
        static sqrl::Allocator<char> allocator;
        fp_pack_t *fp_pack = static_cast<fp_pack_t *>(args);
        sqrl::apply(*fp_pack);
        // clean function pack after executing it
        allocator.deallocate((char *)args, sizeof(fp_pack));
        // ignore return value
        return NULL;
      }
    }; // internal Helper

    // memory leak mitigation
    sqrl::Allocator<char> allocator;
    size_t pack_size = sizeof(fp_pack_t);
    char *pack_block = allocator.allocate(pack_size);
    fp_pack_t *memory_block = reinterpret_cast<fp_pack_t *>(pack_block);
    fp_pack_t *fp_pack = new (memory_block)
        fp_pack_t(std::forward<Callable>(fp), std::forward<Args>(args)...);

    // error handling
    [[maybe_unused]] int succeed = pthread_create(
        &worker, NULL, Helper::executor, reinterpret_cast<void *>(fp_pack));
    if (succeed != 0) {
      allocator.deallocate(pack_block, pack_size);
      created = false;
    } else {
      created = true;
    }
  }

  inline void join() noexcept {
    if (created && !joined) {
      pthread_join(worker, NULL);
      joined = true;
    }
  }

  bool joinable() { return !joined; };

  ~ThreadImpl() {
    if constexpr (AutoJoin) {
      join();
    }
  }
};

}; // namespace details

using Thread = details::ThreadImpl<false>;
using JThread = details::ThreadImpl<true>;

}; // namespace sqrl

#endif