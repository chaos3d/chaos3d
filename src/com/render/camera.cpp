#include "com/render/camera.h"
#include "com/render/render_component_mgr.h"

using namespace com;

camera::camera(game_object* go, int priority)
: component(go), _priority(priority), _disabled(false) {
    render_component_mgr::instance().add_camera(this);
}

camera::~camera() {
    render_component_mgr::instance().remove_camera(this);
}

camera& camera::operator=(camera const& rhs) {
    _project_mat = rhs._project_mat;
    _view_mat = rhs._view_mat;
    _target = rhs._target;
    _disabled = rhs._disabled;
    return *this;
}

void camera::collect(std::vector<game_object*> const&) {
    
}

void camera::do_render(render_component_mgr const&) {
    
}
