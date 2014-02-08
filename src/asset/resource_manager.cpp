#include "asset/resource_manager.h"
#include "asset/asset_locator.h"

#if 0
void test() {
    resource_manager* res;
    res->register_factories({
        {typeid(int), [=] (data_stream*) {
            resource* r;
            return r;
        }}
    });
}
#endif

bool resource_manager::register_factory(resource_type_t const& rtype, resource_creator_t const& rfactory) {
    auto typeidx = std::type_index(rtype);
    auto it = _creators.find(typeidx);
    if(it != _creators.end())
        return false;
    
    _creators.emplace(typeidx, rfactory);
    return true;
}

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
    
    data_stream::ptr stream = locate(rid);
    if(!stream)
        return nullptr;
    
    resource* res = it->second(stream.get());
    if(!res)
        return nullptr;
    
    cache(rid, res);
    return res;
}

data_stream::ptr resource_manager::locate(const resource_id_t & rid) const{
    data_stream::ptr stream;
    for(auto& it : _locators) {
        if((stream = it->from(rid)))
            break;
    }
    return std::move(stream);
}

void resource_manager::add_locators(std::initializer_list<asset_locator*> locators) {
    for(auto l : locators) {
        _locators.emplace_front(l);
    }
    
    _locators.sort([=](std::unique_ptr<asset_locator> const& r, std::unique_ptr<asset_locator> const& l) {
        return  r->priority() < l->priority();
    });
}
