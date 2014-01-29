#include "re/render_uniform.h"
#include <algorithm>

render_uniform::render_uniform(render_uniform* parent)
:_parent(parent)
{
}

render_uniform::render_uniform(std::initializer_list<init_t> const& list, render_uniform* parent)
:_parent(parent){
    _uniforms.reserve(list.size());
    for(auto& it : list) {
        _uniforms.emplace_back(std::get<0>(it).release());
#if 0
        auto& it = std::get<0>(tuple);
        if(typeid(it) == typeid(uniform_float)) {
            set_vector<uniform_float>(it.name(), static_cast<uniform_float const&>(it).value);
        } else if(typeid(it) == typeid(uniform_mat2)) {
            set_vector<uniform_mat2>(it.name(), static_cast<uniform_mat2 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_mat3)) {
            set_vector<uniform_mat3>(it.name(), static_cast<uniform_mat3 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_mat4)) {
            set_vector<uniform_mat4>(it.name(), static_cast<uniform_mat4 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_vector2)) {
            set_vector<uniform_vector2>(it.name(), static_cast<uniform_vector2 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_vector3)) {
            set_vector<uniform_vector3>(it.name(), static_cast<uniform_vector3 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_vector4)) {
            set_vector<uniform_vector4>(it.name(), static_cast<uniform_vector4 const&>(it).value);
        } else if(typeid(it) == typeid(uniform_texture)) {
            set_vector<uniform_texture>(it.name(), static_cast<uniform_texture const&>(it).value);
        }
#endif
    }
    std::sort(_uniforms.begin(), _uniforms.end());
    std::unique(_uniforms.begin(), _uniforms.end());
}

render_uniform::render_uniform(uniforms_t&& uniforms)
: _uniforms(std::forward<uniforms_t>(uniforms)){
    std::sort(_uniforms.begin(), _uniforms.end());
    std::unique(_uniforms.begin(), _uniforms.end());
}


bool render_uniform::equal_to(render_uniform const& rhs) const {
    if(rhs.uniforms().size() != uniforms().size())
        return false;
    
#if 0
    // FIXME: not right...
    return std::mismatch(_uniforms.begin(), _uniforms.end(), rhs.uniforms().begin()).first == _uniforms.end();
#else
    return false;
#endif
}

render_uniform::uniforms_t::iterator render_uniform::find(std::string const& name){
    return std::lower_bound(_uniforms.begin(), _uniforms.end(), name,
                            [] (std::unique_ptr<uniform> const& u, std::string const& name) {
                                return u->name() < name;
                            });
}

render_uniform::uniform* render_uniform::find(std::string const& name, bool) {
    auto it = find(name);
    
    if(it == _uniforms.end() || it->get()->name() != name)
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
