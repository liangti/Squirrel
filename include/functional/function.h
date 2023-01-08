#ifndef INCLUDED_FUNCTION_H
#define INCLUDED_FUNCTION_H

#include <memory>
#include <utility>

namespace sqrl {

template <typename> class Function;

template <class Return, class... Args> class Function<Return(Args...)> {
public:
  Function() = default;

  template <typename FS> Function(const FS &fs) {
    callable = std::make_shared<CallableImpl<FS>>(fs);
  }
  // function signature or just a functor type
  template <typename FS> Function &operator=(FS fs) {
    // unique_ptr is not ready for function type
    // delete callable;
    callable.reset();
    callable = std::make_shared<CallableImpl<FS>>(fs);
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
  // TODO: fix sqrl::shared_ptr
  std::shared_ptr<CallableBase> callable;
};

}; // end of namespace sqrl
#endif