#ifndef _CHAOS3D_ASSET_ASSET_COLLECTION_H
#define _CHAOS3D_ASSET_ASSET_COLLECTION_H

#include "common/log.h"
#include "asset/asset_handle.h"

#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

class asset_collection {
public:
    typedef asset_handle::ptr handle_ptr;
    typedef std::unordered_map<std::string, handle_ptr> handles_t;

    /// the asset context to decide which subset of assets should be loaded
    /// i.e.
    ///  current screen scaling based on the desired size
    ///  compression data based on the chips
    struct context {
        float scale; /// scaling factor
    };

public:
    /// the collection containing the assets based on the context
    asset_collection(context const&);

    ~asset_collection();

    /// load the asset by the given name (usually filename without ext)
    /// return null if the meta doens't exist
    /// NB: the asset support header also needs to be included
    template<class T>
    typename T::ptr load(std::string const& name) {
        static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                      "T should be tye raw type");
        auto it = _assets.find(name);
        if (it == _assets.end()) {
            LOG_WARN("unable to load the asset (" << name
                     << ") not found");
            return nullptr;
        }

        if (!it->second->is_loaded()) {
            LOG_INFO("start loading asset: " << name);
            do_load(it->second.get());
        } else {
            LOG_INFO("asset is already loaded: " << name);
        }

        return it->second->as<asset_handle_base<T>>().get_asset();
    }

    /// check if the given name is contained in this collection
    bool contains(std::string const& name) {
        return _assets.find(name) != _assets.end();
    }

    /// manually add a new asset meta
    bool add(std::string const& name, handle_ptr const& handle, bool override = true);

    /// release unused resources
    // TODO: LRU, space to free, other hints for partial purge?
    void purge();

    /// get the context
    context const& context_() const { return _context; };

    /// get all the assets
    handles_t const& assets() const { return _assets; };

private:
    void do_load(asset_handle*);
    //void loading_thread();

protected: // subclassing
    context const _context;
    handles_t _assets;
    //std::thread _loading_thread; // TODO: asynch loading
};

#endif
