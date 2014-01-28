#include "re/render_uniform.h"
#include <algorithm>

render_uniform::render_uniform(render_uniform* parent)
:_parent(parent)
{
}

render_uniform::render_uniform(std::initializer_list<init_t> const& list, render_uniform* parent)
:_parent(parent){
    for(auto& it : list) {
        if(typeid(std::get<1>(it)) == typeid(uniform_float)) {
            set_vector<uniform_float>(std::get<0>(it), static_cast<uniform_float const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_mat2)) {
            set_vector<uniform_mat2>(std::get<0>(it), static_cast<uniform_mat2 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_mat3)) {
            set_vector<uniform_mat3>(std::get<0>(it), static_cast<uniform_mat3 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_mat4)) {
            set_vector<uniform_mat4>(std::get<0>(it), static_cast<uniform_mat4 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_vector2)) {
            set_vector<uniform_vector2>(std::get<0>(it), static_cast<uniform_vector2 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_vector3)) {
            set_vector<uniform_vector3>(std::get<0>(it), static_cast<uniform_vector3 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_vector4)) {
            set_vector<uniform_vector4>(std::get<0>(it), static_cast<uniform_vector4 const&>(std::get<1>(it)).value);
        } else if(typeid(std::get<1>(it)) == typeid(uniform_texture)) {
            set_vector<uniform_texture>(std::get<0>(it), static_cast<uniform_texture const&>(std::get<1>(it)).value);
        }
    }
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
