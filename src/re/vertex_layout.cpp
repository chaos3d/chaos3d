#include "re/vertex_layout.h"

vertex_layout::channels_t make_channels(std::initializer_list<std::tuple<vertex_buffer::ptr&&, int, int, size_t, size_t>> const& list) {
    vertex_layout::channels_t channels;
    for(auto&& it : list) {
        channels.emplace_back(std::move(std::get<0>(it)), std::get<1>(it),
                              std::get<2>(it), std::get<3>(it), std::get<4>(it));
    }
    return std::move(channels);
}
