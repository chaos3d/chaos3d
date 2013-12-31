#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

#if 0
#include "gpu_program.h"
#include "vertex_buffer.h
#include "render_state.h"
#endif

class vertex_layout;
class render_state;
class render_uniform;
class gpu_program;

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
        vertex_layout* layout;
        render_state* state;
        render_uniform* uniform;
        gpu_program* program;
    };
    
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&) {
        return *this; // TODO: just fixed linking
    };
    
    render_batch(vertex_layout* layout,
                 render_state* state,
                 render_uniform* uniform,
                 gpu_program* program)
    : _uniform(uniform), _state(state),
    _layout(layout), _program(program)
    {}
                 
    render_batch(batch_t const& batch)
    : render_batch(batch.layout, batch.state,
                   batch.uniform, batch.program)
    {}
    
    void execute() {
        // TODO: fixed linking
    };
    
private:
    sort_key _sort_key;
    
    vertex_layout* _layout;
    render_state* _state;
    render_uniform* _uniform;
    gpu_program* _program;
    // TODO: memory management?
};

#endif
