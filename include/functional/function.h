#ifndef INCLUDED_FUNCTION_H
#define INCLUDED_FUNCTION_H

#include <utility>

#include <memory/smart_pointer.h>

namespace sqrl {

template <typename> class Function;

template <class Return, class... Args> class Function<Return(Args...)> {
public:
  // constructors
  Function() = default;
  template <typename FS> Function(FS fs) : callable(new CallableImpl<FS>(fs)) {}
  Function(const Function &other) { callable = other.callable; }
  Function(Function &&other) {
    callable = other.callable;
    other.callable = nullptr;
  }
  Function &operator=(const Function &other) {
    callable = other.callable;
    return *this;
  }
  // function signature or just a functor type
  template <typename FS> Function &operator=(FS fs) {
    // unique_ptr is not ready for function type
    // delete callable;
    callable.reset(new CallableImpl<FS>(fs));
    return *this;
  }
  Return operator()(Args &&...args) {
    return callable->invoke(std::forward<Args>(args)...);
  }
  ~Function() {}

private:
  struct CallableBase {
    virtual Return invoke(Args &&...args) = 0;
    virtual ~CallableBase() = default;
  };
  template <typename FS> struct CallableImpl : public CallableBase {
    CallableImpl(const FS &fs) : fp(fs) {}
    Return invoke(Args &&...args) override {
      return fp(std::forward<Args>(args)...);
    }
    FS fp;
  };
  // TODO: sqrl::shared_ptr having same issue as raw pointer in concurrency run
  sqrl::shared_ptr<CallableBase> callable;
};

}; // end of namespace sqrl
#endif