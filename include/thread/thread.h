#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <pthread.h>

#include <metaprogramming/tuple.h>

namespace sqrl {

class Thread {
private:
  pthread_t worker;

public:
  template <typename Callable, typename... Args>
  explicit Thread(Callable &&fp, Args &&...args) : worker() {
    typedef sqrl::Tuple<Callable, Args...> fp_pack_t;

    struct Helper {
      static void *executor(void *args) {
        fp_pack_t *fp_pack = static_cast<fp_pack_t *>(args);
        sqrl::apply(*fp_pack);

        // ignore return value
        return NULL;
      }
    }; // internal Helper

    // TODO: memory leak
    fp_pack_t *fp_pack;
    fp_pack = new fp_pack_t(std::forward<Callable>(fp),
                            std::forward<Args...>(args)...);
    // TODO error handling
    [[maybe_unused]] int succeed = pthread_create(
        &worker, NULL, Helper::executor, reinterpret_cast<void *>(fp_pack));
  }

  inline void join() { pthread_join(worker, NULL); }
};

}; // namespace sqrl

#endif