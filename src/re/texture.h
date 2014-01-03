#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <Eigen/Dense>

class texture{
public:
    typedef Eigen::Vector2i vector2i;
    
    ///! type: Texture 1D, 2D, 3D
    enum { T1D, T2D, T3D };
    
    ///! color format
    enum {
        RGBA8888, RGB565, ALPHA, LUMINANCE,
        PVRTC4_RGB, PVRTC2_RGB, PVRTC4_RGBA, PVRTC2_RGBA
    };
    
    ///! wrap
    enum { Clamp, Repeat, Mirrored };
    
    ///! filter
    enum {
        Nearest, Linear,
        NearestNearest, LinearNearst, NearestLinear, LinearLinear
    };
    
    struct attribute_t {
        int type, color;
        int wrap_s, wrap_t;
        int min_filter, max_filter;
        int mipmap; // level of mipmaps
    };
    
    texture(vector2i const& size, attribute_t const& attr = {
        T2D, RGBA8888,
        Clamp, Clamp,
        NearestLinear, Nearest,
        1
    }) : _attribute(attr), _size(size) {};
    virtual ~texture() {};
    
    vector2i const& size() const { return _size; }
    attribute_t const& attribute() const { return _attribute; }
    
    virtual bool load(void const*, size_t, int color, int level = 0) = 0;
    virtual bool generate_mipmap() { return false; };
    
private:
    vector2i _size;
    attribute_t _attribute;
};

#endif