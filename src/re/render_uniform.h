#ifndef _RENDER_UNIFORM_H
#define _RENDER_UNIFORM_H

#include <Eigen/Dense>
#include <vector>

class texture;
class gpu_program;

// a.k.a render_parameter/render_environment
//  uniform values that gpu uses
class render_uniform {
public:
    typedef Eigen::Matrix2f matrix2f;
    typedef Eigen::Matrix3f matrix3f;
    typedef Eigen::Matrix4f matrix4f;
    typedef Eigen::Vector2f vector2f;
    typedef Eigen::Vector3f vector3f;
    typedef Eigen::Vector4f vector4f;
    typedef std::unique_ptr<render_uniform> ptr;
    
    struct uniform {
        uniform(std::string const& name, size_t size)
        : _name(name), _size(size)
        {}
        
        bool operator<(uniform const& rhs) const {
            return name() < rhs.name();
        }
        
        bool operator==(uniform const& lhs) const {
            return size() == lhs.size() &&
            std::memcmp(reinterpret_cast<char const*>(&_size) + sizeof(_size),
                        reinterpret_cast<char const*>(&lhs._size) + sizeof(_size),
                        size()) == 0 &&
            name() == lhs.name();
        }
        
        bool data_equal(uniform const& rhs) const {
            return size() == rhs.size() && typeid(*this) == typeid(rhs) &&
            std::memcmp(reinterpret_cast<char const*>(&_size) + sizeof(_size),
                               reinterpret_cast<char const*>(&rhs._size) + sizeof(_size),
                               size()) == 0;
        }
        
        std::string const& name() const {return _name; }
        size_t size() const { return _size; }
    private:
        std::string _name; size_t _size;
    };
    
    struct uniform_texture : public uniform {
        uniform_texture(std::string const&name, texture* v)
        : uniform(name, sizeof(texture*)), value(v) {};
        texture* value;
    };
    
    struct uniform_float : public uniform {
        uniform_float(std::string const&name, float v)
        : uniform(name, sizeof(float)), value(v) {};
        float value;
    };
    
    struct uniform_vector2 : public uniform {
        uniform_vector2(std::string const&name, vector2f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        vector2f value;
    };
    
    struct uniform_vector3 : public uniform {
        uniform_vector3(std::string const&name, vector3f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        vector3f value;
    };
    
    struct uniform_vector4 : public uniform {
        uniform_vector4(std::string const&name, vector4f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        vector4f value;
    };
    
    struct uniform_mat2 : public uniform {
        uniform_mat2(std::string const&name, matrix2f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        matrix2f value;
    };
    
    struct uniform_mat3 : public uniform {
        uniform_mat3(std::string const&name, matrix3f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        matrix3f value;
    };

    struct uniform_mat4 : public uniform {
        uniform_mat4(std::string const&name, matrix4f const& v)
        : uniform(name, sizeof(value)), value(v) {};
        matrix4f value;
    };

    typedef std::unique_ptr<uniform> uniform_ptr;
    typedef std::vector<uniform_ptr> uniforms_t;
    typedef std::function<void(uniform const&)> visitor_t;
    typedef std::tuple<uniform_ptr &&> init_t;
    
    typedef std::tuple<
    uniform_float, uniform_vector2, uniform_vector3, uniform_vector4,
    uniform_mat2, uniform_mat3, uniform_mat4, uniform_texture
    > uniform_types;
    typedef std::tuple<
    float, vector2f, vector3f, vector4f, matrix2f, matrix3f, matrix4f, texture*
    > value_types; // easist iteration unfolding...
    
    enum { Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4, Texture };
    
public:
    render_uniform(uniforms_t &&); // this might not be really useful...
    render_uniform(render_uniform* parent = nullptr);
    render_uniform(std::initializer_list<init_t> const&, render_uniform* parent = nullptr);
    render_uniform(render_uniform const& rhs) : _parent(nullptr)
    { *this = rhs; }
    render_uniform(render_uniform&& rhs) = default;
    
    render_uniform& operator=(render_uniform const&);
    
    // whether the two contain the uniforms with the same name
    // and the same value
    std::pair<bool, bool> contains(render_uniform const&) const;
    
    void set_vector(std::string const& name, float v) {
        set_vector<uniform_float>(name, v);
    }
    
    void set_vector(std::string const& name, float u, float v) {
        set_vector<uniform_vector2>(name, vector2f(u, v));
    }
    
    void set_vector(std::string const& name, float x, float y, float z) {
        set_vector<uniform_vector3>(name, vector3f(x, y, z));
    }
    
    void set_matrix(std::string const& name, matrix2f const& val) {
        set_vector<uniform_mat2>(name, val);
    }
    
    void set_matrix(std::string const& name, matrix3f const& val) {
        set_vector<uniform_mat3>(name, val);
    }
    
    void set_matrix(std::string const& name, matrix4f const& val) {
        set_vector<uniform_mat4>(name, val);
    }
    
    void set_texture(std::string const& name, texture* tex) {
        set_vector<uniform_texture>(name, tex);
    }
    
    // walk through all the uniforms, from the parent to the child
    void apply_to(visitor_t const&) const;
    
    uniforms_t const& uniforms() const { return _uniforms; }
    
    // this only compares its own level
    bool operator==(render_uniform const&) const;
  
    // TODO: hash code
protected:
    uniforms_t::iterator find(std::string const&);
    uniform* find(std::string const&, bool);
    
    template<class U, class... Args>
    void set_vector(std::string const& name, Args&&... args) {
        auto it = find(name);
        if(it == _uniforms.end() || it->get()->name() != name) {
            _uniforms.emplace(it, new U(name, std::forward<Args>(args)...));
        } else {
            assert(typeid(*it->get()) == typeid(U));
            static_cast<U*>(it->get())->value = decltype(std::declval<U>().value) (std::forward<Args>(args)...);
        }
    }

private:
    render_uniform* _parent;
    uniforms_t _uniforms;
};

// helper function to initialize the uniforms
template<class U, class... Args>
render_uniform::uniform_ptr make_uniform(std::string const& name, Args&&... args) {
    return render_uniform::uniform_ptr(new U(name, std::forward<Args>(args)...));
}

template<size_t _Type, class... Args>
render_uniform::uniform_ptr make_uniform(std::string const& name, Args&&... args) {
    typedef typename std::tuple_element<_Type, render_uniform::uniform_types>::type U;
    return std::move(render_uniform::uniform_ptr(new U(name, std::forward<Args>(args)...)));
}

template<class Value, size_t _Type = 0,
typename std::enable_if<std::is_same<
    typename std::tuple_element<_Type, render_uniform::value_types>::type,
Value>::value>::type* = nullptr>
render_uniform::uniform_ptr make_uniform(std::string const& name, Value const& value) {
    typedef typename std::tuple_element<_Type, render_uniform::uniform_types>::type U;
    return render_uniform::uniform_ptr(new U(name, value));
}

template<class Value, size_t _Type = 0,
typename std::enable_if<std::is_same<
typename std::tuple_element<_Type, render_uniform::value_types>::type,
Value>::value == false>::type* = nullptr>
render_uniform::uniform_ptr make_uniform(std::string const& name, Value const& value) {
    static_assert(std::tuple_size<render_uniform::value_types>::value > _Type+1,
                  "not supported value type for the uniform");
    return make_uniform<Value, _Type + 1>(name, value);
}

#endif