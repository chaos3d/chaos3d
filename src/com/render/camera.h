#ifndef _COM_CAMERA_H
#define _COM_CAMERA_H

#include "Eigen/Dense"
#include "go/component.h"
#include "go/component_manager.h"
#include "common/common.h"
#include "re/render_target.h"
#include <vector>
#include <forward_list>

namespace com {
    class renderable;
    class render_component_mgr;
    // TODO: culling interface
    
    // represent a rendering algorithm
    // it will collect all visible renderables, and render to the
    // target
    class camera : public component {
    public:
        typedef nil_component_mgr<std::false_type> manager_t;
        typedef std::forward_list<renderable*> renderables_t;
        typedef Eigen::Matrix4f matrix4f;
        
    public:
        camera(game_object*, int priority = 0);
        
        // collect all the visual components (renderables)
        // there can be various algorithms (aabb-tree, or bullet3d?)
        virtual void collect(std::vector<game_object*> const&);
        
        // renders to the target
        // render_mgr would be used as an environmental reference
        // i.e. the lights/shadows etc
        virtual void do_render(render_component_mgr const&);
        
        void set_orthographic();
        void set_perspective(float fovY, float aspect, float near, float far);
        
        bool operator< (camera const& rhs) const {
            return _priority < rhs.priority();
        }
        
        void set_clear(render_target::color_t const& color) {
            _target->set_clear_color(color);
        }
        
    protected:
        camera& operator=(camera const&);
        virtual ~camera();
        renderables_t& renderables() { return _renderables; };
        render_target::ptr target() const { return _target; };
        
    private:
        matrix4f _project_mat;
        matrix4f _view_mat;
        render_target::ptr _target;
        renderables_t _renderables;
        
        ATTRIBUTE(bool, disabled);
        ATTRIBUTE(int, priority); // render order, smaller is higher

        SIMPLE_CLONE(camera);
    };
    
}
#endif