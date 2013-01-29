#include "2d/Viewport.h"
#include "2d/Renderable.h"
#include <math.h>

//using namespace chaos;

//IMPLEMENT_CLASS(Viewport,LIB_DOMAIN)
//IMPLEMENT_END;

void Viewport::setOrthoProj(const float left,const float bottom,const float right,const float top,const float near,const float far){
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

	mProjMatrix = Matrix4f::ZERO;
	mProjMatrix[0][0] = A;
	mProjMatrix[0][3] = C;
	mProjMatrix[1][1] = B;
	mProjMatrix[1][3] = D;
	mProjMatrix[2][2] = q;
	mProjMatrix[2][3] = qn;
	mProjMatrix[3][3] = 1;

	mProjMatrixInv = mProjMatrix.Inverse();
}

void Viewport::unProject(Vector3f& viewPos) const{
	Vector4f pos(
		(viewPos[0]-mViewport.pos.x)/mViewport.size.width *2.f -1.f,
		1.f-(viewPos[1]-mViewport.pos.y)/mViewport.size.height *2.f,
		viewPos[2]*2.f -1.f,
		1.f
		);

	pos = mProjMatrixInv * pos;

	viewPos[0] = pos[0]/pos[3];
	viewPos[1] = pos[1]/pos[3];
	viewPos[2] = pos[2]/pos[3];
}

void Viewport::setPerspectiveProj(const float left,const float bottom,const float right,const float top,const float near,const float far){
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
	mProjMatrix = Matrix4f::ZERO;
	mProjMatrix[0][0] = A;
	mProjMatrix[0][2] = C;
	mProjMatrix[1][1] = B;
	mProjMatrix[1][2] = D;
	mProjMatrix[2][2] = q;
	mProjMatrix[2][3] = qn;
	mProjMatrix[3][2] = -1;

	mProjMatrixInv = mProjMatrix.Inverse();
}

void Viewport::setPerpective(const float fov,const float aspect,const float near,const float far){
	float right = tanf(fov/2.f) * near, top = tanf(fov/2.f) * near/aspect;
  	setPerspectiveProj( -right, -top, right, top, near, far);
}

void Viewport::setOrtho(const float width,const float height,const float near,const float far){
	setOrthoProj( 0.f, 0.f, width, height, near, far);
}


void Viewport::addTransparentRenderable(Renderable const& rd){
	mTransparentRenderables.push_back( rd );
}

void Viewport::doRender(){
	for( TRendererList::iterator it = mRenderers.begin();
		it != mRenderers.end();
		++ it)
	{
		(*it)->doRender(this);
	}
}

void Viewport::addRenderer(Renderer::Ptr rd){
	mRenderers.push_back(rd);
}

Viewport::~Viewport(){
}

