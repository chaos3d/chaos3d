#ifndef _RENDER_PIPELINE_H
#define _RENDER_PIPELINE_H

#include <vector>
#include "render_pass.h"

class render_pipeline {
public:
    typedef std::vector<render_pass> re_pass_vec;
    
    void render(re_pass_vec::const_iterator& it);
    void render_all();
    
    void push_pass(render_pass const&);
    void remove_pass(uint16_t idx);
    void clear_pass();
    
private:
    re_pass_vec _passes;
};

#endif
