#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

class vertex_channels;
class data_stream;

class gpu_program {
public:
    enum {
        // starting with those supports
        Texture,
        Float,
        Vec2, Vec3, Vec4,
        Mat3x3, Mat4x4
    };
    
    struct channel {
        std::string name;
        int type;
    };
    
    struct uniform {
        std::string name;
        int type;
        uint32_t last; // last update time
    };
    
    typedef std::vector<channel> channels_t;
    typedef std::vector<uniform> uniforms_t;

public:
    gpu_program(data_stream* vs, data_stream* ps);
    
    channels_t const& channels() const { return _channels; }
    uniforms_t const& uniforms() const { return _uniforms; }

    virtual void compile() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
private:
    // defines the program 'signature'
    channels_t _channels;
    uniforms_t _uniforms;
};

#endif