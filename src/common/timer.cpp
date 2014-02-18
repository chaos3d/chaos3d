#include "common/timer.h"

#ifdef __APPLE__
const double timer::_tick_per_second = timer::tick_per_second();
#endif