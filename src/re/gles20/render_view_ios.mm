#include "render_view_ios.h"

#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

@interface EAGLView : UIView

@property (nonatomic, assign) gles20::render_view* host;

@end

@implementation EAGLView

@synthesize host;

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

// TODO: passing events to host
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
    
}

@end

namespace gles20 {

render_view::render_view(target_size_t const& size_, window_pos_t const& pos_) :
    render_window(size_, pos_), _native_view(nil) {
    _context = [EAGLContext currentContext];
    assert(_context != nil);
}

render_view::~render_view() {
    [_native_view release];
}

void render_view::create_native() {
    assert(_native_view == nil);
    _native_view = [[EAGLView alloc] initWithFrame: CGRectMake(_position(0), _position(1),
                                                               _size(0), _size(1))];
    _native_view.host = this;
	_native_view.userInteractionEnabled = YES;
	_native_view.opaque = TRUE;
	_native_view.autoresizesSubviews = NO;
	_native_view.autoresizingMask = UIViewAutoresizingNone;
    
}
    
void render_view::create_view() {
    EAGLContext* context = _context;

    // TODO: format, gl error check, multi sample

    // frame buffer
    glGenFramebuffers(1, &_framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);

    // color buffer
    glGenRenderbuffers(1, &_colorbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorbuffer_id);
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)_native_view.layer;
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
	[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorbuffer_id);

    GLint width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    // depth buffer
    glGenRenderbuffers(1, &_depthbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height); // TODO: depth format
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer_id);
    
    // stencil
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);

	//glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE
    
}

bool render_view::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);
    return true;
}
    
bool render_view::flush() {
    glBindRenderbuffer(GL_RENDERBUFFER, _colorbuffer_id);
    return [_context presentRenderbuffer:GL_RENDERBUFFER];
}

}