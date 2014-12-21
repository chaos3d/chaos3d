#ifndef _CHAOS3D_ASSET_ASSET_LOADER_H
#define _CHAOS3D_ASSET_ASSET_LOADER_H

class data_stream;

/// the loader to load the asset from the stream
class asset_loader {
public:

    /// check if the stream is valid for the supported asset type
    /// it might read the data and will reset it back to where it was
    virtual bool can_load(data_stream* ds) const = 0;

    //virtual void async_load();
};

#endif
