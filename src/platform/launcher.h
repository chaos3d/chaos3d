#ifndef _CHAOS3D_PLATFORM_LAUNCHER_H
#define _CHAOS3D_PLATFORM_LAUNCHER_H

#include <Eigen/Dense>
#include "common/singleton.h"
#include "event/event_dispatcher.h"
#include "game_window.h"

class launcher : public singleton<launcher>, public event_dispatcher {
public:
    typedef Eigen::Vector2f vector2f;
    
public:
    virtual ~launcher() {};
    
    // create the main window
    // TODO: could be const? or track windows?
    virtual game_window::ptr create_game_window(vector2f const& size,
                                                vector2f const& origin = vector2f::Zero()) = 0;
    
public:
    // to initialize the environment
    //  i.e. create NSApp for Mac, set up event loop for Win
    static launcher& initialize();  // platform dependent impl
};

#endif
