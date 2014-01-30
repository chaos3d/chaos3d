#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

#include "re/render_state.h"
#include "re/render_uniform.h"
#include "re/vertex_layout.h"
#include "re/gpu_program.h"

class render_state;
class gpu_program;
class render_context;

// a lightweight 'function call' that renders vertices
// into the bound target
//  input: vertices, state, uniforms, etc
//  function: shader
//  output: the bound target
class render_batch {
public:
    typedef uint64_t sort_key;
    typedef std::tuple<vertex_layout::const_ptr&&, render_uniform::const_ptr const&,
    const render_state*, gpu_program*> batch_t;
    
public:
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&) {
        assert(0);
        return *this; // TODO: just fixed linking
    };
    
    render_batch(render_batch&&) = default;
    render_batch& operator=(render_batch&&) = default;
    
    render_batch(vertex_layout::const_ptr&& layout,
                 render_uniform::const_ptr const& uniform,
                 const render_state* state,
                 gpu_program* program)
    : _uniform(uniform), _state(state),
    _layout(std::move(layout)), _program(program)
    {}
    
    render_batch(batch_t const& batch)
    : render_batch(std::move(std::get<0>(batch)), std::get<1>(batch),
                   std::get<2>(batch), std::get<3>(batch))
    {}
        
    const vertex_layout* layout() const { return _layout.get(); };
    const render_state* state() const { return _state; };
    const render_uniform* uniform() const { return _uniform.get(); };
    gpu_program* program() const { return _program; };
    
private:
    sort_key _sort_key;
    
    vertex_layout::const_ptr _layout;
    render_uniform::const_ptr _uniform;
    gpu_program* _program;
    const render_state* _state;
    // TODO: memory management?
};

#endif
