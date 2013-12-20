#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

class gpu_program {
public:
    struct channel {
        std::string name;
        int type;
    };
    
    struct uniform {
        std::string name;
        int type;
    };
    
    typedef std::vector<channel> channels_t;
    typedef std::vector<uniform> uniforms_t;

public:
    channels_t const& channels() const { return _channels; }
    uniforms_t const& uniforms() const { return _uniforms; }

private:
    channels_t _channels;
    uniforms_t _uniforms;
};

#endif