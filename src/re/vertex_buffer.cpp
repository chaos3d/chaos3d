#include "vertex_buffer.h"

vertex_buffer::vertex_buffer(size_t size, int type)
: _size(size), _type(type)
{
}

void vertex_buffer::set_channels(std::initializer_list<channel_desc> desc) {
    _channels.insert(_channels.end(), desc);
}

vertex_array::vertex_array(std::initializer_list<vertex_buffer_ptr> buffers)
: _buffers(buffers){
    
}
