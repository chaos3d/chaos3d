#ifndef _CHAOS3D_ASSET_ASSET_MANAGER_H
#define _CHAOS3D_ASSET_ASSET_MANAGER_H

#include <thread>
#include <vector>
#include "common/referenced_count.h"
#include "common/singleton.h"
#include "common/utility.h"
#include "common/log.h"

#include "asset/asset_collection.h"

class asset_bundle;

class asset_manager : public asset_collection, public std::enable_shared_from_this<asset_manager> {
public:
    asset_manager(context const&);

    // add/remove the meta info from the bundle to the manager
    // so the manager could find the meta
    // if the meta with the same already exists, it will be
    // overriden, and when it is removed, they will be all gone
    // returns the number of item being added and replaced
    std::pair<uint32_t, uint32_t> add_from_bundle(asset_bundle*);
    uint32_t remove_from_bundle(asset_bundle*);

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