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
        int unit;   // number of elements
        size_t offset;
        size_t stride;
        
        channel(vertex_buffer::ptr && ptr,
                int type_, int unit_,
                size_t offset_, size_t stride_)
        : buffer(std::move(ptr)), type(type_), unit(unit_),
        offset(offset_), stride(stride_)
        {}
    };
    typedef std::vector<channel> channels_t;
    typedef std::unique_ptr<vertex_layout, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<vertex_layout const, referenced_count::release_deleter> const_ptr;
    
    // the layout lock
    // all the vertex buffers will be locked and unlocked at destruction time
    // help to access channel data (buffer/stride/type) easily
    // move offsets will affect all the buffers such that getting buffers for
    // each channel will consider the offset
    class locked_buffer {
    public:
        explicit locked_buffer(vertex_layout::ptr&& layout)
        : _layout(std::move(layout)), _offset(0) {
            _buffer.reserve(_layout->channels().size());
        }
        
        char* buffer(int idx) const {
            assert(idx >= 0 && idx < _buffer.size());
            return _buffer[idx] + _offset * stride(idx);
        }
        
        size_t stride(int idx) const {
            assert(idx >= 0 && idx < _buffer.size());
            return _layout->channels()[idx].stride;
        }
        
        int type(int idx) const{
            assert(idx >= 0 && idx < _buffer.size());
            return _layout->channels()[idx].type;
        }
        
        int unit(int idx) const{
            assert(idx >= 0 && idx < _buffer.size());
            return _layout->channels()[idx].unit;
        }
        
        void unlock();
        
        ~locked_buffer() { unlock(); }
        locked_buffer(locked_buffer&&) = default;
        
        locked_buffer(locked_buffer const&) = delete;
        locked_buffer() = delete;
        locked_buffer& operator=(locked_buffer const&) = delete;
        locked_buffer& operator=(locked_buffer &&) = delete;
        
    private:
        std::vector<char*> _buffer; // buffer address
        vertex_layout::ptr _layout;
        
        ATTRIBUTE(size_t, offset, size_t());
        friend class vertex_layout;
    };
    
public:
    vertex_layout(channels_t&& channels,
                  vertex_index_buffer::ptr&& buf, // make sure we take over the ownership
                  uint8_t mode)
    : _channels(std::move(channels)), _mode(mode),
    _index_buffer(std::move(buf))
    {}
    
    virtual ~vertex_layout() {};
    
    // draw to the "context"
    virtual void draw(render_context*, size_t start, size_t count) const = 0;
    
    vertex_index_buffer::ptr index_buffer() const {
        return _index_buffer->retain<vertex_index_buffer>();
    }
    
    vertex_index_buffer* index_buffer_raw() const {
        return _index_buffer.get();
    }
    
    channels_t const& channels() const { return _channels; }

    // TODO: asynch locking? more options? per buffer locking?
    locked_buffer lock_channels();
    
    static size_t type_size(int type) {
        assert(type >= 0 && type < TypeMax);
        static size_t _size[] = {sizeof(char), sizeof(float)};
        return _size[type];
    }
    
private:
    channels_t _channels;
    vertex_index_buffer::ptr _index_buffer;
    
    ATTRIBUTE(uint8_t, mode, Points); // TODO: this could be in batches too?
};

extern vertex_layout::channels_t make_channels(std::initializer_list<std::tuple<vertex_buffer::ptr&&, int, int, size_t, size_t>> const& list);

#endif