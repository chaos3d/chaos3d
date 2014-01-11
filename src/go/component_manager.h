#ifndef _COMPONENT_MANAGER_H
#define _COMPONENT_MANAGER_H

#include "common/singleton.h"
class game_object;

/// opaque component handle, being a type recoganizer
struct component_handle {
    uint32_t handle;
    
    component_handle() : handle(0) {}
};

/// the container/factory of a particular type of component
/// memory management, optimization, threading, etc...
class component_manager {
public:
    virtual void update() = 0;
    //virtual component* create() = 0;
    
private:
    game_object* _root;
};

template<class C>
class component_manager_base : public component_manager, public singleton<component_manager_base<C>>{
public:
    C* from(component_handle const&) const {
        return nullptr;
    }
    
private:
};

#endif