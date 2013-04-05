#include "2d/PptCameraNode.h"
#include "script/bind.h"
#include "script/lua.h"

//using namespace chaos;

IMPLEMENT_CLASS(PptCameraNode, LIB_DOMAIN)
IMPLEMENT_FACTORY(LuaRefFactory<PptCameraNode> )
IMPLEMENT_FUNC(setPerspective, &PptCameraNode::setPerpective)
IMPLEMENT_END;

PptCameraNode::PptCameraNode(TypeLua const& lua) : CameraNode("ppt camera"){
	lua_State* L(lua.getL());

	lua.get(L);
	if( !lua_istable(L,-1) ){
		LOG("Expected a table. (PptCameraNode)");
		lua_pop(L,1);
		return;
	}

	lua_rawgeti( L, -1, 1); float fov( (float)lua_tonumber(L,-1));
	lua_rawgeti( L, -2, 2); float aspect( (float)lua_tonumber(L,-1));
	lua_rawgeti( L, -3, 3); float near( (float)lua_tonumber(L,-1));
	lua_rawgeti( L, -4, 4); float far( (float)lua_tonumber(L,-1));

	setPerpective( fov, aspect, near, far );
}

void PptCameraNode::parseLua(TypeLua const& lua){
	Scene2DNode::parseLua(lua);

	lua_State* L(lua.getL());
	int n = lua_gettop( L );
	lua.get(L);
	ASSERT( lua_istable(L,-1) );

	lua_pushstring(L, "camera" );
	lua_rawget(L,-2);
	if(lua_istable(L,-1)){
		lua_rawgeti( L, -1, 1); float fov( (float)lua_tonumber(L,-1));
		lua_rawgeti( L, -2, 2); float aspect( (float)lua_tonumber(L,-1));
		lua_rawgeti( L, -3, 3); float near( (float)lua_tonumber(L,-1));
		lua_rawgeti( L, -4, 4); float far( (float)lua_tonumber(L,-1));

		setPerpective( fov, aspect, near, far );
	}
	lua_settop(L,n);
}

void PptCameraNode::unproject(Vector3f& viewPos, cRect const& vp) const{
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
}

void PptCameraNode::setPerspectiveProj(const float left,const float bottom,const float right,const float top,const float near,const float far){
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

void PptCameraNode::setPerpective(const float fov,const float aspect,const float near,const float far){
	float fovr = (float)DEGREE_TO_RADIAN( fov );
	float right = tanf(fovr/2.f) * near, top = tanf(fovr/2.f) * near/aspect;
  	setPerspectiveProj( -right, -top, right, top, near, far);
}

