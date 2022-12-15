#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <pthread.h>

namespace sqrl {

class Thread {
private:
  typedef void *(*Fp)(void *);
  pthread_t worker;

public:
  Thread(Fp fp, void *arg) : worker() {
    [[maybe_unused]] int succeed = pthread_create(&worker, NULL, fp, arg);
  }
  inline void join() { pthread_join(worker, NULL); }
};

}; // namespace sqrl

#endif