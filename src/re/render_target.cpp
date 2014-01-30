#include "re/render_target.h"
#include "re/render_context.h"
#include <algorithm>

render_target::render_target(target_size_t const& size)
: _size(size), _clear_color({0.f,0.f,0.f,0.f}), _batch_retained(true)
{
    
}

void render_target::add_batch(render_batch::batch_t const&batch) {
    _batches.emplace_back(batch);
}

void render_target::do_render(render_context* context) {
    // TODO: profile and logging
    
    if(!bind(context))
        return;
    // sorting should be done by the client level
    //sort();
    
    for(auto& it : _batches) {
        context->set_state(*it.state());
        it.program()->bind(context, it.uniform());
        it.layout()->draw(context);
    }
    
    if(!_batch_retained)
        _batches.clear();
    
    flush(context);
}

void render_target::sort() {
    std::sort(_batches.begin(), _batches.end(), std::less<render_batch>());
}