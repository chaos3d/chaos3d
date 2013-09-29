#ifndef _RENDER_PASS_H
#define _RENDER_PASS_H

#include <vector>

class render_target;

class render_collector {
public:
    typedef std::vector<render_batch> re_batch_vec;
    
    virtual ~render_collector() {};
    virtual collect(re_batch_vec&) = 0;
};

class render_pass {
public:
    typedef std::vector<render_batch> re_batch_vec;

    void sort();
    
private:
    re_batch_vec _batches;
    render_target* _target;
    render_collector* _collector;
};

#endif