#ifndef _CHAOS3D_ASSET_ASSET_LOADER_H
#define _CHAOS3D_ASSET_ASSET_LOADER_H

#include "asset/asset_handle.h"
#include "io/data_stream.h"
#include <memory>

/// the loader to load the asset from the stream
class asset_loader {
public:
    typedef std::unique_ptr<asset_loader> ptr;

public:
    /// check if the stream is valid for the supported asset type
    /// it might read the data and will reset it back to where it was
    virtual bool can_load(data_stream* ds) const = 0;

    /// load the handle, will take over the data stream
    /// the actual asset will be lazily loaded
    virtual asset_handle::ptr load(data_stream::ptr&& ds) const = 0;

};

#endif
