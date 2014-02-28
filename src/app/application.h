#ifndef _CHAOS3D_APP_APPLICATION_H
#define _CHAOS3D_APP_APPLICATION_H

#include "common/singleton.h"
#include "event/event_listener.h"

class application : public event_listener, public singleton<application> {
public:
    //
    // common template methods rather than events
    
    // called after the system is warmed up
    virtual void on_initialize() = 0;
    
    // called before the run loop starts
    virtual void on_launch() = 0;
};

#endif