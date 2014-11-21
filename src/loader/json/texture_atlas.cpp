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
        // TODO: source size/trim
        auto& frame = (*it)["frame"];
        bool rotated = (*it)["rotated"].GetBool();
        float x = (float)frame["x"].GetInt();
        float y = (float)frame["y"].GetInt();
        float w = (float)frame["w"].GetInt();
        float h = (float)frame["h"].GetInt();
        if (rotated) {
            rects.emplace(std::piecewise_construct, std::forward_as_tuple((*it)["filename"].GetString()),
                          std::forward_as_tuple(sprite_v_t{{
                vector2f((x + h)/size.x(), (y + w)/size.y()), vector2f(x/size.x(), (y + w)/size.y()),
                vector2f((x + h)/size.x(), y/size.y()), vector2f(x/size.x(), y/size.y())
            }}));
        } else {
            rects.emplace(std::piecewise_construct, std::forward_as_tuple((*it)["filename"].GetString()),
                          std::forward_as_tuple(sprite_v_t{{
                vector2f(x/size.x(), (y + h)/size.y()), vector2f(x/size.x(), y/size.y()),
                vector2f((x + w)/size.x(), (y + h)/size.y()), vector2f((x + w)/size.x(), y/size.y())
            }}));
        }
    }
    
    return ptr(atlas);
}
