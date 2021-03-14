#ifndef INCLUDED_RECLAIMER_H
#define INCLUDED_RECLAIMER_H

#include <memory/block.h>

namespace sql {

class Reclaimer {
public:
  void run();
};

}; // namespace sql

#endif