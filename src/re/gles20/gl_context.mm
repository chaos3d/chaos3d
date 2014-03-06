#include "re/gles20/gl_context.h"
#include "re/gles20/gl_gpu.h"
#include "re/gles20/gl_texture.h"
#include "gles2.h"

static GLenum _depth_func_map [] = {
    0, // DepthNone
    GL_NEVER, GL_ALWAYS,
    GL_LESS, GL_LEQUAL,
    GL_EQUAL, GL_NOTEQUAL,
    GL_GEQUAL, GL_GREATER
};

static GLenum _blend_func_map [] = {
    0, // BlendNone
    GL_ONE,	GL_ZERO,
    GL_DST_COLOR, GL_SRC_COLOR,
    GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR,
    GL_DST_ALPHA, GL_SRC_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,	GL_ONE_MINUS_SRC_ALPHA
};

gl_context::gl_context(EAGLContext* context, size_t max)
: _context(context), render_context(max)
{
    
}

void gl_context::set_current() {
    [EAGLContext setCurrentContext: context()];
}

void gl_context::apply() {
    render_state const& cur = _cur_state;
    render_state const& bound = _bound_state;
    
    //TODO: add to the state
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    if(cur.depth_func() != bound.depth_func()) {
        if(cur.depth_func() == render_state::DepthNone) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(_depth_func_map[cur.depth_func()]);
        }
        GLNOERROR;
    }

    if(cur.src_blend() != bound.src_blend()
       || cur.dst_blend() != bound.dst_blend()
       || cur.src_alpha_blend() != bound.src_alpha_blend()
       || cur.dst_alpha_blend() != bound.dst_alpha_blend()
       ){
        if(cur.src_blend() == render_state::BlendNone
           || cur.dst_blend() == render_state::BlendNone
           || cur.src_alpha_blend() == render_state::BlendNone
           || cur.dst_alpha_blend() == render_state::BlendNone) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFuncSeparate(_blend_func_map[cur.src_blend()],
                                _blend_func_map[cur.dst_blend()],
                                _blend_func_map[cur.src_alpha_blend()],
                                _blend_func_map[cur.dst_alpha_blend()]);
        }
        GLNOERROR;
    }
    
    if(cur.blend_color() != bound.blend_color())
        glBlendColor(cur.blend_color()[0], cur.blend_color()[1], cur.blend_color()[2], cur.blend_color()[3]);
    
    int unit = 0;
    auto bound_it = _bound_textures.begin();
    for(auto it = _textures.begin(); it != _textures.end();
        ++bound_it, ++unit, ++it) {
        if(it->get() == bound_it->get())
            continue;
        
        glActiveTexture(GL_TEXTURE0 + unit);
        if(*it == nullptr) {
            glDisable(GL_TEXTURE_2D); // FIXME: texture type
        } else {
            glBindTexture(GL_TEXTURE_2D, static_cast<gles20::gl_texture const*>(it->get())->tex_id());
        
            // TODO: texture parameters
        }
        GLNOERROR;
    }
    
    glActiveTexture(GL_TEXTURE0);
    _bound_state = _cur_state;
    std::transform(_textures.begin(), _textures.end(), _bound_textures.begin(), [](texture::const_ptr &t) {
        return t.get() ? t->retain<texture>() : std::move(t);
    });
    GLNOERROR;

    //_bound_textures = std::move(_textures);
}

#pragma mark - move up
bool gl_context::set_state(render_state const&state) {
    _cur_state = state;
    return true;
}

bool gl_context::set_program(gpu_program const& program) {
    assert(typeid(program) == typeid(gl_gpu_program));
    return true;
}
