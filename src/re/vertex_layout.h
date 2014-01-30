#ifndef _VERTEX_LAYOUT_H
#define _VERTEX_LAYOUT_H

#include "common/utility.h"
#include "common/referenced_count.h"
#include "re/vertex_buffer.h"
#include <vector>
#include <initializer_list>

class render_context;

// a thin wrapper that dictates how the data will be
// sent to the driver, and it will send the data
//
// note: layout owns the buffers, the buffers belong
// to the layout
class vertex_layout : public referenced_count {
public:
    enum { U8, Float, TypeMax };
    enum { Points, Lines, TriangleStrip, TriangleFan, Triangles };
    
    struct channel {
        vertex_buffer::ptr buffer;
        int type;   // unsigned/float
        int unit;   // count
        size_t offset;
        size_t stride;
        
        channel(vertex_buffer::ptr && ptr,
                int type_, int unit_,
                size_t offset_, size_t stride_)
        : buffer(std::move(ptr)), type(type), unit(unit_),
        offset(offset_), stride(stride_)
        {}
    };

    typedef std::vector<channel> channels_t;
    typedef std::unique_ptr<vertex_layout, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<vertex_layout const, referenced_count::release_deleter> const_ptr;
    
public:
    vertex_layout(channels_t&& channels,
                  vertex_index_buffer::ptr&& buf, // make sure we take over the ownership
                  uint8_t mode)
    : _channels(std::move(channels)), _mode(mode),
    _index_buffer(std::move(buf)), _size(0)
    {}
    
    virtual ~vertex_layout() {};
    
    virtual void draw(render_context*) = 0;
    
    vertex_index_buffer::ptr index_buffer() const {
        return _index_buffer->retain<vertex_index_buffer>();
    }
    
    vertex_index_buffer* index_buffer_raw() const {
        return _index_buffer.get();
    }
    
    channels_t const& channels() const { return _channels; }

    static size_t type_size(int type) {
        assert(type >= 0 && type < TypeMax);
        static size_t _size[] = {sizeof(char), sizeof(float)};
        return _size[type];
    }
    
private:
    channels_t _channels;
    vertex_index_buffer::ptr _index_buffer;
    
    ATTRIBUTE(uint8_t, mode);
    ATTRIBUTE(size_t, size);
};

vertex_layout::channels_t make_channels(std::initializer_list<std::tuple<vertex_buffer::ptr&&, int, int, size_t, size_t>> const& list) {
    vertex_layout::channels_t channels;
    for(auto&& it : list) {
        channels.emplace_back(std::move(std::get<0>(it)), std::get<1>(it),
                              std::get<2>(it), std::get<3>(it), std::get<4>(it));
    }
    return std::move(channels);
}
#endif