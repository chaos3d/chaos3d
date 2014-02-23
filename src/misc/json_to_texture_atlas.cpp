#include "asset/asset_manager.h"
#include "asset_support/texture_asset.h"
#include "com/sprite2d/texture_atlas.h"
#include "io/json_loader.h"
#include "io/memory_stream.h"
#include <cJSON/cJSON.h>

// texture_atlas loader for texture packer in json
template<>
std::pair<bool, json_loader::position_t> texture_atlas::load_from<json_loader>(json_loader const& json,
                                                                               asset_manager& mgr,
                                                                               texture_atlas::TexturePacker &&) {
    cJSON* root = json.internal();
    if (root == NULL)
        return {false, 0};
    
    // TODO: validate the input?
    cJSON* meta = cJSON_GetObjectItem(root, "metadata");
    cJSON* target = cJSON_GetObjectItem(meta, "target");
    std::string file_name(cJSON_GetObjectItem(target, "textureFileName")->valuestring);
    
    if (!mgr.contains(file_name)) {
        assert(0); // FIXME: manual add meta, and load
    }

    _texture = mgr.load<texture>(file_name);
    auto size = _texture->size();
    
    cJSON* frames = cJSON_GetObjectItem(root, "frames");
    for (cJSON* child = frames->child; child; child = child->next) {
        // TODO: support other fields...
        char const* rect = cJSON_GetObjectItem(child, "textureRect")->valuestring;
        
        // TODO: parsing the data..
        _rects.emplace(std::piecewise_construct, std::forward_as_tuple(child->string),
                       std::forward_as_tuple(Eigen::Vector2f{ (float)0 / size.x(), (float)0 / size.y()},
                                             Eigen::Vector2f{ (float)64 / size.x(), (float)64 / size.y()}));
    }
    return {true, 0};
}
