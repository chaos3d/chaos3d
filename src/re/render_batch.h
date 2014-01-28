#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

#include "re/render_state.h"
#include "re/render_uniform.h"
#include "re/vertex_layout.h"
#include "re/gpu_program.h"

class vertex_layout;
class render_state;
class render_uniform;
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
    
public:
    struct batch_t {
        const render_state* state;
        const render_uniform* uniform;
        const vertex_layout* layout;
        gpu_program* program;
    };
    
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&) {
        assert(0);
        return *this; // TODO: just fixed linking
    };
    
    render_batch(vertex_layout* layout,
                 const render_state* state,
                 const render_uniform* uniform,
                 gpu_program* program)
    : _uniform(uniform), _state(state),
    _layout(layout), _program(program)
    {}
                 
    render_batch(batch_t const& batch)
    : render_batch(batch.layout, batch.state,
                   batch.uniform, batch.program)
    {}
        
    vertex_layout* layout() const { return _layout; };
    const render_state* state() const { return _state; };
    const render_uniform* uniform() const { return _uniform; };
    gpu_program* program() const { return _program; };
    
private:
    sort_key _sort_key;
    
    vertex_layout* _layout;
    const render_state* _state;
    const render_uniform* _uniform;
    gpu_program* _program;
    // TODO: memory management?
};

#endif
