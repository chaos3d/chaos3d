#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

#include <initializer_list>
#include <vector>
#include <memory>
#include "common/referenced_count.h"
#include "re/render_uniform.h"

class vertex_channels;
class gpu_shader;
class memory_stream;
class render_context;

class gpu_shader : public std::enable_shared_from_this<gpu_shader> {
public:
    enum { Vertex, Fragment };
    typedef std::shared_ptr<gpu_shader> ptr;
    typedef std::shared_ptr<const gpu_shader> const_ptr;
    
public:
    gpu_shader(int type) : _type(type)
    {}
    
    virtual ~gpu_shader() {};
    
    virtual gpu_shader& compile(std::vector<char const*> const&) = 0;
    
    gpu_shader& compile(char const* source) {
        return compile(std::vector<char const*>{source});
    };
    
private:
    int _type;
};

class gpu_program : public referenced_count {
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
        mutable uint32_t last; // last update time
        std::string name;
    };
    
    typedef std::vector<channel> channels_t;
    typedef std::vector<uniform> uniforms_t;
    typedef std::unique_ptr<gpu_program, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<gpu_program const, referenced_count::release_deleter> const_ptr;
    
public:
    virtual ~gpu_program() {};
    
    channels_t const& channels() const { return _channels; }
    uniforms_t const& uniforms() const { return _uniforms; }

    // shaders can be safely deleted after linking
    virtual gpu_program& link(std::vector<std::string> layout /* vertex attributes layout hints, channel name*/,
                              std::vector<gpu_shader*> shaders) = 0;
    
    // bind to the hardware
    virtual void bind(render_context*, render_uniform const*,
                      std::vector<render_uniform::const_ptr> const&) const = 0;
 
protected:
    channels_t& channels() { return _channels; }
    uniforms_t& uniforms() { return _uniforms; }

private:
    // defines the program 'signature'
    channels_t _channels;
    uniforms_t _uniforms;
};

#endif