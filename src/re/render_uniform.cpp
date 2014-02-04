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


bool render_uniform::operator==(render_uniform const& rhs) const {
    if(rhs.uniforms().size() != uniforms().size())
        return false;
    
    return std::mismatch(_uniforms.begin(), _uniforms.end(), rhs.uniforms().begin(),
                         [] (uniform_ptr const& lhs, uniform_ptr const& rhs) {
                             return lhs->data_equal(*rhs);
                         }).first == _uniforms.end();
}

render_uniform& render_uniform::operator=(render_uniform const& rhs) {
    _uniforms.clear();
    for(auto& ptr : rhs._uniforms) {
        auto* it = ptr.get();
        if(typeid(*it) == typeid(uniform_float)) {
            _uniforms.emplace_back(new uniform_float(it->name(), static_cast<uniform_float const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_mat2)) {
            _uniforms.emplace_back(new uniform_mat2(it->name(), static_cast<uniform_mat2 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_mat3)) {
            _uniforms.emplace_back(new uniform_mat3(it->name(), static_cast<uniform_mat3 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_mat4)) {
            _uniforms.emplace_back(new uniform_mat4(it->name(), static_cast<uniform_mat4 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_vector2)) {
            _uniforms.emplace_back(new uniform_vector2(it->name(), static_cast<uniform_vector2 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_vector3)) {
            _uniforms.emplace_back(new uniform_vector3(it->name(), static_cast<uniform_vector3 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_vector4)) {
            _uniforms.emplace_back(new uniform_vector4(it->name(), static_cast<uniform_vector4 const&>(*it).value));
        } else if(typeid(*it) == typeid(uniform_texture)) {
            _uniforms.emplace_back(new uniform_texture(it->name(), static_cast<uniform_texture const&>(*it).value));
        } else
            assert(0); // not supported type?
    }
    return *this;
}

std::pair<bool, bool> render_uniform::contains(render_uniform const& rhs) const {
    bool value_equal = true;
    return std::make_pair(std::includes(_uniforms.begin(), _uniforms.end(),
                                        rhs.uniforms().begin(), rhs.uniforms().end(),
                                        [&value_equal] (uniform_ptr const& lhs, uniform_ptr const& rhs) {
                                            int ret = lhs->name().compare(rhs->name());
                                            if(ret == 0 && value_equal) {
                                                value_equal = lhs->data_equal(*rhs);
                                            }
                                            return ret < 0;
                                        }),
                          value_equal);
}

render_uniform& render_uniform::merge(render_uniform const& rhs, bool append) {
    assert(append == false); //FIXME
    auto first1 = _uniforms.begin(), last1 = _uniforms.end();
    auto first2 = rhs.uniforms().begin(), last2 = rhs.uniforms().end();
    for (; first1 != last1;) {
        if (first2 == last2) {
            break;
        }
        int ret = (*first1)->name().compare((*first2)->name());
        if (ret > 0) {
            //*d_first = *first2; // FIXME: merge if appending
            ++first2;
        } else if (ret < 0) {
            //*d_first = *first1; // FIXME: merge if appending
            ++first1;
        } else {
            (*first1)->assign(**first2);
            ++first1, ++first2;
        }
    }
    return *this;
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
