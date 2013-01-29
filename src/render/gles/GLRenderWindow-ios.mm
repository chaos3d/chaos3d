#include "2d/gles/GLRenderWindow-ios.h"
//#include "app/app.h"
#include "event/evt.h"
#include "2d/gles/GLRender2DManager.h"
#include <algorithm>

//using namespace chaos;

@implementation EAGLView

@synthesize ptTransform;

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void) setWindow:(GLRenderWindow *) w{
	window = w;
}

// only programmatically create the view
// - (id)initWithCoder:(NSCoder*)coder

- (id) initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {        
		[self initLayer];
	}
    return self;
}

- (void) layoutSubviews{
	[super layoutSubviews];
}

- (void) initLayer{
	// Get the layer
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
	
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	self.userInteractionEnabled = YES;
	self.opaque = TRUE;
	self.autoresizesSubviews = NO;
	self.autoresizingMask = UIViewAutoresizingNone;
	//self.backgroundColor = [UIColor redColor];
}

- (void) touchesBegan: (NSSet *)touches withEvent: (UIEvent *)event{
	if( window == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseBegan )
			continue;

		//CGPoint pt = CGPointApplyAffineTransform( [touch locationInView: self], ptTransform);
		CGPoint pt = [touch locationInView: self];
		
		//TouchEvent::Touch& etouch = window->getTouches().update(touch);
		//etouch = TouchEvent::Touch(
		//						   app->global(),touch.tapCount,TouchEvent::TouchBegan, 
		//						   Vector2f(pt.x, pt.y));
		
		//it->onTouchBegan(window->getTouches(), etouch);
		EventTouchBegan *evt = new EventTouchBegan(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
	}
}

- (void) touchesMoved: (NSSet *)touches withEvent: (UIEvent *)event{	
	if( window == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseMoved )
			continue;

		CGPoint pt = [touch locationInView: self];
		/*
		CGPoint pt = CGPointApplyAffineTransform( [touch locationInView: self], ptTransform);

		TouchEvent::Touch& etouch = window->getTouches().update(touch);
		etouch.phase = TouchEvent::TouchMoved;
		etouch.lastPos = etouch.nowPos;
		etouch.nowPos = Vector2f(pt.x,pt.y);
		it->onTouchMoved(window->getTouches(), etouch);
		etouch.phase = TouchEvent::TouchNormal;
		 */
		EventTouchMoved *evt = new EventTouchMoved(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
	}
}

- (void) touchesEnded: (NSSet *)touches withEvent: (UIEvent *)event{
	if(window == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseEnded )
			continue;
		
		CGPoint pt = [touch locationInView: self];
		/*
		CGPoint pt = CGPointApplyAffineTransform( [touch locationInView: self], ptTransform);

		TouchEvent::Touch& etouch = window->getTouches().update(touch);
		etouch.phase = TouchEvent::TouchEnded;
		etouch.lastPos = etouch.nowPos;
		etouch.nowPos = Vector2f(pt.x,pt.y);
		it->onTouchEnded(window->getTouches(), etouch);
		etouch.phase = TouchEvent::TouchNormal;
		 */
		EventTouchEnded *evt = new EventTouchEnded(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
	}
	//window->getHandler().processEvent(window->getTouches());
}

- (void) touchesCancelled: (NSSet *)touches withEvent: (UIEvent *)event{
	if( window == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseCancelled )
			continue;
		
		CGPoint pt = [touch locationInView: self];
		/*
		CGPoint pt = [touch locationInView: nil];
		TouchEvent::Touch& etouch = window->getTouches().update(touch);
		etouch.phase = TouchEvent::TouchCancelled;
		etouch.lastPos = etouch.nowPos;
		etouch.nowPos = Vector2f(pt.x,pt.y);
		it->onTouchCancelled(window->getTouches(), etouch);
		etouch.phase = TouchEvent::TouchNormal;
		 */
		EventTouchCancelled *evt = new EventTouchCancelled(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
	}
	//window->getHandler().processEvent(window->getTouches());
}
@end

GLRenderWindow::GLRenderWindow(EAGLView* view, EAGLContext* context) : mView([view retain]), mContext([context retain]){
	create();
};

GLRenderWindow::~GLRenderWindow(){
	[mView release];
	[mContext release];
}

void GLRenderWindow::create(){
	ASSERT(GLRender2DManager::getInstance() != 0);
	
	float width(mView.bounds.size.width), height(mView.bounds.size.height);	
	LOG("The view (0,0,%3.1f,%3.1f)", width, height);
	
	EAGLContext *context = mContext;
	[EAGLContext setCurrentContext: context];
	
	glGenFramebuffersOES(1, &mFrameBufferID);
	glGenRenderbuffersOES(1, &mRenderBufferID);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, mFrameBufferID);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, mRenderBufferID);	
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)mView.layer];

	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, mRenderBufferID);

    GLint framebufferWidth;
    GLint framebufferHeight;
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &framebufferWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &framebufferHeight);

	glGenRenderbuffersOES(1, &mDepthBufferID);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, mDepthBufferID);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, framebufferWidth, framebufferHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, mDepthBufferID);
	
	[mView setWindow: this];
	
	if( glGetError() != GL_NO_ERROR )
		LOG("Unable to create frame and render buffers");
	
	mSize = Rectanglef( Vector2f(0.f,0.f), width, height );
}

void GLRenderWindow::resize(bool anim){
	/*
	if( anim )
		[UIView beginAnimations: nil context: nil];
	
	float width(320.f), height(480.f);
	Device::getInstance()->getSize(width, height);
	
	CGRect screenSize = [UIScreen mainScreen].bounds;
	EAGLView *view = (EAGLView*) mView;
	view.transform = CGAffineTransformIdentity;
	// move to the center of the screen
	view.center = CGPointMake( screenSize.size.width/2.f, screenSize.size.height/2.f);
	view.bounds = CGRectMake( -width/2.f,-height/2.f, width, height);
	//view.frame = (CGRect){-width/2.f,-height/2.f,width,height};
	
	switch(Device::getInstance()->getOrientation())
	{
		case PORTRAIT_UP:
			view.ptTransform = CGAffineTransformMakeRotation( DEGREE_TO_RADIAN(0.f) );
			view.transform = view.ptTransform;
			//view.transform = CGAffineTransformTranslate( view.ptTransform, width/2.f, height/2.f);
			break;
		case PORTRAIT_DOWN:
			view.ptTransform = CGAffineTransformMakeRotation( DEGREE_TO_RADIAN(180.f) );
			view.transform = view.ptTransform;
			//view.transform = CGAffineTransformTranslate( view.ptTransform, -width/2.f, -height/2.f);
			break;
		case LANDSCAPE_LEFT:
			view.transform = CGAffineTransformMakeRotation( DEGREE_TO_RADIAN(90.f) );
			view.ptTransform = CGAffineTransformTranslate(view.transform, 0.f, -height);
			
			//view.transform = CGAffineTransformTranslate( view.ptTransform, width/2.f, -height/2.f);
			//view.ptTransform = CGAffineTransformTranslate( view.ptTransform, 0.f, -height);
			//view.ptTransform = CGAffineTransformScale( view.ptTransform, -1.f, 1.f);
			break;
		case LANDSCAPE_RIGHT:
			view.transform = CGAffineTransformMakeRotation( DEGREE_TO_RADIAN(-90.f) );
			view.ptTransform = CGAffineTransformTranslate(view.transform, -width, 0.f);
			
			//view.transform = CGAffineTransformTranslate( view.ptTransform, -width/2.f, height/2.f);
			//view.ptTransform = CGAffineTransformTranslate( view.ptTransform, -width, 0.f);
			break;
	default:
		break;
	}
	view.ptTransform = CGAffineTransformInvert( view.ptTransform );
	mSize = Rectanglef( Vector2f(0.f,0.f), width, height );
	//mSize = Rectanglef( Vector2f(-(800.f-1024.f)/2.f,-(600.f-height)/2.f), 800.f, 600.f );

	if( anim )
		[UIView commitAnimations];	
	 */
}

void GLRenderWindow::bind(){
    [EAGLContext setCurrentContext:(EAGLContext*)mContext];
	
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, mFrameBufferID);
}

void GLRenderWindow::flush(){
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, mRenderBufferID);
	[mContext presentRenderbuffer:GL_RENDERBUFFER_OES];
}
