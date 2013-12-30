#include "re/render_uniform.h"
#include <algorithm>

render_uniform::render_uniform(render_uniform* parent)
:_parent(parent)
{
}

render_uniform::uniforms_t::iterator render_uniform::find(std::string const& name){
    return std::lower_bound(_uniforms.begin(), _uniforms.end(), name,
                            [] (std::unique_ptr<uniform> const& u, std::string const& name) {
                                return u->name < name;
                            });
}

render_uniform::uniform* render_uniform::find(std::string const& name, bool) {
    auto it = find(name);
    
    if(it == _uniforms.end() || it->get()->name != name)
        return _parent ? _parent->find(name, true) : nullptr;
    else
        return it->get();
}

void render_uniform::apply_to(visitor_t const& visitor) const {
    if(_parent)
        _parent->apply_to(visitor);

    for(auto &it : _uniforms) {
        visitor(*it.get());
    }
}
