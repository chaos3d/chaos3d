#ifndef _CHAOS3D_PLATFORM_GAME_WINDOW_H
#define _CHAOS3D_PLATFORM_GAME_WINDOW_H

#include <memory>
#include "event/event_dispatcher.h"
#include "common/referenced_count.h"

class game_window : public event_dispatcher, public referenced_count {
public:
    typedef std::unique_ptr<game_window, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<game_window const, referenced_count::release_deleter> const_ptr;
    
public:

};

#endif
