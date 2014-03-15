#ifndef _COM_CAMERA_H
#define _COM_CAMERA_H

#include <Eigen/Geometry>
#include "common/common.h"
#include "common/referenced_count.h"
#include "go/component.h"
#include "go/component_manager.h"
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
        typedef Eigen::AlignedBox2i rect2d;
        typedef Eigen::Vector2f vector2f;
        typedef Eigen::Vector3f vector3f;
        typedef Eigen::Vector4f color_t;
        
        struct ray {
            // R(t) = p + t*d;
            vector3f p;
            vector3f d; // normalized direction
        };
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
        camera& set_perspective(float fovY, float aspect, float near, float far);
        void set_perspective(float left, float bottom, float right, float top, float near, float far);
        
        bool operator< (camera const& rhs) const {
            return _priority < rhs.priority();
        }
        
        camera& set_target(render_target* target) {
            _target = target->retain<render_target>(); // TODO: weak reference?
            return *this;
        }

        camera& set_viewport_from_target() {
            set_viewport(Eigen::Vector2i{0,0},
                         Eigen::Vector2i{_target->size().x(),_target->size().y()});
            return *this;
        }
        
        // get the projected view size at the given z distant
        vector2f size_at(float z = 0.f) const;
        
        // client(screen) space to the world space
        vector3f unproject(vector3f const&) const;
        
        // cast a ray from a screen point (a.k.a camera point)
        ray cast_from_screen(vector2f const&) const;
        
        // the projection matrix
        matrix4f const& proj_matrix() const { return _proj_mat; }
        
        // the z-/depth value for the plane where the height
        // is in pixels based on the current target
        // and the perspective matrix
        float distant_for_perfect_pixel(float height = 0.f) const {
            return (height <= FLT_EPSILON ? target()->size().y() : height)
            * .5f * _proj_mat(1,1);
        }
        
        // move the camera to where it is pixel perfect
        // on the plane z = 0
        // NB: if transform doesn't exist, one will be created
        camera& move_for_perfect_pixel(float height = 0.f);
        
    protected:
        camera& operator=(camera const&);
        virtual ~camera();
        renderables_t& renderables() { return _renderables; };
        render_target::ptr target() const { return _target->retain<render_target>(); };
        render_uniform::ptr uniform() const { return _uniform; }
        void update_matrix();
        void update_from_transform();
        
    private:
        render_uniform::ptr _uniform;
        matrix4f _proj_view_inverse;
        matrix4f _proj_mat, _proj_inverse;
        render_target::ptr _target;
        renderables_t _renderables;
        
        ATTRIBUTE(rect2d, viewport, rect2d());
        ATTRIBUTE(color_t, clear_color, color_t());
        ATTRIBUTE(bool, disabled, false);
        ATTRIBUTE(int, priority, 0); // render order, smaller is higher

        SIMPLE_CLONE(camera);
    };
    
}
#endif