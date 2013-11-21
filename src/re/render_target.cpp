#include "render_target.h"
#include "render_batch.h"
#include <algorithm>

void render_target::do_render(bool clear) {
    // TODO: profile and logging
    if(!bind())
        return;
    sort();
    
    for(auto& it : _batches) {
        it.execute();
    }
    
    flush();
}

void render_target::sort() {
    std::sort(_batches.begin(), _batches.end(), std::less<render_batch>());
}