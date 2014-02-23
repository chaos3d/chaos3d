#include "asset/asset_manager.h"
#include "asset_support/texture_asset.h"
#include "com/sprite2d/texture_atlas.h"
#include "io/json_loader.h"
#include "io/memory_stream.h"
#include <rapidjson/document.h>

using namespace rapidjson;

// texture_atlas loader for texture packer in json
template<>
std::pair<bool, json_loader::position_t> texture_atlas::load_from<json_loader>(json_loader const& json,
                                                                               asset_manager& mgr,
                                                                               texture_atlas::TexturePacker &&) {
    Document const& root = json.internal<Document>();

    std::string file_name(root["metadata"]["target"]["textureFileName"].GetString());
    
    if (!mgr.contains(file_name)) {
        assert(0); // FIXME: manual add meta, and load
    }

    _texture = mgr.load<texture>(file_name);
    auto size = _texture->size();

    auto& frames = root["frames"];
    for (auto it = frames.MemberBegin(); it != frames.MemberEnd(); ++it) {
        _rects.emplace(std::piecewise_construct, std::forward_as_tuple(it->name.GetString()),
                       std::forward_as_tuple(Eigen::Vector2f{ (float)0 / size.x(), (float)0 / size.y()},
                                             Eigen::Vector2f{ (float)64 / size.x(), (float)64 / size.y()}));

    }

    return {true, 0};
}
