#ifndef _CHAOS3D_APP_APPLICATION_H
#define _CHAOS3D_APP_APPLICATION_H

#include "common/singleton.h"
#include "event/event_listener.h"

class screen;
class render_window;
class render_device;
class render_context;

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
    virtual bool on_initialize();
    
    // called before the run loop starts
    //  this should be to initialize components, actions to
    //  actually start the game
    //  it also needs to init render context
    virtual void on_launch() = 0;
    
    // get the main screen
    virtual screen& get_screen() const;
    
    render_window* main_window() const { return _main_window; }
    render_device* main_device() const { return _main_device; }
    render_context* main_context() const { return _main_context; }
    
protected:
    application& set_main_window(render_window* window) { _main_window = window; return *this; }
    application& set_main_device(render_device* device) { _main_device = device; return *this; }
    application& set_main_context(render_context* context) { _main_context = context; return *this; }
    
private:
    render_window* _main_window = nullptr;
    render_device* _main_device = nullptr;
    render_context* _main_context = nullptr;
};

#endif