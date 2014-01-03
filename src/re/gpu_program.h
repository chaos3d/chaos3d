#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

#include <initializer_list>
#include <vector>

class vertex_channels;
class render_uniform;
class gpu_shader;
class data_stream;
class render_context;

class gpu_shader {
public:
    enum { Vertex, Fragment };
    
public:
    gpu_shader(int type) : _type(type)
    {}
    
    virtual ~gpu_shader() {};
    
    virtual void compile(data_stream*) = 0;
    
private:
    int _type;
};

class gpu_program {
public:
    enum {
        // starting with those supports
        Texture,
        Float, Int,
        FVec2, FVec3, FVec4,
        IVec2, IVec3, IVec4,
#if 0
        Double, Unsigned,
        DVec2, DVec3, DVec4,
        UVec2, UVec3, UVec4,
#endif

        Mat2x2, Mat3x3, Mat4x4
    };
    
    struct channel {
        int location;
        int type;
        std::string name;
    };
    
    struct uniform {
        int location;
        int type;
        uint32_t last; // last update time
        std::string name;
    };
    
    typedef std::vector<channel> channels_t;
    typedef std::vector<uniform> uniforms_t;

public:
    virtual ~gpu_program() {};
    
    channels_t const& channels() const { return _channels; }
    uniforms_t const& uniforms() const { return _uniforms; }

    // shaders can be safely deleted after linking
    virtual void link(std::initializer_list<char const*> layout /* vertex attributes layout hints, channel name*/,
                      std::initializer_list<gpu_shader*> shaders) = 0;
    
    virtual void bind(render_context*, render_uniform const*) = 0;
    virtual void unbind() = 0;
 
protected:
    channels_t& channels() { return _channels; }
    uniforms_t& uniforms() { return _uniforms; }

private:
    // defines the program 'signature'
    channels_t _channels;
    uniforms_t _uniforms;
};

#endif