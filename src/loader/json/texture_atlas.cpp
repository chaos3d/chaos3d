#include "common/log.h"
#include "asset/asset_manager.h"
#include "asset_support/texture_asset.h"
#include "com/sprite2d/texture_atlas.h"
#include "io/memory_stream.h"
#include "json_loader.h"
#include <rapidjson/document.h>

using namespace rapidjson;

// texture_atlas loader for texture packer in json
template<>
texture_atlas::ptr texture_atlas::load_from(json_loader const& json,
                                            asset_manager& mgr,
                                            texture_atlas::TexturePacker &&) {
    Document const& root = json.internal<Document>();

    std::string file_name(root["meta"]["image"].GetString());
    
    if (!mgr.contains(file_name)) {
        LOG_WARN(asset_manager, "expected asset not loaded:" << file_name);
        // FIXME: manual add meta, and load
        return nullptr;
    }

    texture_atlas *atlas = new texture_atlas();
    atlas->_texture = mgr.load<texture>(file_name);
    auto size = atlas->_texture->size();

    auto& frames = root["frames"];
    auto& rects = atlas->_rects;
    for (auto it = frames.Begin(); it != frames.End(); ++it) {
        // TODO: source size/rotate/trim
        auto& frame = (*it)["frame"];
        float x = (float)frame["x"].GetInt() / size.x();
        float y = (float)frame["y"].GetInt() / size.y();
        rects.emplace(std::piecewise_construct, std::forward_as_tuple((*it)["filename"].GetString()),
                      std::forward_as_tuple(
                                            Eigen::Vector2f{x, y},
                                            Eigen::Vector2f{
                                                (float)frame["w"].GetInt() / size.x() + x,
                                                (float)frame["h"].GetInt() / size.y() + y}
                                            )
                      );
    }
    
    return ptr(atlas);
}
