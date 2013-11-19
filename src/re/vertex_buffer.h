#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

class vertex_channel {
public:
    size_t stride() const;
    uint8_t* data() const;
};

// a single buffer that saves multi-channel data
// which can be interleaved
class vertex_buffer {
public:
    enum { Static, Dynamic };
    enum { U8, Float };
    struct channel_desc {
        int type;
        int format;
        size_t size;
    };
    
    virtual ~vertex_buffer() {};
    
    virtual void bind();
    virtual void unbind();
    
    size_t size() const;
    bool interleaved() const;
    bool client() const;
    
    vertex_channel* channel(int idx) const;
private:
    
};

// TODO
// an array of vertex_buffer that are grouped together
// in a logical manner, i.e.
// a static buffer, a dynamic buffer and an indexed buffer
// each of them could have different channels
class vertex_array {
    
};
#endif