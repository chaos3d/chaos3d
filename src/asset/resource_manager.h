#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <string>
#include <unordered_map>
#include <forward_list>
#include <memory>
#include <initializer_list>

class resource;
class data_stream;
class asset_locator;

class resource_manager {
public:
    typedef std::string resource_id_t;
    typedef std::type_index resource_type_t;
    typedef std::function<resource*(data_stream*)> resource_creator_t;
    typedef std::unordered_map<resource_type_t, resource_creator_t> creators_map_t;
    typedef std::forward_list<std::auto_ptr<asset_locator>> locators_t;

public:
	virtual ~resource_manager() {};
    
    virtual resource* create_or_retrieve(resource_id_t const&, resource_type_t const&);
    template<class R>
    R* create_or_retrieve(resource_id_t const& rid) {
        return static_cast<R*>(create_or_retrieve(rid, resource_type_t(typeid(R))));
    }
    
    // register a factory method
    bool register_factory(resource_type_t const&, resource_creator_t const&);
    
    // remove all the resources that are not used
    virtual void purge() = 0;
    
    // unload all the unused resources while keeping the meta, so it could be
    // quickly reloaded.
    virtual void unload() = 0;
    
    // get the existing resource, return null if it doesn't exist
    virtual resource* get_existing(resource_id_t const&) = 0;

    // add a list of locators
    void add_locators(std::initializer_list<asset_locator*>);
    
protected:
    virtual void cache(resource_id_t const&, resource*) = 0;
    data_stream* locate(resource_id_t const&) const;
    
private:
    creators_map_t _creators;
    locators_t _locators;
};

#endif