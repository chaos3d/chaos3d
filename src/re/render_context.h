#ifndef _RENDER_CONTEXT_H
#define _RENDER_CONTEXT_H

#include "re/render_uniform.h"
#include "re/render_state.h"

class gpu_program;
class render_target;
class render_device;
/**
 * the per-thread render context saving the state
 */
// FIXME: memory managemant
class render_context {
    friend class render_device;
public:
    typedef std::vector<texture::const_ptr> textures_t;
    
public:
    render_context(size_t max_tex)
    : _textures(), _bound_textures()
    {
        _textures.resize(max_tex);
        _bound_textures.resize(max_tex);
    }
    
    virtual render_context& set_current() = 0;
    virtual void apply() = 0;
    virtual bool set_state(render_state const&) = 0;
    virtual bool set_program(gpu_program const&) = 0;
    virtual bool set_target(render_target const&) { return true; }; // FIXME
    
    void set_texture(int unit, texture* tex) {
        assert(unit < _textures.size());
        if(tex)
            _textures[unit] = tex->retain<texture>();
    }
    
    void clear_textures() {
        for(auto& it : _textures)
            it.reset();
    }
    
protected:
    virtual ~render_context() {};

    render_state _cur_state;
    render_state _bound_state;
    textures_t _textures;
    textures_t _bound_textures;
};

#endif