#ifndef _GL_GPU_H
#define _GL_GPU_H

#include "gles2.h"
#include "re/gpu_program.h"
#include "re/render_uniform.h"

class gl_gpu_shader : public gpu_shader {
public:
    gl_gpu_shader(int type);
    virtual ~gl_gpu_shader();
    
    virtual void compile(std::vector<char const*> const&) override;

    GLuint shader_id() const { return _shader_id; }
    
private:
    GLuint _shader_id;
};

class gl_gpu_program : public gpu_program{
public:
    gl_gpu_program();
    virtual ~gl_gpu_program();
    
    virtual void link(std::vector<std::string>, std::vector<gpu_shader*>) override;
    virtual void bind(render_context*, render_uniform const*,
                      std::vector<render_uniform::const_ptr> const&) const override;
    
protected:
    void detach_all();

    // assign/update to the hardware buffer
    void assign_uniforms(render_context* context, render_uniform::uniforms_t const&) const;
    void update_uniform(uniform const&, render_uniform::uniform const&) const;
    
    void load_attributes();
    void load_uniforms();
    void add_attribute(char* name, GLenum type, GLint size);
    void add_uniform(char* name, GLenum type, GLint size);

private:
    GLuint _program_id;
};

#endif