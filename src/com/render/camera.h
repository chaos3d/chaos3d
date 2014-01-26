#ifndef _COM_CAMERA_H
#define _COM_CAMERA_H

#include "Eigen/Dense"
#include "go/component.h"
#include "common/common.h"
#include "re/render_target.h"
#include <vector>
#include <forward_list>

class game_object;

namespace com {
    class renderable;
    class render_component_mgr;
    
    // represent a rendering algorithm
    // it will collect all visible renderables, and render to the
    // target
    class camera : public component {
    public:
        typedef std::forward_list<renderable*> renderables_t;
        typedef Eigen::Matrix4f matrix4f;
        
    public:
        camera();
        
        // collect all the visual components (renderables)
        // there can be various algorithms (aabb-tree, or bullet3d?)
        virtual void collect(std::vector<game_object*> const&);
        
        // renders to the target
        // render_mgr would be used as an environmental reference
        // i.e. the lights/shadows etc
        virtual void do_render(render_component_mgr const&);
        
        void set_orthographic();
        void set_perspective();
        
    private:
        matrix4f _project_mat;
        matrix4f _view_mat;
        render_target::ptr _target;
        renderables_t _renderables;
        
        ATTRIBUTE(bool, disabled);
    };
}
#endif