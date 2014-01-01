#include "re/render_target.h"
#include "re/render_context.h"
#include <algorithm>

void render_target::do_render(render_context* context, bool clear) {
    // TODO: profile and logging
    
    if(!bind(context))
        return;
    sort();
    
    for(auto& it : _batches) {
        context->set_state(*it.state());
        it.program()->bind(context, it.uniform());
        it.layout()->draw(context);
    }
    
    flush(context);
}

void render_target::sort() {
    std::sort(_batches.begin(), _batches.end(), std::less<render_batch>());
}