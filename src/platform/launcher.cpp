#include "platform/launcher.h"
#include "action/action.h"

bool launcher::poll_event(bool wait, timer* timer_) {
    _root_action.update();
    return true;
}
