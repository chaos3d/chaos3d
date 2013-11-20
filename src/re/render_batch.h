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
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&);
    
private:
    typedef uint64_t sort_key;
    
    sort_key _sort_key;
    
    //---------====-----------
public:
    void execute();
    
private:
    vertex_array* _vertex;
    render_state* _state;
    render_parameters* _parameters;
    gpu_program* _program;
    
};

#endif
