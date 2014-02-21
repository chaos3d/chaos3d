#ifndef _CHAOS3D_ASSET_ASSET_MANAGER_H
#define _CHAOS3D_ASSET_ASSET_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>
#include "asset/asset_bundle.h"
#include "common/referenced_count.h"
#include "common/singleton.h"

class asset_manager {
public:
    typedef std::shared_ptr<asset_handle> asset_ptr;
    typedef std::unordered_map<std::string, asset_ptr> assets_t;
    
public:
    asset_manager();
    ~asset_manager();
    
    template<class T>
    typename std::remove_cv<T>::type* load(std::string const& name) {
        typedef typename std::remove_cv<T>::type Raw_T;
        auto it = _assets.find(name);
        if (it == _assets.end())
            return nullptr;

        load_asset(it->second.get());
        return it->second->as<asset_handle_base<Raw_T>>().get_asset();
    }
    
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
    
private:
    void load_asset(asset_handle*);
    void loading_thread();
    
private:
    assets_t _assets;
    std::thread _loading_thread; // TODO: asynch loading
};

class global_asset_mgr :
    public asset_manager,
    public singleton<global_asset_mgr, Static_Instance> {
};

#endif