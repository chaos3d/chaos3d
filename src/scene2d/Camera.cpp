#include "Camera.h"
#include "Scene2DNode.h"

Camera::Camera(Scene2DNode* node): _node(node), _transformInv(Matrix4f::IDENTITY),
_projMatrix(Matrix4f::IDENTITY), _projMatrixInv(Matrix4f::IDENTITY)
{
}

Camera::~Camera(){
    SAFE_RELEASE0(_rootNode);
}

void Camera::setRootNode(Scene2DNode* rootNode){
    // won't assign to itself
    if(rootNode == _node)
        return;
    
    SAFE_REFAGN(rootNode, _rootNode);
}

void Camera::viewportToWorld(Vector3f& viewPos, Viewport const& vp) const{
    Wm4::Vector4f pos(
                 (viewPos[0]-vp.x)/vp.width *2.f -1.f,
                 1.f-(viewPos[1]-vp.y)/vp.height *2.f,
                 viewPos[2]*2.f -1.f,
                 1.f
                 );
    
	pos = _projMatrixInv * pos;
    
	viewPos[0] = pos[0]/pos[3];
	viewPos[1] = pos[1]/pos[3];
	viewPos[2] = pos[2]/pos[3];
}

void Camera::setOrthoProj(const float left,const float bottom,const float right,const float top,
                          const float near,const float far){
	float inv_w = 1.f / (right - left);
	float inv_h = 1.f / (top - bottom);
	float inv_d = 1.f / (far - near);
    
	float A = 2.f * inv_w;
	float B = 2.f * inv_h;
	float C = - (right + left) * inv_w;
	float D = - (top + bottom) * inv_h;
	float q, qn;
	q = - 2.f * inv_d;
	qn = - (far + near)  * inv_d;
    
	// NB: This creates 'uniform' orthographic projection matrix,
	// which depth range [-1,1], right-handed rules
	//
	// [ A   0   0   C  ]
	// [ 0   B   0   D  ]
	// [ 0   0   q   qn ]
	// [ 0   0   0   1  ]
	//
	// A = 2 * / (right - left)
	// B = 2 * / (top - bottom)
	// C = - (right + left) / (right - left)
	// D = - (top + bottom) / (top - bottom)
	// q = - 2 / (far - near)
	// qn = - (far + near) / (far - near)
    
	_projMatrix = Matrix4f::ZERO;
	_projMatrix[0][0] = A;
	_projMatrix[0][3] = C;
	_projMatrix[1][1] = B;
	_projMatrix[1][3] = D;
	_projMatrix[2][2] = q;
	_projMatrix[2][3] = qn;
	_projMatrix[3][3] = 1;
    
	_projMatrixInv = _projMatrix.Inverse();
}

void Camera::setPerspectiveProj(const float left,const float bottom,const float right,const float top,const float near,const float far){
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
	_projMatrix = Matrix4f::ZERO;
	_projMatrix[0][0] = A;
	_projMatrix[0][2] = C;
	_projMatrix[1][1] = B;
	_projMatrix[1][2] = D;
	_projMatrix[2][2] = q;
	_projMatrix[2][3] = qn;
	_projMatrix[3][2] = -1;
    
	_projMatrixInv = _projMatrix.Inverse();
}

void Camera::setPerspective(const float fov,const float aspect,const float near,const float far){
	float fovr = (float)DEGREE_TO_RADIAN( fov );
	float right = tanf(fovr/2.f) * near, top = tanf(fovr/2.f) * near/aspect;
  	setPerspectiveProj( -right, -top, right, top, near, far);
}

void Camera::setOrtho(const float width,const float height,const float near,const float far){
	setOrthoProj( 0.f, 0.f, width, height, near, far);
}

void Camera::render(){
    
}