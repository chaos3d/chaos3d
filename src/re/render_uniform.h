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
    
    struct uniform
    { std::string name; };
    
    struct uniform_texture : public uniform
    { uniform_texture(texture* v) : value(v) {}; texture* value; };
    
    struct uniform_float : public uniform
    { uniform_float(float v) : value(v) {}; float value; };
    
    struct uniform_vector2 : public uniform
    { uniform_vector2(vector2f const& v) : value(v) {}; vector2f value; };
    
    struct uniform_vector3 : public uniform
    { uniform_vector3(vector3f const& v) : value(v) {}; vector3f value; };
    
    struct uniform_vector4 : public uniform
    { uniform_vector4(vector4f const& v) : value(v) {}; vector4f value; };
    
    struct uniform_mat2 : public uniform
    { uniform_mat2(matrix2f const& v): value(v) {}; matrix2f value; };
    
    struct uniform_mat3 : public uniform
    { uniform_mat3(matrix3f const& v): value(v) {}; matrix3f value; };

    struct uniform_mat4 : public uniform
    { uniform_mat4(matrix4f const& v): value(v) {}; matrix4f value; };

    typedef std::vector<std::unique_ptr<uniform>> uniforms_t;
    typedef std::function<void(uniform const&)> visitor_t;
    typedef std::tuple<char const*, uniform const&> init_t;

public:
    render_uniform(std::initializer_list<init_t> const&, render_uniform* parent = nullptr);
    render_uniform(render_uniform* parent = nullptr);
    
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
    bool equal_to(render_uniform const&) const;
  
    // TODO: hash code
protected:
    uniforms_t::iterator find(std::string const&);
    uniform* find(std::string const&, bool);
    
    template<class U, class... Args>
    void set_vector(std::string const& name, Args&&... args) {
        auto it = find(name);
        if(it == _uniforms.end() || it->get()->name != name) {
            _uniforms.emplace(it, new U(std::forward<Args>(args)...));
        } else {
            assert(typeid(*it->get()) == typeid(U));
            *static_cast<U*>(it->get()) = U(std::forward<Args>(args)...);
        }
    }

private:
    render_uniform* _parent;
    uniforms_t _uniforms;
};
#endif