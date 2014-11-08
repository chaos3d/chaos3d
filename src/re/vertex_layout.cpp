#include "re/vertex_layout.h"

vertex_layout::channels_t make_channels(std::initializer_list<std::tuple<vertex_buffer::ptr&&, int, int, size_t, size_t>> const& list) {
    vertex_layout::channels_t channels;
    for(auto&& it : list) {
        channels.emplace_back(std::move(std::get<0>(it)), std::get<1>(it),
                              std::get<2>(it), std::get<3>(it), std::get<4>(it));
    }
    return std::move(channels);
}

void vertex_layout::locked_buffer::unlock() {
    if(!_layout)
        return;
    
    for(auto& it : _layout->channels()) {
        if(it.buffer->is_locked())
            it.buffer->unlock();
    }
}

vertex_layout::locked_buffer vertex_layout::lock_channels() {
    locked_buffer locked(retain<vertex_layout>());
    
    typedef std::tuple<vertex_buffer*, char*> locked_t;
    std::vector<locked_t> locks;
    for (auto& it : _channels) {
        char* buf = nullptr;
        if (!it.buffer->is_locked()) {
            buf = static_cast<char*>(it.buffer->lock());
            locks.emplace_back(it.buffer.get(), buf);
        } else {
            buf = std::get<1>(*std::find_if(locks.begin(), locks.end(),
                                            [&] (decltype(locks.front()) const& l) {
                                                return std::get<0>(l) == it.buffer.get();
                                            }));
        }
        locked._buffer.emplace_back(buf + it.offset);
    }
    return std::move(locked);
}
