#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

#if 0
#include "gpu_program.h"
#include "vertex_buffer.h
#include "render_state.h"
#endif

class vertex_array;
class render_state;
class render_parameters;
class gpu_program;

// a lightweight 'function call' that renders vertices
// into the bound target
//  input: vertices, state, uniforms, etc
//  function: shader
//  output: the bound target
class render_batch {
public:
    struct batch_t {
        vertex_array* vertex;
        render_state* state;
        render_parameters* parameters;
        gpu_program* program;
    };
    
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&) {
        return *this; // TODO: just fixed linking
    };
    
    render_batch(vertex_array* vertex,
                 render_state* state,
                 render_parameters* parameters,
                 gpu_program* program)
    : _vertex(vertex), _state(state),
    _parameters(parameters), _program(program)
    {}
                 
    render_batch(batch_t const& batch)
    : render_batch(batch.vertex, batch.state,
                   batch.parameters, batch.program)
    {}
private:
    typedef uint64_t sort_key;
    
    sort_key _sort_key;
    
    //---------====-----------
public:
    void execute() {
        // TODO: fixed linking
    };
    
private:
    vertex_array* _vertex;
    render_state* _state;
    render_parameters* _parameters;
    gpu_program* _program;
    // TODO: memory management?
};

#endif
