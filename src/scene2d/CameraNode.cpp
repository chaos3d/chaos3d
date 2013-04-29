#include "CameraNode.h"
#include "Transform.h"
#include "Camera.h"

//using namespace chaos;

CameraNode::~CameraNode(){
	SAFE_DELETE0(_camera);
}

void CameraNode::doRender() {
    if(_camera != NULL){
        _camera->render();
    }
}

void CameraNode::setCamera(Camera* camera){
    if(_camera == camera)
        return;
    SAFE_DELETE(_camera);
    _camera = camera;
    assert(camera->getNode() == this);
}
#if 0
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

#endif