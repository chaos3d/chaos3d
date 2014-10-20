#ifndef _RENDER_UNIFORM_H
#define _RENDER_UNIFORM_H

#include <vector>
#include <typeinfo>
#include <memory>
#include <Eigen/Dense>
#include "re/texture.h"

class gpu_program;

// a.k.a render_parameter/render_environment
//  uniform values that gpu uses
class render_uniform : public std::enable_shared_from_this<render_uniform> {
public:
    typedef Eigen::Matrix2f matrix2f;
    typedef Eigen::Matrix3f matrix3f;
    typedef Eigen::Matrix4f matrix4f;
    typedef Eigen::Vector2f vector2f;
    typedef Eigen::Vector3f vector3f;
    typedef Eigen::Vector4f vector4f;
    typedef std::shared_ptr<render_uniform> ptr;
    typedef std::shared_ptr<render_uniform const> const_ptr;
    
    struct uniform {
        uniform(std::string const& name)
        : _name(name), _last(1)
        {}
        
        virtual ~uniform() {};
        
        bool operator<(uniform const& rhs) const {
            return name() < rhs.name();
        }
        
        bool operator==(uniform const& rhs) const {
            return name() == rhs.name() && data_equal(rhs);
        }
        
        virtual bool data_equal(uniform const& rhs) const = 0;
        
        virtual uniform& assign(uniform const& rhs) {
            assert(typeid(*this) == typeid(*(&rhs)));
            *this = rhs;
            return *this;
        }
        
        std::string const& name() const {return _name; }
        uint32_t last() const { return _last; }
        
    private:
        std::string _name;
        uint32_t _last; // the last time it is being set
    };
    
    struct uniform_texture : public uniform {
        uniform_texture(std::string const&name, texture* v)
        : uniform(name), value(v)
        { SAFE_RETAIN(value); };
        
        uniform_texture(uniform_texture const& rhs)
        : uniform_texture(rhs.name(), rhs.value)
        { SAFE_RETAIN(value); };
        
        uniform_texture& operator=(uniform_texture const& rhs)
        {
            uniform::operator=(rhs);
            if(value == rhs.value)
                return *this;
            SAFE_RELEASE(value);
            value = rhs.value;
            SAFE_RETAIN(value);
            return *this;
        };
        
        virtual bool data_equal(uniform const& rhs) const{
            return typeid(rhs) == typeid(uniform_texture)
            && value == static_cast<uniform_texture const&>(rhs).value;
        }
        
        uniform_texture(uniform_texture&&) = default; // FIXME: might need to fix this, probably use texture::const_ptr
        uniform_texture& operator=(uniform_texture&&) = default;
        
        virtual uniform& assign(uniform const& rhs) override{
            return *this = static_cast<uniform_texture const&>(rhs);
        }
        
        virtual ~uniform_texture() {
            SAFE_RELEASE(value);
        }
        
        texture* value;
    };
    
    struct uniform_float : public uniform {
        uniform_float(std::string const&name, float v)
        : uniform(name), value(v) {};
        float value;
        
        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_float)
            && value == static_cast<uniform_float const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_float const&>(rhs);
        }
    };
    
    struct uniform_vector2 : public uniform {
        uniform_vector2(std::string const&name, vector2f const& v)
        : uniform(name), value(v) {};
        vector2f value;
        
        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_vector2)
            && value == static_cast<uniform_vector2 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_vector2 const&>(rhs);
        }
    };
    
    struct uniform_vector3 : public uniform {
        uniform_vector3(std::string const&name, vector3f const& v)
        : uniform(name), value(v) {};

        vector3f value;
        
        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_vector3)
            && value == static_cast<uniform_vector3 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_vector3 const&>(rhs);
        }
    };
    
    struct uniform_vector4 : public uniform {
        uniform_vector4(std::string const&name, vector4f const& v)
        : uniform(name), value(v) {};

        vector4f value;

        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_vector4)
            && value == static_cast<uniform_vector4 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_vector4 const&>(rhs);
        }
    };
    
    struct uniform_mat2 : public uniform {
        uniform_mat2(std::string const&name, matrix2f const& v)
        : uniform(name), value(v) {};

        matrix2f value;

        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_mat2)
            && value == static_cast<uniform_mat2 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_mat2 const&>(rhs);
        }
    };
    
    struct uniform_mat3 : public uniform {
        uniform_mat3(std::string const&name, matrix3f const& v)
        : uniform(name), value(v) {};

        matrix3f value;
        
        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_mat3)
            && value == static_cast<uniform_mat3 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_mat3 const&>(rhs);
        }
    };

    struct uniform_mat4 : public uniform {
        uniform_mat4(std::string const&name, matrix4f const& v)
        : uniform(name), value(v) {};

        matrix4f value;
        
        virtual bool data_equal(uniform const& rhs) const override{
            return typeid(*&rhs) == typeid(uniform_mat3)
            && value == static_cast<uniform_mat4 const&>(rhs).value;
        }
        
        virtual uniform& assign(uniform const& rhs) override {
            assert(typeid(*this) == typeid(*(&rhs)));
            return *this = static_cast<uniform_mat4 const&>(rhs);
        }
    };

    template<class T>
    struct uniform_assigner {
        void operator()(T& lhs, T && value) const {
            lhs = std::move(value);
        }
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
    render_uniform(uniforms_t &&); // this might not be really useful or even wrong...
    render_uniform(render_uniform* parent = nullptr);
    render_uniform(std::initializer_list<init_t> const&, render_uniform* parent = nullptr);
    render_uniform(render_uniform const& rhs) : _parent(nullptr)
    { *this = rhs; }
    render_uniform(render_uniform&& rhs) = default;
    render_uniform& operator=(render_uniform&&) = default;
    
    render_uniform& operator=(render_uniform const&);
    
    // whether it contains the uniforms with the same name
    // and the same value
    std::pair<bool, bool> contains(render_uniform const&) const;
    
    // whether it "intersects" with the other
    bool compatible(render_uniform const& rhs) const {
        auto equal = _uniforms.size() > rhs._uniforms.size() ? contains(rhs) : rhs.contains(*this);
        return equal.first && equal.second;
    }
    
    // merge/move the right side to itself, with or without the new uniforms
    render_uniform& merge(render_uniform const& rhs, bool = false);
    
    render_uniform& set_vector(std::string const& name, float v) {
        return set_vector<uniform_float>(name, v);
    }
    
    render_uniform& set_vector(std::string const& name, float u, float v) {
        return set_vector<uniform_vector2>(name, vector2f(u, v));
    }
    
    render_uniform& set_vector(std::string const& name, float x, float y, float z) {
        return set_vector<uniform_vector3>(name, vector3f(x, y, z));
    }

    render_uniform& set_vector(std::string const& name, float x, float y, float z, float w) {
        return set_vector<uniform_vector4>(name, vector4f(x, y, z, w));
    }

    render_uniform& set_matrix(std::string const& name, matrix2f const& val) {
        return set_vector<uniform_mat2>(name, val);
    }
    
    render_uniform& set_matrix(std::string const& name, matrix3f const& val) {
        return set_vector<uniform_mat3>(name, val);
    }
    
    render_uniform& set_matrix(std::string const& name, matrix4f const& val) {
        return set_vector<uniform_mat4>(name, val);
    }
    
    render_uniform& set_texture(std::string const& name, texture* tex) {
        return set_vector<uniform_texture>(name, tex);
    }
    
    template<class Value, size_t _Type = 0,
    typename std::enable_if<std::is_same<typename std::tuple_element<_Type, value_types>::type,
    Value>::value>::type* = nullptr>
    bool get(std::string const& name, Value const*& value) const {
        typedef typename std::tuple_element<_Type, uniform_types>::type U;
        auto it = find(name);
        if (it != _uniforms.end() && (*it)->name() == name) {
            value = &static_cast<U const*>(it->get())->value;
            return true;
        }
        return false;
    }
    
    template<class Value, size_t _Type = 0,
    typename std::enable_if<std::is_same<typename std::tuple_element<_Type, value_types>::type,
    Value>::value == false>::type* = nullptr>
    bool get(std::string const& name, Value const*& value) const {
        static_assert(std::tuple_size<value_types>::value > _Type + 1,
                      "not supported value type for the uniform");
        return get<Value, _Type + 1>(name, value);
    }
    
    // walk through all the uniforms, from the parent to the child
    void apply_to(visitor_t const&) const;
    
    uniforms_t const& uniforms() const { return _uniforms; }
    
    // this only compares its own level
    bool operator==(render_uniform const&) const;
  
    // TODO: hash code
protected:
    uniforms_t::const_iterator find(std::string const&) const;
    uniform* find(std::string const&, bool);
    
    template<class U, class... Args>
    render_uniform& set_vector(std::string const& name, Args&&... args) {
        auto it = find(name);
        if(it == _uniforms.end() || it->get()->name() != name) {
            _uniforms.emplace(it, new U(name, std::forward<Args>(args)...));
        } else {
            assert(typeid(*it->get()) == typeid(U));
            typedef decltype(std::declval<U>().value) value_type;
            uniform_assigner<value_type>()(static_cast<U*>(it->get())->value,
                                           value_type (std::forward<Args>(args)...));
        }
        return *this;
    }

private:
    render_uniform* _parent;
    uniforms_t _uniforms;
};

template<>
struct render_uniform::uniform_assigner<texture*> {
    void operator()(texture*& lhs, texture*&& value) const {
        if(lhs == value)
            return;
        SAFE_RELEASE(lhs);
        lhs = value;
        SAFE_RETAIN(value);
    }
};

inline render_uniform::ptr make_uniforms_ptr(std::initializer_list<render_uniform::init_t> const& list) {
    return render_uniform::ptr(new render_uniform(list));
}

// helper function to initialize the uniforms
// create the uniform from the uniform type
template<class U, class... Args>
render_uniform::uniform_ptr make_uniform(std::string const& name, Args&&... args) {
    return render_uniform::uniform_ptr(new U(name, std::forward<Args>(args)...));
}

template<size_t _Type, class... Args>
render_uniform::uniform_ptr make_uniform(std::string const& name, Args&&... args) {
    typedef typename std::tuple_element<_Type, render_uniform::uniform_types>::type U;
    return std::move(render_uniform::uniform_ptr(new U(name, std::forward<Args>(args)...)));
}

// create the uniform from the value
// uses uniform-value types mapping to iterate and find the proper uniform
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