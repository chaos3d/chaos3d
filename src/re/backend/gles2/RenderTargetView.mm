#include "RenderTargetView.h"

@interface RenderView : UIView{
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
    eaglLayer.contentScaleFactor = [UIScreen mainScreen].scale; //TODO
	
	self.userInteractionEnabled = YES;  // TODO
	self.opaque = TRUE;
	self.autoresizesSubviews = NO;
	self.autoresizingMask = UIViewAutoresizingNone;
    self.multipleTouchEnabled = TRUE; // TODO

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

RenderTargetView::RenderTargetView(UIView* parent, uint16_t width, uint16_t height){
    _view = [[RenderView alloc] initWithFrame:CGRectMake(0.f, 0.f, width, height)];
    _view.target = this;
    
    [parent addSubview: _view];
}

RenderTargetView::~RenderTargetView(){
    [_view removeFromSuperview];
    [_view release];
}

RenderTarget::Size RenderTargetView::getSize() const{
    CGSize size = _view.bounds.size;
    float scale = _view.contentScaleFactor;
    return RenderTarget::Size(size.width*scale, size.height*scale);
}