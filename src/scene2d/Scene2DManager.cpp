#include "2d/Scene2DManager.h"
#include "2d/Scene2DNode.h"
#include "2d/Scene2DHandler.h"
#include "2d/CameraNode.h"
#include "2d/PptCameraNode.h"
#include "2d/NodeFrameGlobal.h"

#include "2d/render.h"
#include "script/bind.h"
#include <stack>

//using namespace chaos;
using namespace std;

IMPLEMENT_CLASS(Scene2DManager, LIB_DOMAIN )
IMPLEMENT_PROP(camera, &Scene2DManager::setCamera, &Scene2DManager::getCamera )
IMPLEMENT_PROP(root, 0, &Scene2DManager::getRoot )
IMPLEMENT_FUNC(forceRender, &Scene2DManager::renderScene)
IMPLEMENT_FUNC(unproject, &Scene2DManager::unproject)
IMPLEMENT_FUNC(orientationChanged, &Scene2DManager::orientationChanged )
IMPLEMENT_FUNC(clear, &Scene2DManager::setClear )
IMPLEMENT_FUNC(createNode, (Scene2DNode* (Scene2DManager::*)(TypeLua const&))&Scene2DManager::createNode )
IMPLEMENT_END;

class SceneUpdater{
public:
	inline void operator() (Scene2DNode* node) const{
		node->update();
	};
};

class SceneClearFlag{
public:
	inline void operator() (Scene2DNode* node) const{
		node->clearFlag();
	};
};

class SceneRenderer{
	Render2DManager	*mRM;
public:
	SceneRenderer(Render2DManager *rm)
		: mRM(rm)
	{};

	inline void operator() (Scene2DNode* node) const{
		if( node->getSprite() != 0 )
			mRM->drawSprite( node->getSprite() );
	};
};

Scene2DManager::Scene2DManager() :
	mRootNode( new Scene2DNode("root") ), mCamera(new PptCameraNode("ppt camera")),
	mClearColor(0.2f,.3f,.5f,1.f), mVP( 0,0,480,320), mHandler(0)
{
	//((PptCameraNode*)mCamera)->setPerpective();
	Render2DManager *r2d = Render2DManager::getInstance();
	mMainWindow = r2d->createWindow( );
	mMainWindow->retain();
	mMainWindow->resize(false);

	mHandler = new Scene2DHandler(this);
	mMainWindow->setHandler(mHandler);

	adjustViewport();
	mRootNode->setFrame( new NodeFrameGlobal(mRootNode) );
}

Scene2DManager::~Scene2DManager(){
	// TODO: set scene to nil for handler
	SAFE_RELEASE0(mHandler);
	SAFE_RELEASE0(mCamera);
	SAFE_RELEASE0(mMainWindow);
	SAFE_RELEASE0(mRootNode);
}

void Scene2DManager::setCamera(CameraNode* camera){
	SAFE_REFAGN(mCamera, camera);
	mRootNode->addChild( mCamera );
}

void Scene2DManager::setClear( float r, float g, float b, float a){
	mClearColor = ColorRGBA(r,g,b,a);
}

void Scene2DManager::update(){
	visit( SceneUpdater() );
	visit( SceneClearFlag() );
}

void Scene2DManager::adjustViewport(){
	/*
	float width(320.f), height(480.f);

	int device( Device::getInstance()->getDevice() );
	int type( Device::getInstance()->getOrientation() );

	if( device == iPhone3 ){
		width = 480.f, height = 320.f;
	}else if( device == iPhone4 ){
		width = 960.f, height = 640.f;
	}else if( device == iPad ){
		width = 1024.f, height = 768.f;
	}

	if( type == PORTRAIT_UP || type == PORTRAIT_DOWN )
		std::swap( width, height );*/

	Rectanglef const& size = mMainWindow->getSize();
	
	mVP.pos.x = size.Origin.X();
	mVP.pos.y = size.Origin.Y();
	mVP.size.width = size.Extent[0];
	mVP.size.height = size.Extent[1];
}

void Scene2DManager::orientationChanged( ){
	mMainWindow->resize( true );
	adjustViewport();
}

void Scene2DManager::renderScene(){
	Render2DManager *r2d = Render2DManager::getInstance();
	RenderWindow *rw = mMainWindow;

	rw->bind();
	r2d->setModelMatrix( mCamera->getTransformInvMatrix() );
	r2d->setProjectiveMatrix( mCamera->getProjectiveMatrix() );

	r2d->setViewport( mVP );
	r2d->clear( Render2DManager::CLEAR_COLOR|Render2DManager::CLEAR_DEPTH,
		mClearColor);

	r2d->beginRender();
	SceneRenderer sr(r2d);
	visit( sr );
	r2d->endRender();

	rw->flush();
}

template<class F>
void Scene2DManager::visit( F const& visitor ){
	if( mRootNode->mFirstChild == 0 )
		return ;

	stack<Scene2DNode*> nodes;
	nodes.push( mRootNode );

	Scene2DNode* node = nodes.top();

	do{
		visitor( node );

		nodes.pop();
		if( node->mNextSibling != 0 )
			nodes.push( node->mNextSibling );

		if( node->mFirstChild != 0 )
			nodes.push( node->mFirstChild );

		if( nodes.empty() )
			break;
		else
			node = nodes.top();
	}while(1);
}

/*
Scene2DNode* Scene2DManager::pick(Vector2f const& pos, Vector3f& q, int& ret){
	ASSERT( mCamera != 0 );
	Rect vp( 0,0,480,320);

	Vector4f p4(pos[0],pos[1], 0.f, 1.f);
	Vector4f d4(pos[0],pos[1], 1.f, 1.f);
	p4 = p4 * mCamera->getTransformMatrix();
	d4 = d4 * mCamera->getTransformMatrix();

	Vector3f p(p4), d(d4);
	mCamera->unproject( p, vp );
	mCamera->unproject( d, vp );
	(d -= p).Normalize();
	
	Scene2DNode* pick = mRootNode->getFrame()->pick( p, d, q, ret );

	return pick;
}

void Scene2DManager::unproject( Vector3f &pos){
	ASSERT( mCamera != 0 );
	Rect vp( 0,0,480,320);

	Vector4f p4(pos[0],pos[1], pos[2], 1.f);
	pos = p4 * mCamera->getTransformInvMatrix();

	mCamera->unproject( pos, vp );
}
*/
Scene2DNode* Scene2DManager::pick(Vector2f const& pos, Vector3f& q, int& ret){
 	ASSERT( mCamera != 0 );

	Vector3f p(pos[0],pos[1], 0.f);
	Vector3f d(pos[0],pos[1], -1.f);
	mCamera->unproject( p, mVP );
	mCamera->unproject( d, mVP );

	p = mCamera->getTransformMatrix() * Vector4f(p[0],p[1],p[2],1.f);
	d = mCamera->getTransformMatrix() * Vector4f(d[0],d[1],d[2],1.f);
	
	(d -= p).Normalize();
	Scene2DNode* pick = mRootNode->getFrame()->pick( p, d, q, ret );

	return pick;
}

void Scene2DManager::unproject( Vector3f &pos){
	ASSERT( mCamera != 0 );

	mCamera->unproject( pos, mVP );
	pos = mCamera->getTransformMatrix() * Vector4f(pos[0],pos[1],pos[2],1.f);

}
