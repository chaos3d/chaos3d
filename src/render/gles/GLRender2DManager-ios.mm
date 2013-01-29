#include "2d/gles/GLRender2DManager.h"
#include "2d/gles/GLRender2DManager-ios.h"
#include "2d/gles/GLRenderWindow-ios.h"

//using namespace chaos;

RenderWindow* GLRender2DManager::createWindow(){
	UIViewController* controller = mImpl->controller;
	ASSERT(controller != nil && "native context wasn't set up");
	CGRect rt = controller.view.bounds;	// TODO: view size
	UIView* view = [[[EAGLView alloc] initWithFrame: rt] autorelease];
	RenderWindow* rw = new GLRenderWindow(view, mImpl->createContext());
	rw->autorelease();
	[controller.view addSubview: view];
	return rw;
}

void GLRender2DManager::createImpl() {
	mImpl = new GLRender2DManagerImpl();
}

GLRender2DManager::~GLRender2DManager() {
	delete mImpl;
}
