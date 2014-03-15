#ifndef _UI_UI_MANAGER_H
#define _UI_UI_MANAGER_H

#include <vector>
#include <Eigen/Geometry>
#include "com/render/camera.h"
#include "event/event_dispatcher.h"
#include "event/touch_event.h"
#include "go/component_manager.h"

class ui_control;

class ui_manager : public component_manager_base<ui_manager> {
public:
    struct queryable {
        virtual ~queryable() {};
    };
    
    typedef std::unique_ptr<queryable> queryable_ptr;
    typedef Eigen::Vector2f vector2f;
    
public:
    ui_manager();
    
    template<class T>
    void set_queryable(T *);

    void set_camera(com::camera* cam) {
        _camera = cam;
    }
    
    void touch_began(touch_began_event const&);
    void touch_moved(touch_moved_event const&);
    void touch_ended(touch_ended_event const&);
    
private:
    
    queryable_ptr _queryable;
    com::camera* _camera;
};

#endif