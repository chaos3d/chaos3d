#ifndef _TEXTURE_H
#define _TEXTURE_H

class texture{
public:
    typedef Eigen::Vector2f vector2f;
    
    ///! type: Texture 1D, 2D, 3D
    enum { T1D, T2D, T3D };
    
    ///! format: compress/uncompress etc
    enum { NONE, ETC, PVRTC2, PVRTC4 };
    
    ///! color channels
    enum { RGBA8, RGB565, ALPHA, LUMINANCE };
    
    struct attribute_t {
        int _type, _format, _color;
        int _wrap_s, _wrap_t;
        int _min_filter, _max_filter;
        bool _mipmap;
    };
    
    virtual ~texture() {};
    
    vector2f const& size() const { return _size; }
    int type() const { return _type; }
    int format() const { return _format; }
    int color() const { return _color; }
    attribute_t const& attribute() const { return _attribute; }
    
    virtual bool load() = 0;

protected:
    void set_size(vector2f const& size) { _size = size; }
    void set_type(int type) { _type = type; }
    void set_format(int format) { _format = format; }
    void set_color(int color) { _color = color; }
    attribute_t& attribute() { return _attribute; }
    
private:
    vector2f _size;
    int _type, _format, _color;
    attribute_t _attribute;
};

#endif