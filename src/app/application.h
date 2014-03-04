#ifndef _CHAOS3D_APP_APPLICATION_H
#define _CHAOS3D_APP_APPLICATION_H

#include "common/singleton.h"
#include "event/event_listener.h"

namespace evt {
    class memory_warning : public event {
    };
    
    class notification : public event {
    };
}

class application : public event_listener, public singleton<application> {
public:
    //
    // common template methods rather than events
    
    // called before everything is initialized
    //  this should be to initialize global singletons etc
    //  but don't touch any part of engine
    virtual void on_initialize() = 0;
    
    // called before the run loop starts
    //  this should be to initialize components, actions to
    //  actually start the game
    virtual void on_launch() = 0;
};

#endif