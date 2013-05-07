#include "RenderTargetView.h"

#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
#include <GLKit/GLKit.h>
#endif

@interface RenderView :
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
    GLKView
#else
    UIView
#endif
{
}

@property (assign, nonatomic) RenderTargetView* target;

@end

@implementation RenderView

@synthesize target;

- (void) dealloc{
    SAFE_RELEASE0(target);
    [super dealloc];
}

- (void) setTarget: (RenderTargetView*) target_{
    SAFE_REFAGN(target, target_);
}

#pragma earlier iOS version (prior to iOS 5) support

#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_5_0
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame context: (EGLContext*) _{
    if ((self = [super initWithFrame:frame])) {
		[self initLayer];
	}
    return self;
}

// TODO: different type of eag layer?
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

#endif // __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_5_0

- (void) touchesBegan: (NSSet *)touches withEvent: (UIEvent *)event{
	if( self.target == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseBegan )
			continue;
        
		CGPoint pt = [touch locationInView: self];

#if 0
        //TODO
		EventTouchBegan *evt = new EventTouchBegan(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
#endif
	}
}

- (void) touchesMoved: (NSSet *)touches withEvent: (UIEvent *)event{
	if( self.target == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseMoved )
			continue;
        
		CGPoint pt = [touch locationInView: self];
#if 0
		EventTouchMoved *evt = new EventTouchMoved(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
#endif
	}
}

- (void) touchesEnded: (NSSet *)touches withEvent: (UIEvent *)event{
	if(self.target == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseEnded )
			continue;
		
		CGPoint pt = [touch locationInView: self];
#if 0
		EventTouchEnded *evt = new EventTouchEnded(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
#endif
	}
	//window->getHandler().processEvent(window->getTouches());
}

- (void) touchesCancelled: (NSSet *)touches withEvent: (UIEvent *)event{
	if( self.target == 0)
		return;
	
	for( UITouch* touch in touches ){
		if( touch.phase != UITouchPhaseCancelled )
			continue;
		
		CGPoint pt = [touch locationInView: self];

#if 0
		EventTouchCancelled *evt = new EventTouchCancelled(Vector2f(pt.x, pt.y), 0.f /*TODO*/, touch.tapCount);
		evt->which = (int) touch;
		window->getHandler().queueEvent(evt);
		evt->release();
#endif
	}
	//window->getHandler().processEvent(window->getTouches());
}
@end
