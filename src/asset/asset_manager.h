#ifndef _CHAOS3D_ASSET_ASSET_MANAGER_H
#define _CHAOS3D_ASSET_ASSET_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>
#include <type_traits>
#include "common/referenced_count.h"
#include "common/singleton.h"
#include "common/utility.h"
#include "common/log.h"
#include "asset/asset_handle.h"

class asset_bundle;

class asset_manager : public std::enable_shared_from_this<asset_manager> {
public:
    typedef std::shared_ptr<asset_handle> handle_ptr;
    typedef std::unordered_map<std::string, handle_ptr> handles_t;
    
    // the asset context to decide which subset of assets should be loaded
    // i.e.
    //  current screen scaling based on the desired size
    //  compression data based on the chips
    struct context {
        float scale; // scaling factor
    };
    
public:
    // load the asset by the given name (usually filename without ext)
    // return null if the meta doens't exist
    // NB: the asset support header also needs to be included
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
            load_asset(it->second.get());
        } else {
            LOG_INFO("asset is already loaded: " << name);
        }
        
        return it->second->as<asset_handle_base<T>>().get_asset();
    }
    
    // check if it contains the asset meta (usually loaded from bundles)
    bool contains(std::string const& name) {
        return _assets.find(name) != _assets.end();
    }
    
    // manually add a new asset meta
    bool add(std::string const& name, handle_ptr const& handle, bool override = true);
    
    // add/remove the meta info from the bundle to the manager
    // so the manager could find the meta
    // if the meta with the same already exists, it will be
    // overriden, and when it is removed, they will be all gone
    // returns the number of item being added and replaced
    std::pair<uint32_t, uint32_t> add_from_bundle(asset_bundle*);
    uint32_t remove_from_bundle(asset_bundle*);

    // release unused resources
    // TODO: LRU, space to free, other hints for partial purge?
    void purge();

protected:
    asset_manager(context const&);
    ~asset_manager();
    
private:
    void load_asset(asset_handle*);
    void loading_thread();
    
private:
    context const _context;
    handles_t _assets;
    std::thread _loading_thread; // TODO: asynch loading
    
    CONSTRUCTOR_FOR_SHARED(asset_manager);
};

// convenient asset manager singleton
class global_asset_mgr :
    public asset_manager,
    public singleton<global_asset_mgr> {
        
public:
    // initialize the manager to the shard_ptr because
    // asset manager is shared_ptr enabled
    // multiple calling this function won't create multiple
    // instances but still should be prevented
    static asset_manager& create(context const& ctx) {
        static std::shared_ptr<global_asset_mgr> _instance(new global_asset_mgr(ctx));
        return *_instance;
    }

private:
    global_asset_mgr(context const& ctx)
    : asset_manager(ctx) {
    }
};

#endif