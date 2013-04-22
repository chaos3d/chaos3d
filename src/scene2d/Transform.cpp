#include "Transform.h"
#include "Scene2DNode.h"

using namespace Wm4;
//using namespace chaos;
#if 0
IMPLEMENT_CLASS(Transform, LIB_DOMAIN )
IMPLEMENT_FUNC(relocate, &Transform::relocate )
IMPLEMENT_FUNC(update, &Transform::forceUpdate )
IMPLEMENT_FUNC( curTranslate, &Transform::getTranslate )
IMPLEMENT_FUNC( curScale, &Transform::getScale )
IMPLEMENT_FUNC( curRotation, &Transform::getRotate )
IMPLEMENT_FUNC( setTranslate, (void (Transform::*)(float,float,float))&Transform::setTranslate )
IMPLEMENT_FUNC( setRotate, (void (Transform::*)(float,float,float))&Transform::setRotate )
IMPLEMENT_FUNC( setQuaternion, (void (Transform::*)(float,float,float,float))&Transform::setRotate )
IMPLEMENT_FUNC( setScale, (void (Transform::*)(float,float,float))&Transform::setScale )
//IMPLEMENT_FUNC( translate, (AnimationState* (Transform::*)(float,float,float))&Transform::translate )
//IMPLEMENT_FUNC( rotate, (AnimationState* (Transform::*)(float,float,float))&Transform::rotate )

IMPLEMENT_PROP( translate, (void (Transform::*)(Vector3f const&))&Transform::setTranslate, &Transform::getTranslate)
IMPLEMENT_PROP( rotate, (void (Transform::*)(Quaternionf const&))&Transform::setRotate, &Transform::getRotate)
IMPLEMENT_PROP( scale, (void (Transform::*)(Vector3f const&))&Transform::setScale, &Transform::getScale)

IMPLEMENT_FUNC( linearTranslate, (&Transform::animation<Transform::TranslateApplier, LinearTiming, Vector3f>) )
IMPLEMENT_FUNC( linearRotate, (&Transform::animation<Transform::RotateApplier, LinearTiming, Quaternionf>) )
IMPLEMENT_FUNC( linearScale, (&Transform::animation<Transform::ScaleApplier, LinearTiming, Vector3f>) )
IMPLEMENT_FUNC( cubicTranslate, (&Transform::animation<Transform::TranslateApplier, CubicTiming, Vector3f>) )
IMPLEMENT_FUNC( cubicRotate, (&Transform::animation<Transform::RotateApplier, CubicTiming, Quaternionf>) )
IMPLEMENT_FUNC( cubicScale, (&Transform::animation<Transform::ScaleApplier, CubicTiming, Vector3f>) )
//IMPLEMENT_FUNC( nodeTranslate3, (&Transform::animation<TranslateApplierWrapper, CubicTiming>) )
//IMPLEMENT_FUNC( nodeScale, (&Transform::animation<ScaleApplierWrapper, LinearTiming>) )
//IMPLEMENT_FUNC( nodeScale3, (&Transform::animation<ScaleApplierWrapper, CubicTiming>) )

IMPLEMENT_END;


Transform::Transform(Scene2DNode *n, TypeLua const& lua)
: mRotate(1.f,0.f,0.f,0.f),mScale(1.f,1.f,1.f),mTranslate(0.f,0.f,0.f),
	_node(n)
{
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);

	lua_pushnil(L);
	while( lua_next(L, -2 ) ){
		if( lua_type( L, -2 ) == LUA_TSTRING ){
			char const* key = lua_tostring(L, -2);

			if( strcmp( key, "translate") == 0 ){
				LUA_GET3(L);
				setTranslate( 
					(float)lua_tonumber(L,-3),(float)lua_tonumber(L,-2),(float)lua_tonumber(L,-1)
				);
				lua_pop(L, 3);
			}
			
			else if( strcmp( key, "rotate") == 0 ){
				LUA_GET3(L);
				setRotate( (float)lua_tonumber(L,-3),
					(float)lua_tonumber(L,-2),
					(float)lua_tonumber(L,-1)
					);
				lua_pop(L, 3);
			}

			else if( strcmp( key, "scale") == 0 ){
				LUA_GET3(L);
				setScale(
					(float)lua_tonumber(L,-3),(float)lua_tonumber(L,-2),(float)lua_tonumber(L,-1)
					);
				lua_pop(L, 3);
			}
		};

		lua_pop(L,1);
	};

	lua_settop(L,top);
}
#endif

Transform::Transform(Scene2DNode* node):mRotate(1.f,0.f,0.f,0.f),mScale(1.f,1.f,1.f),mTranslate(0.f,0.f,0.f),
_node(node){
    assert(_node != 0);
    _node->setTransform(this);
}

void Transform::relocate(Transform* ot){
	Matrix4f global = ot->getMatrix().Inverse() * mGlobal;
	mScale = Vector3f(
                   sqrt(global(0,0)*global(0,0) + global(1,0)*global(1,0) + global(2,0)*global(2,0)),
                   sqrt(global(0,1)*global(0,1) + global(1,1)*global(1,1) + global(2,1)*global(2,1)),
                   sqrt(global(0,2)*global(0,2) + global(1,2)*global(1,2) + global(2,2)*global(2,2))
                   );
    
	mRotate.FromRotationMatrix(Matrix3f(
                    global(0,0)/mScale[0], global(1,0), global(2,0),
                    global(0,1), global(1,1)/mScale[1], global(2,1),
                    global(0,2), global(1,2), global(2,2)/mScale[2])
							   );
    
	mTranslate = Vector3f( global(0,3), global(1,3), global(2,3) );
	
	// global transform doesn't change
}

void Transform::forceUpdate(){
	Scene2DNode* parent = _node->getParent();
	if(parent != 0 && parent->getTransform())
		parent->getTransform()->forceUpdate();

	if(_node->transformFlag()){
		updateTransform();
		// Don't clear flag here since the parent may have other children that are not populated yet
		// _node->dirtyFlag() = _node->dirtyFlag() & (~Scene2DNode::D_TRANSFORM);
		// or populate the dirty flag to all it children
	}
}

void Transform::updateTransform(){
	float const* tuple(mRotate);
    float fTx  = (2.f)*tuple[1];
    float fTy  = (2.f)*tuple[2];
    float fTz  = (2.f)*tuple[3];
    float fTwx = fTx*tuple[0];
    float fTwy = fTy*tuple[0];
    float fTwz = fTz*tuple[0];
    float fTxx = fTx*tuple[1];
    float fTxy = fTy*tuple[1];
    float fTxz = fTz*tuple[1];
    float fTyy = fTy*tuple[2];
    float fTyz = fTz*tuple[2];
    float fTzz = fTz*tuple[3];

	Matrix4f mat( 
		(1.f-(fTyy+fTzz))*mScale.X(), (fTxy-fTwz)*mScale.Y(), (fTxz+fTwy)*mScale.Z(), mTranslate.X(),
		(fTxy+fTwz)*mScale.X(), (1.f-(fTxx+fTzz))*mScale.Y(), (fTyz-fTwx)*mScale.Z(), mTranslate.Y(),
		(fTxz-fTwy)*mScale.X(), (fTyz+fTwx)*mScale.Y(), (1.f-(fTxx+fTyy))*mScale.Z(), mTranslate.Z(),
		0.f, 0.f, 0.f, 1.f);

	if( _node->getParent() != 0 && _node->getParent()->getTransform() != 0 )
		mGlobal = _node->getParent()->getTransform()->mGlobal * mat;
	else
		mGlobal = mat;
}

void Transform::setTranslate(Vector3f const& vec){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mTranslate = vec;
}

void Transform::setTranslate(float x,float y,float z){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mTranslate = Vector3f(x,y,z);
}

void Transform::setRotate(Quaternionf const& rot){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	(mRotate = rot).Normalize();
}

void Transform::setRotate(float x,float y,float z){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mRotate = Quaternionf((float)DEGREE_TO_RADIAN(x),(float)DEGREE_TO_RADIAN(y),(float)DEGREE_TO_RADIAN(z));
}

void Transform::setRotate(float x,float y,float z, float angle){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mRotate = Quaternionf(Vector3f(x,y,z), angle);
}

void Transform::setScale(Vector3f const& scale){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mScale = scale;
}

void Transform::setScale(float x,float y,float z){
	_node->dirtyFlag() |= Scene2DNode::D_TRANSFORM;
	mScale = Vector3f(x,y,z);
}

#if 0
AnimationState* Transform::translate(float x, float y, float z){
	Vector3fAnimation* anim = new Vector3fAnimation();
	Vector3fAnimation::TKFList& kfs = anim->getKeyFrames();
	kfs.push_back( Vector3fAnimation::KeyFrame(mTranslate ,0.f) );
	kfs.push_back( Vector3fAnimation::KeyFrame(Vector3f(x,y,z),1.f) );

	AnimationState* as = anim->createState<TranslateApplier,CubicTiming>(TranslateApplier(this));

	anim->release();
	return as;
}

AnimationState* Transform::rotate(float x, float y, float z){
	QuaternionAnimation* anim = new QuaternionAnimation();
	QuaternionAnimation::TKFList& kfs = anim->getKeyFrames();
	kfs.push_back( QuaternionAnimation::KeyFrame(mRotate,0.f) );
	kfs.push_back( QuaternionAnimation::KeyFrame(
		Quaternionf((float)DEGREE_TO_RADIAN(x),(float)DEGREE_TO_RADIAN(y),(float)DEGREE_TO_RADIAN(z)),
		1.f) );

	AnimationState* as = anim->createState<RotateApplier,CubicTiming>(RotateApplier(this));

	anim->release();
	return as;
}
#endif
