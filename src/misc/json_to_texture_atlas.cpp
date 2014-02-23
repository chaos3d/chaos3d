#include "asset/asset_manager.h"
#include "asset_support/asset_texture.h"
#include "com/sprite2d/texture_atlas.h"
#include "io/json_loader.h"
#include "io/memory_stream.h"

template<>
std::pair<bool, json_loader::position_t> texture_atlas::load_from<json_loader>(json_loader const& json, asset_manager& mgr) {
    _texture = mgr.load<texture>("chaos3d");

    auto size = _texture->size();
    _rects.emplace(std::piecewise_construct, std::forward_as_tuple("test1"),
                   std::forward_as_tuple(Eigen::Vector2f{ (float)0 / size.x(), (float)0 / size.y()},
                                         Eigen::Vector2f{ (float)64 / size.x(), (float)64 / size.y()}));
    return {true, 0};
}
