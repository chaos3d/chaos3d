#ifndef _CHAOS3D_PLATFORM_LAUNCHER_H
#define _CHAOS3D_PLATFORM_LAUNCHER_H

#include <array>
#include "common/singleton.h"
#include "event/event_dispatcher.h"
#include "game_window.h"

class launcher : public singleton<launcher>, public event_dispatcher {
public:
    virtual ~launcher() {};
    
    // polling system events
    virtual bool poll_event(bool wait = false) = 0;
    
    // create the main window
    // TODO: could be const? or track windows?
    virtual game_window::ptr create_game_window(char const*,
                                                std::array<float, 4> const&) = 0;
    
public:
    // to initialize the environment
    //  i.e. create NSApp for Mac, set up event loop for Win
    static launcher& initialize();  // platform dependent impl
};

#endif
