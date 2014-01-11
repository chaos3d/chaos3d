#ifndef _COMPONENT_MANAGER_H
#define _COMPONENT_MANAGER_H

class game_object;

/// opaque component handle, being a type recoganizer
struct component_handle {
    uint32_t handle;
    
    component_handle() : handle(0) {}
};

/// the container/factory of a particular type of component
class component_manager {
public:
    virtual void update() = 0;
    //virtual component* create() = 0;
    
private:
    game_object* _root;
};

template<class C>
class component_manager_base : public component_manager{
public:
    static C* from(component_handle const&) {
        return nullptr;
    }
    
private:
};

#endif