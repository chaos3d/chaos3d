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
#include "asset/asset_handle.h"

class asset_bundle;

class asset_manager {
public:
    typedef std::shared_ptr<asset_handle> asset_ptr;
    typedef std::unordered_map<std::string, asset_ptr> assets_t;
    
public:
    asset_manager();
    ~asset_manager();
    
    template<class T>
    typename T::ptr load(std::string const& name) {
        static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                      "T should be tye raw type");
        auto it = _assets.find(name);
        if (it == _assets.end())
            return nullptr;

        if (!it->second->is_loaded())
            load_asset(it->second.get());
        
        return it->second->as<asset_handle_base<T>>().get_asset();
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