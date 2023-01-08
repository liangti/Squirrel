#ifndef INCLUDED_FUNCTION_H
#define INCLUDED_FUNCTION_H

#include <utility>

namespace sqrl {

template <typename> class Function;

template <class Return, class... Args> class Function<Return(Args...)> {
public:
  // function signature or just a functor type
  template <typename FS> Function &operator=(FS fs) {
    // unique_ptr is not ready for function type
    delete callable;
    callable = new CallableImpl<FS>(fs);
    return *this;
  }
  Return operator()(Args &&...args) {
    return callable->invoke(std::forward<Args>(args)...);
  }
  ~Function() { delete callable; }

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
  CallableBase *callable = nullptr;
};

}; // end of namespace sqrl
#endif