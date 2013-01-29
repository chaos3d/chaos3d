#include "2d/CameraNode.h"
#include "2d/Transform.h"

//using namespace chaos;

IMPLEMENT_CLASS(CameraNode, LIB_DOMAIN)
IMPLEMENT_END;

CameraNode::~CameraNode(){
	setTransform( new Transform(this) );
}

void CameraNode::update(){
	Scene2DNode::update();

	if( mTransform != 0 && transformFlag() )
		mTransformInv = mTransform->getMatrix().Inverse();
};

Matrix4f const& CameraNode::getTransformMatrix() const{
	if( mTransform == 0 )
		return Matrix4f::IDENTITY;
	else
		return mTransform->getMatrix();
};
