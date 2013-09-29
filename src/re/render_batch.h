#ifndef _RENDER_BATCH_H
#define _RENDER_BATCH_H

class render_batch {
public:
    bool operator<(render_batch const&rhs) const {
        return _sort_key < rhs._sort_key;
    }
    
    render_batch& operator=(render_batch const&);
    
private:
    typedef uint64_t sort_key;
    
    sort_key _sort_key;
    render_state _state;
    render_vertex_array *_vertex;
};

#endif
