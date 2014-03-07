#include <algorithm>
#include <typeinfo>

#include "re/gles20/gl_gpu.h"
#include "re/gles20/gles2.h"
#include "re/render_context.h"
#include "io/data_stream.h"

static int _shader_map [] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
};

// TODO: error log
gl_gpu_shader::gl_gpu_shader(int type) : gpu_shader(type) {
    assert(type == Vertex || type == Fragment);
    _shader_id = glCreateShader(_shader_map[type]);
}

gl_gpu_shader::~gl_gpu_shader() {
    glDeleteShader(_shader_id);
}

void gl_gpu_shader::compile(data_stream* ds) {
    GLint size = ds->size();
    GLchar* buf = new char [size];
    ds->read(buf, size);
    
    glShaderSource(_shader_id, 1, &buf, &size);
    glCompileShader(_shader_id);
    
    GLint status;
    glGetShaderiv(_shader_id, GL_COMPILE_STATUS, &status);
    
    if (! status)
    {
        GLsizei length;
		glGetShaderiv(_shader_id, GL_INFO_LOG_LENGTH, &length);
		GLchar* src = (GLchar *)malloc(sizeof(GLchar) * length);
		
		glGetShaderInfoLog(_shader_id, length, &length, src);
        
        printf("%s\n", src);
        free(src);
    }
    delete [] buf;
}

gl_gpu_program::gl_gpu_program() {
    _program_id = glCreateProgram();
}

gl_gpu_program::~gl_gpu_program() {
    glDeleteProgram(_program_id);
}

void gl_gpu_program::detach_all() {
    GLint num = 0;
    glGetProgramiv(_program_id, GL_ATTACHED_SHADERS, &num);
    
    // num will stay 0 for errors
    if(num == 0)
        return;
    
    GLuint shaders[num];
    glGetAttachedShaders(_program_id, num, &num, shaders);
    
    for(int i = 0; i < num; ++i) {
        glDetachShader(_program_id, shaders[i]);
    }
}

void gl_gpu_program::add_attribute(char* name, GLenum type, GLint size) {
    assert(size == 1); // only one dimension
    channels().push_back({glGetAttribLocation(_program_id, name), 0, name});
    auto& attr = channels().back();
    switch (type) {
        case GL_FLOAT: attr.type = Float; break;
        case GL_FLOAT_VEC2: attr.type = FVec2; break;
        case GL_FLOAT_VEC3: attr.type = FVec3; break;
        case GL_FLOAT_VEC4: attr.type = FVec4; break;
        default: assert(0); // not supported yet
            break;
    }
}

void gl_gpu_program::add_uniform(char* name, GLenum type, GLint size) {
    assert(size == 1); // only one dimension
    
    uniforms().push_back({glGetUniformLocation(_program_id, name), 0, 0, name});
    auto& attr = channels().back();
    switch (type) {
        case GL_FLOAT: attr.type = Float; break;
        case GL_FLOAT_VEC2: attr.type = FVec2; break;
        case GL_FLOAT_VEC3: attr.type = FVec3; break;
        case GL_FLOAT_VEC4: attr.type = FVec4; break;
        case GL_INT: attr.type = Int; break;
        case GL_INT_VEC2: attr.type = IVec2; break;
        case GL_INT_VEC3: attr.type = IVec3; break;
        case GL_INT_VEC4: attr.type = IVec4; break;
        case GL_FLOAT_MAT2: attr.type = Mat2x2; break;
        case GL_FLOAT_MAT3: attr.type = Mat3x3; break;
        case GL_FLOAT_MAT4: attr.type = Mat4x4; break;
        case GL_SAMPLER_2D: attr.type = Texture; break;
            
        default: assert(0); // not supported yet
            break;
    }
}

void gl_gpu_program::load_attributes() {
    GLint num = 0;
    glGetProgramiv(_program_id, GL_ACTIVE_ATTRIBUTES, &num);
    
    if(num == 0)
        return;
    
    GLint max_len = 0;
    glGetProgramiv(_program_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_len);
    
    GLchar name[max_len];
    GLenum type;
    GLint size;
    GLsizei name_len;
    
    for(int idx = 0; idx < num; ++idx) {
        glGetActiveAttrib(_program_id, idx, max_len, &name_len, &size, &type, name);
        
        // ignore built-in variables
        if(memcmp(name, "gl_", 3) == 0)
            continue;
        
        add_attribute(name, type, size);
    }
    
    std::sort(channels().begin(), channels().end(), [] (channel const& rhs, channel const lhs) {
        return rhs.name < lhs.name;
    });
}

void gl_gpu_program::load_uniforms() {
    GLint num = 0;
    glGetProgramiv(_program_id, GL_ACTIVE_UNIFORMS, &num);
    
    if(num == 0)
        return;
    
    GLint max_len = 0;
    glGetProgramiv(_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    
    GLchar name[max_len];
    GLenum type;
    GLint size;
    GLsizei name_len;
    
    for(int idx = 0; idx < num; ++idx) {
        glGetActiveUniform(_program_id, idx, max_len, &name_len, &size, &type, name);
        
        // ignore built-in variables
        if(memcmp(name, "gl_", 3) == 0)
            continue;
        
        add_uniform(name, type, size);
    }

    std::sort(uniforms().begin(), uniforms().end(), [] (uniform const& rhs, uniform const lhs) {
        return rhs.name < lhs.name;
    });
}

void gl_gpu_program::link(std::vector<std::string> layout, std::initializer_list<gpu_shader*> shaders) {
    uniforms().clear();
    channels().clear();
    detach_all();
    
    int idx = -1;
    for(auto it : layout) {
        ++idx;
        if(it.empty())
            continue;
        glBindAttribLocation(_program_id, idx, it.c_str());
    }
    
    for(auto it : shaders) {
        assert(dynamic_cast<gl_gpu_shader*>(it) != nullptr);
        glAttachShader(_program_id, static_cast<gl_gpu_shader*>(it)->shader_id());
    }
    glLinkProgram(_program_id);
    
#if 1
    GLint status;
    glGetProgramiv(_program_id, GL_LINK_STATUS, &status);
    
    if(status != GL_TRUE) {
    
    }
    
    GLint log_len = 0;
    glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &log_len);
    
    if(log_len > 0) {
        char* log = new char [log_len];
        glGetProgramInfoLog(_program_id, log_len, &log_len, log);
        printf("%s\n", log);
        delete [] log;
    }
#endif
    
    load_attributes();
    load_uniforms();
}

void gl_gpu_program::update_uniform(uniform const& g_uniform, render_uniform::uniform const& uniform) const {
    if(typeid(uniform) == typeid(render_uniform::uniform_vector4)) {
        glUniform4fv(g_uniform.location, 1, static_cast<render_uniform::uniform_vector4 const&>(uniform).value.data());
    } else if(typeid(uniform) == typeid(render_uniform::uniform_mat4)) {
        glUniformMatrix4fv(g_uniform.location, 1, GL_FALSE,
                           static_cast<render_uniform::uniform_mat4 const&>(uniform).value.data());
    } else if(typeid(uniform) == typeid(render_uniform::uniform_float)) {
        glUniform1fv(g_uniform.location, 1, &static_cast<render_uniform::uniform_float const&>(uniform).value);
    } else if(typeid(uniform) == typeid(render_uniform::uniform_mat3)) {
        glUniformMatrix3fv(g_uniform.location, 1, GL_FALSE,
                           static_cast<render_uniform::uniform_mat3 const&>(uniform).value.data());
    } else if(typeid(uniform) == typeid(render_uniform::uniform_vector3)) {
        glUniform3fv(g_uniform.location, 1, static_cast<render_uniform::uniform_vector3 const&>(uniform).value.data());
    } else if(typeid(uniform) == typeid(render_uniform::uniform_vector2)) {
        glUniform2fv(g_uniform.location, 1, static_cast<render_uniform::uniform_vector2 const&>(uniform).value.data());
    } else if(typeid(uniform) == typeid(render_uniform::uniform_mat2)) {
        glUniformMatrix2fv(g_uniform.location, 1, GL_FALSE,
                           static_cast<render_uniform::uniform_mat2 const&>(uniform).value.data());
    }
}

void gl_gpu_program::assign_uniforms(render_context* context, render_uniform::uniforms_t const& rd_uniforms) const {
    auto rd_begin = rd_uniforms.begin(), rd_end = rd_uniforms.end();
    auto gpu_begin = uniforms().begin(), gpu_end = uniforms().end();
    int unit = 0;
    
    while (rd_begin != rd_end && gpu_begin != gpu_end) {
        int ret = rd_begin->get()->name().compare(gpu_begin->name);
        if (ret < 0) {
            ++rd_begin;
        } else  {
            if (ret == 0) {
                if(typeid(*rd_begin->get()) == typeid(render_uniform::uniform_texture)) {
                    // TODO: sanity check, unit less than max units
                    glUniform1i(gpu_begin->location, unit);
                    context->set_texture(unit++,
                                         static_cast<render_uniform::uniform_texture const&>(*rd_begin->get()).value);
                } else
                    update_uniform(*gpu_begin, *rd_begin->get());
                
                gpu_begin->last = rd_begin->get()->last();
            }
            ++gpu_begin;
        }
    }
    
}

void gl_gpu_program::bind(render_context* context, render_uniform const* uniform,
                          std::vector<render_uniform::const_ptr> const& uniforms) const{
    glUseProgram(_program_id);
    GLNOERROR;

    if(!uniform)
        return;

    for(auto& it : uniforms) {
        assign_uniforms(context, it->uniforms());
    }
    assign_uniforms(context, uniform->uniforms());
    GLNOERROR;
}

