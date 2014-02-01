#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

#include "re/render_state.h"
#include "re/render_uniform.h"
#include "re/vertex_layout.h"
#include "re/gpu_program.h"

class render_state;
class gpu_program;
class render_context;

// the batch that will be queued to the render target
// and defered to be fired
// the render algorithm determines how to create or sort
// the batches
class render_batch {
public:
    typedef std::tuple<init_ptr<vertex_layout::const_ptr>::type,
    init_ptr<render_uniform::const_ptr>::type,
    init_ptr<render_state::const_ptr>::type,
    init_ptr<gpu_program::const_ptr>::type> batch_t;
    
public:
    render_batch(render_batch const& rhs)
    :_layout(rhs._layout->retain<vertex_layout>()),
    _program(rhs._program->retain<gpu_program>()),
    _uniform(rhs._uniform),
    _state(rhs._state)
    {};
    
    render_batch& operator=(render_batch const& rhs) {
        _layout = rhs._layout->retain<vertex_layout>();
        _program = rhs._program->retain<gpu_program>();
        _uniform = rhs._uniform;
        _state = rhs._state;
        return *this;
    };
    
    render_batch(render_batch&&) = default;
    render_batch& operator=(render_batch&&) = default;
    
    render_batch(init_ptr<vertex_layout::const_ptr>::type layout,
                 init_ptr<render_uniform::const_ptr>::type uniform,
                 init_ptr<render_state::const_ptr>::type state,
                 init_ptr<gpu_program::const_ptr>::type program)
    : _uniform(uniform), _state(state),
    _layout(std::move(layout)), _program(std::move(program))
    {}
    
    render_batch(batch_t const& batch)
    : render_batch(std::forward<init_ptr<vertex_layout::const_ptr>::type>(std::get<0>(batch)),
                   std::forward<init_ptr<render_uniform::const_ptr>::type>(std::get<1>(batch)),
                   std::forward<init_ptr<render_state::const_ptr>::type>(std::get<2>(batch)),
                   std::forward<init_ptr<gpu_program::const_ptr>::type>(std::get<3>(batch)))
    {}
        
    const vertex_layout* layout() const { return _layout.get(); };
    const render_state* state() const { return _state.get(); };
    const render_uniform* uniform() const { return _uniform.get(); };
    const gpu_program* program() const { return _program.get(); };
    
private:
    vertex_layout::const_ptr _layout;
    render_uniform::const_ptr _uniform;
    gpu_program::const_ptr _program;
    render_state::const_ptr _state;
    // TODO: memory management?
};

#endif
