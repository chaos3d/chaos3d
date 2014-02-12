#include "render_view_ios.h"
#include "re/gles20/gl_context.h"
#include "re/gles20/gles2.h"
#include "event/touch_event.h"

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

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for(UITouch* touch : touches) {
        if(touch.phase != UITouchPhaseBegan)
            continue;
        
        CGPoint pos = [touch locationInView: self];
        host->dispatch(touch_began_event(
            {pos.x, pos.y}, 0.f /*FIXME, time*/,
            reinterpret_cast<uint32_t>(touch)
        ));
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch : touches) {
        if(touch.phase != UITouchPhaseMoved)
            continue;
        
        CGPoint pos = [touch locationInView: self];
        host->dispatch(touch_moved_event(
                                         {pos.x, pos.y}, 0.f /*FIXME, time*/,
                                         reinterpret_cast<uint32_t>(touch)
                                         ));
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch : touches) {
        if(touch.phase != UITouchPhaseEnded)
            continue;
        
        CGPoint pos = [touch locationInView: self];
        host->dispatch(touch_ended_event(
                                         {pos.x, pos.y}, 0.f /*FIXME, time*/,
                                         reinterpret_cast<uint32_t>(touch)
                                         ));
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch : touches) {
        if(touch.phase != UITouchPhaseCancelled)
            continue;
        
        CGPoint pos = [touch locationInView: self];
        host->dispatch(touch_cancelled_event(
                                             {pos.x, pos.y}, 0.f /*FIXME, time*/,
                                             reinterpret_cast<uint32_t>(touch)
                                             ));
    }
}

@end

namespace gles20 {

render_view::render_view(target_size_t const& size_, window_pos_t const& pos_) :
    render_window(size_, pos_), _native_view(nil) {
    create_native();
    create_view();
}

render_view::~render_view() {
    [_native_view release];
}

void render_view::create_native() {
    assert(_native_view == nil);
    _native_view = [[EAGLView alloc] initWithFrame: CGRectMake(_position(0), _position(1),
                                                               size()(0), size()(1))];
    _native_view.host = this;
	_native_view.userInteractionEnabled = YES;
	_native_view.opaque = TRUE;
	_native_view.autoresizesSubviews = NO;
	_native_view.autoresizingMask = UIViewAutoresizingNone;
    _native_view.backgroundColor = [UIColor yellowColor];
    
}
    
void render_view::create_view() {
    EAGLContext* context = [EAGLContext currentContext];
    assert(context != nil);

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

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    
}

void render_view::set_viewport(rect2d const& view) {
    glViewport(view.min().x(), view.min().y(),
               view.max().x(), view.max().y());
    GLNOERROR;
}

void render_view::clear(int mask, color_t const& color) {
    glClearColor(color(0), color(1), color(2), color(3));
    
    glClear(((mask & COLOR) ? GL_COLOR_BUFFER_BIT : 0) |
            ((mask & DEPTH) ? GL_DEPTH_BUFFER_BIT : 0) );

    GLNOERROR;
}

render_view::vector4f render_view::normalize_position(vector3f const& screen_pos,
                                         rect2d const& viewport) const {
    return vector4f(
                    (screen_pos.x() - viewport.min().x()) /
                    viewport.sizes().x() * 2.f -1.f,
                    1.f - (screen_pos.y() - viewport.min().y()) /
                    viewport.sizes().y() * 2.f,
                    screen_pos.z() * 2.f - 1.f,
                    1.f
                    );

}

void render_view::clear_stencil(int set) {
    glClearStencil(set);
    GLNOERROR;
}

bool render_view::bind(render_context*) {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);

    GLNOERROR;
    return true;
}
    
bool render_view::flush(render_context* context) {
    assert(typeid(*context) == typeid(gl_context));
    glBindRenderbuffer(GL_RENDERBUFFER, _colorbuffer_id);
    GLNOERROR;
    return [static_cast<gl_context*>(context)->context() presentRenderbuffer:GL_RENDERBUFFER];
}

}