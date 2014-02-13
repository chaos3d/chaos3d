#include "com/render/camera.h"
#include "com/render/render_component_mgr.h"

using namespace com;

camera::camera(game_object* go, int priority)
: component(go), _priority(priority), _disabled(false),
_viewport(Eigen::Vector2i{0, 0}, Eigen::Vector2i{256, 256}),
_clear_color(.1f, .1f, .1f, 0.f), _view_mat(matrix4f::Identity()),
_project_mat(matrix4f::Identity()),
_uniform(make_uniforms_ptr({
    make_uniform<render_uniform::Mat4>("c_ProjViewMat", matrix4f::Identity())
})){
    render_component_mgr::instance().add_camera(this);
}

camera::~camera() {
    render_component_mgr::instance().remove_camera(this);
}

camera& camera::operator=(camera const& rhs) {
    _project_mat = rhs._project_mat;
    _view_mat = rhs._view_mat;
    _target = rhs._target->retain<render_target>();
    _disabled = rhs._disabled;
    return *this;
}

camera::vector3f camera::unproject(vector3f const& pos) const {
#if 0
	Vector4f pos(
                 (viewPos[0]-vp.pos.x)/vp.size.width *2.f -1.f,
                 1.f-(viewPos[1]-vp.pos.y)/vp.size.height *2.f,
                 viewPos[2]*2.f -1.f,
                 1.f
                 );
    
	pos = mProjMatrixInv * pos;
    
	viewPos[0] = pos[0]/pos[3];
	viewPos[1] = pos[1]/pos[3];
	viewPos[2] = pos[2]/pos[3];
#endif
    return vector3f(); // TODO
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
    _project_mat << A, 0.f, C, 0.f,
                    0.f, B, D, 0.f,
                    0.f, 0.f, q, qn,
    0.f, 0.f, -1.f,0.f;
    _project_mat.transposeInPlace();

    _proj_view_mat = _project_mat * _view_mat;
    _proj_view_reverse = _proj_view_mat.reverse();
    _uniform->set_matrix("c_ProjViewMat", _proj_view_mat);
}

camera& camera::set_perspective(float fovY, float aspect, float near, float far){
    float theta = fovY*0.5;
    float range = far - near;
    float invtan = 1./tan(theta);
    
    _project_mat << invtan / aspect, 0.f, 0.f, 0.f,
                    0.f, invtan, 0.f, 0.f,
                    0.f, 0.f, -(near + far) / range, -2.f * near * far / range,
    0.f, 0.f, -1.f, 0.f;
    //_project_mat.transposeInPlace();
    
    _proj_view_mat = _project_mat * _view_mat;
    _proj_view_reverse = _proj_view_mat.reverse();
    _uniform->set_matrix("c_ProjViewMat", _proj_view_mat);
    return *this;
}