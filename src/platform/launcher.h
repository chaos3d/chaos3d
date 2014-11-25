#ifndef _CHAOS3D_PLATFORM_LAUNCHER_H
#define _CHAOS3D_PLATFORM_LAUNCHER_H

#include <array>
#include "action/action.h"
#include "re/native_window.h"
#include "re/render_device.h"
#include "common/singleton.h"
#include "common/base_types.h"
#include "event/event_dispatcher.h"

class timer;

class launcher : public singleton<launcher>, public event_dispatcher {
public:
    virtual ~launcher() {};
    
    /// polling system events and update the actions/animations
    /// on iOS or hosted system, this will yield to the system and resume
    /// by the timer
    virtual bool poll_event(bool wait = false, timer* = nullptr);
    
    /// create the main window
    // TODO: could be const? or track windows?
    virtual native_window::ptr create_game_window(char const*,
                                                  std::array<float, 4> const&) = 0;
    
    /// create the render device, only OGL ES2 for now (TODO)
    virtual ::render_device* get_render_device() {
        return render_device::get_device();
    }
    
    // the action that handles timely updates for animations, scripts etc.
    root_action& action() { return _root_action; };
    
    /// get the screen size, platform-impl
    vector2f get_screen() const;
    
    /// get the hostname, separated by space
    /// ios, ipad, mac, win, platform-impl
    char const* get_host() const;

public:
    // to initialize the environment, this is platform-specific impl
    //  i.e. create NSApp for Mac, set up event loop for Win
    static launcher& initialize();  // platform dependent impl
    
private:
    root_action _root_action;
};

#endif
