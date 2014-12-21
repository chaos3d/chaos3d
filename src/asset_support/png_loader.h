#ifndef _ASSET_PNG_LOADER_H
#define _ASSET_PNG_LOADER_H

#include "common/referenced_count.h"
#include "asset/asset_loader.h"
#include <memory>

class memory_stream;
class render_device;

// TODO: move this up
struct image_desc {
    typedef Eigen::Vector2i vector2i;
    enum { RGB565, RGBA8888, A8 }; // TODO: probably will support 4-bit channel
    
    vector2i size;
    int format;
};

/// png loader that decodes the png data and load the texture asset
// TODO: should rename to png_texture_loader
class png_loader : public asset_loader {
public:
    struct image_data {
        image_desc desc;
        size_t buf_size;
        std::unique_ptr<char []> buffer;

        /// release the data as a data stream
        std::unique_ptr<memory_stream> data();

    private:
        // created by png loader
        image_data() = default;

        friend class png_loader;
    };

public:
    static ptr loader(render_device* rd) {
        return ptr(new png_loader(rd));
    }
    png_loader(render_device*);
    
    ~png_loader();

    virtual bool can_load(data_stream*) const override;

    virtual asset_handle::ptr load(data_stream::ptr&&) const override;

    // TODO:
    // 1. the original image info
    // 2. load image in a separate step for more controls

private:
    render_device* _device = nullptr; // device to create textures
};

#endif
