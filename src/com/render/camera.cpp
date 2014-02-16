#include "com/render/camera.h"
#include "com/render/render_component_mgr.h"
#include "go/game_object.h"
#include "sg/transform.h"

using namespace com;

camera::camera(game_object* go, int priority)
: component(go), _priority(priority), _disabled(false),
_viewport(Eigen::Vector2i{0, 0}, Eigen::Vector2i{256, 256}),
_clear_color(.1f, .1f, .1f, 0.f),
_proj_view_inverse(matrix4f::Identity()),
_proj_mat(matrix4f::Identity()),
_proj_inverse(matrix4f::Identity()),
_uniform(make_uniforms_ptr({
    make_uniform<render_uniform::Mat4>("c_ProjViewMat", matrix4f::Identity())
})){
    render_component_mgr::instance().add_camera(this);
}

camera::~camera() {
    render_component_mgr::instance().remove_camera(this);
}

camera& camera::operator=(camera const& rhs) {
    _proj_mat = rhs._proj_mat;
    _proj_inverse = rhs._proj_inverse;
    _proj_view_inverse = rhs._proj_view_inverse;
    _uniform = std::make_shared<render_uniform>(*rhs.uniform().get());
    _target = rhs._target->retain<render_target>();
    _disabled = rhs._disabled;
    return *this;
}

camera::vector3f camera::unproject(vector3f const& pos) const {
	Eigen::Vector4f ret = _proj_view_inverse * Eigen::Vector4f{pos[0], pos[1], pos[2], 1.f};
    return vector3f(ret[0]/ret[3], ret[1]/ret[3], ret[2]/ret[3]);
}

camera::ray camera::cast_from_screen(vector2f const& pos) const {
    vector3f p(target()->normalize_position({pos[0], pos[1], 0.f}, viewport()));
	vector3f d(target()->normalize_position({pos[0], pos[1], 1.f}, viewport()));
    
	ray ret = {
        unproject(p), // p
        unproject(d), // d
    };
	
	(ret.d -= ret.p).normalize();
    return ret;
}

void camera::collect(std::vector<game_object*> const&) {
    
}

void camera::do_render(render_component_mgr const&) {
    
}

void camera::set_orthographic() {
    assert(0);
}

void camera::set_perspective(float left, float bottom, float right, float top, float near, float far) {
    float inv_w = 1.f / (right - left);
	float inv_h = 1.f / (top - bottom);
	float inv_d = 1.f / (far - near);
    
	float A = 2.f * near * inv_w;
	float B = 2.f * near * inv_h;
	float C = (right + left) * inv_w;
	float D = (top + bottom) * inv_h;
	float q, qn;
	q = - (far + near) * inv_d;
	qn = - 2.f * (far * near)  * inv_d;
    
	// NB: This creates 'uniform' perspective projection matrix,
	// which depth range [-1,1], right-handed rules
	//
	// [ A   0   C   0  ]
	// [ 0   B   D   0  ]
	// [ 0   0   q   qn ]
	// [ 0   0   -1  0  ]
	//
	// A = 2 * near / (right - left)
	// B = 2 * near / (top - bottom)
	// C = (right + left) / (right - left)
	// D = (top + bottom) / (top - bottom)
	// q = - (far + near) / (far - near)
	// qn = - 2 * (far * near) / (far - near)
    _proj_mat << A, 0.f, C, 0.f,
                    0.f, B, D, 0.f,
                    0.f, 0.f, q, qn,
    0.f, 0.f, -1.f,0.f;

    _uniform->set_matrix("c_ProjViewMat", _proj_mat);
    assert(0); // FIXME
}

void camera::update_from_transform() {
    if (parent()->is_set(com::transform_manager::flag_offset(),
                         com::transform_manager::mask_bit)) {
        update_matrix();
    }
}

void camera::update_matrix() {
    auto* trans = parent()->get_component<com::transform>();
    if (trans) {
        _proj_view_inverse = _proj_inverse * trans->global_affine().matrix();
        _uniform->set_matrix("c_ProjViewMat", matrix4f(_proj_mat * trans->global_inverse().matrix()));
    } else {
        _proj_view_inverse = _proj_inverse;
        _uniform->set_matrix("c_ProjViewMat", _proj_mat);
    }
}

camera& camera::set_perspective(float fovY, float aspect, float near, float far){
    float theta = fovY*0.5;
    float range = far - near;
    float invtan = 1./tan(theta);
    
    _proj_mat << invtan / aspect, 0.f, 0.f, 0.f,
                 0.f, invtan, 0.f, 0.f,
                 0.f, 0.f, -(near + far) / range, -2.f * near * far / range,
                 0.f, 0.f, -1.f, 0.f;
    
    _proj_inverse = _proj_mat.inverse();
    
    update_matrix();
    return *this;
}