#ifndef _CHAOS3D_UI_UI_MANAGER_H
#define _CHAOS3D_UI_UI_MANAGER_H

#include <vector>
#include <Eigen/Geometry>
#include "event/event_dispatcher.h"
#include "event/touch_event.h"
#include "go/component_manager.h"
#include "re/native_window.h"

namespace com {
    class camera;
}

class ui_control;

/// ui manager to handle events and dispatch
/// it will also dispatch wrapped events, i.e. ui_click
/// ui_value_change, which will be generated from ui elements
class ui_manager : public event_dispatcher {
//: public component_manager_base<ui_manager> {
    // TODO: may need to be component for ui elements
public:

public:
    /// the main window to capture the input event
    /// the camera to convert screen coord to world point
    ui_manager(native_window*, com::camera*);
    
protected:
    void touch_began(touch_began_event const&);
    void touch_moved(touch_moved_event const&);
    void touch_ended(touch_ended_event const&);
    
private:
    native_window::ptr _window;
    com::camera* _camera;
};

#endif
