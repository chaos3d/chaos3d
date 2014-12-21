#ifndef _ASSET_PNG_LOADER_H
#define _ASSET_PNG_LOADER_H

#include "common/referenced_count.h"
#include "asset/asset_loader.h"
#include <memory>

class memory_stream;

// TODO: move this up
struct image_desc {
    typedef Eigen::Vector2i vector2i;
    enum { RGB565, RGBA8888, A8 }; // TODO: probably will support 4-bit channel
    
    vector2i size;
    int format;
};

// decode the png data and load the image
// into the memory buffer
class png_loader : public asset_loader {
public:
    // TODO: this may apply to all texture data
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
    png_loader();
    
    ~png_loader();

    virtual bool can_load(data_stream*) const override;

    image_data do_load(data_stream* source, int format = image_desc::RGB565) const;
    
    // TODO:
    // 1. the original image info
    // 2. load image in a separate step for more controls

private:
    void load(data_stream&, image_data&) const;
};

#endif
