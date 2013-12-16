#include "asset/resource_manager.h"
#include "asset/asset_locator.h"

resource* resource_manager::create_or_retrieve(const resource_id_t &rid, const resource_type_t &rtype) {
    // TODO: log errors
    resource* existing = get_existing(rid);
    if(existing)
        return existing;
    
    auto it = _creators.find(rtype);
    if(it == _creators.end())
        return nullptr;
    
    if(!it->second)
        return nullptr;
    
    data_stream* stream = locate(rid);
    if(!stream)
        return nullptr;
    
    resource* res = it->second(stream);
    if(!res)
        return nullptr;
    
    cache(rid, res);
    return res;
}

data_stream* resource_manager::locate(const resource_id_t & rid) const{
    data_stream* stream = nullptr;
    for(auto& it : _locators) {
        if((stream = it->from(rid)) != nullptr)
            break;
    }
    return stream;
}

void resource_manager::add_locators(std::initializer_list<asset_locator*> locators) {
    for(auto l : locators) {
        _locators.emplace_front(l);
    }
    
    _locators.sort([=](std::auto_ptr<asset_locator> const& r, std::auto_ptr<asset_locator> const& l) {
        return  r->priority() < l->priority();
    });
}
