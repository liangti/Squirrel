#ifndef INCLUDED_FUTURE_H
#define INCLUDED_FUTURE_H

#include <condition_variable>
#include <memory>
#include <mutex>

#include <functional/function.h>

namespace sqrl {

namespace details {

template <typename T> struct _SharedState {
  T value;
  bool ready;
  std::mutex mutex;
  std::condition_variable cvar;
};

}; // namespace details

using namespace details;

// no exception

template <typename T> class Future {
private:
  std::shared_ptr<_SharedState<T>> state;

public:
  Future() = default;
  Future(std::shared_ptr<_SharedState<T>> state) : state(state) {}
  Future(const Future<T> &) = delete;
  Future(Future<T> &&other) {
    state = other.state;
    other.state.reset();
  }
  Future<T> &operator=(Future<T> &&other) {
    state = other.state;
    other.state.reset();
    return *this;
  }
  bool valid() { return bool(state); }
  void wait() {
    std::unique_lock<std::mutex> lock;
    while (!state->ready) {
      state->cvar.wait(lock);
    }
  }
  T get() {
    wait();
    return state->value;
  }
};

template <typename T> class Promise {
private:
  std::shared_ptr<_SharedState<T>> state;
  bool future_already_retrieved;

public:
  Promise() {
    state = std::make_shared<_SharedState<T>>();
    future_already_retrieved = false;
  }
  Promise(const Promise<T> &) = delete;
  Promise(Promise<T> &&other) {
    state = other.state;
    other.state.reset();
  }
  Promise<T> &operator=(const Promise<T> &) = delete;
  Promise<T> &operator=(Promise<T> &&other) {
    state = other.state;
    other.state.reset();
    return *this;
  }
  void set_value(T value) {
    std::lock_guard<std::mutex> lock(state->mutex);
    state->value = value;
    state->ready = true;
    state->cvar.notify_all();
  }
  Future<T> get_future() {
    if (future_already_retrieved) {
      return Future<T>(nullptr);
    }
    future_already_retrieved = true;
    return Future<T>(state);
  }
};

template <class Result, class... Args> class PackageTask;

// template specialization for function type input
// non function type input will end up geting "undefined template" compile time
// error
template <class Result, class... Args> class PackageTask<Result(Args...)> {
private:
  Promise<Result> promise;
  Future<Result> future;
  sqrl::Function<void(Promise<Result>, Args...)> task;
  bool promise_already_satisfied;

public:
  template <class F>
  PackageTask(F &&f) : promise(), future(promise.get_future()) {
    // issue: how capture arguments copy/move by std::function
    task = [functor = std::forward<F>(f)](Promise<Result> promise,
                                          Args &&...args) mutable {
      promise.set_value(functor(std::forward<Args>(args)...));
    };
  }
  PackageTask(const PackageTask &) = delete;
  PackageTask(PackageTask &&other)
      : promise(std::move(other.promise)), future(std::move(other.future)),
        task(std::move(other.task)) {}
  void operator()(Args &&...args) {
    task(std::move(promise), std::forward<Args>(args)...);
  }

  Future<Result> get_future() { return std::move(future); }
};

}; // namespace sqrl

#endif