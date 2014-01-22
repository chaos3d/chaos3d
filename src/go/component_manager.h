#ifndef _COMPONENT_MANAGER_H
#define _COMPONENT_MANAGER_H

#include "common/singleton.h"
#include <vector>

class game_object;

/**
 * the container/factory of a particular type of component
 * memory management, optimization, threading, etc...
 * a manager will be transparent to the client, it would be
 * only useful to create a component. Internally, it should
 * handle all the game loop, update, render, events handler
 * etc, through component_managers
 */
class component_manager {
public:
    /**
     * the manager of the managers - when a component manager
     * is initialized, it will be added to this list in order
     * to manage. It is a helper class to have the client less
     * concerned about how the manage works.
     *
     * this is only an interface for the client to use.
     */
    struct managers_t {
        virtual ~managers_t() {};
        virtual void update(game_object* /*root*/) = 0;
    };

public:
    // the constructor will add itself to the managers singleton.
    // once a comoponent manager is created of some sort, it will
    // be automatically managed by the global manager
    component_manager();
    virtual ~component_manager();
    
    virtual void update(std::vector<game_object*> const&) = 0;
    // TODO: add other managing functions
    //
    
    // TODO: there could be function that returns a set of sub-jobs
    // that could run concurrently, and thus the manager will
    // distribute to several threads
    //
    
    // helper function to initialize/construct all the component managers
    template<typename Mgr>
    static void manager_initializer() {
        Mgr::template initialize<Mgr>();
    };
    
    template<typename Mgr, typename... Others>
    static typename std::enable_if<(sizeof...(Others) > 0)>::type manager_initializer() {
        Mgr::template initialize<Mgr>();
        manager_initializer<Others...>();
    };
    
    static managers_t& managers();
    
private:
    
};

// base class for generic functions
template<class C>
class component_manager_base : public component_manager, public singleton<component_manager_base<C>>{
public:
    
public:
    template<typename Mgr>
    static component_manager* initialize() {
        return new Mgr(); // create in the heap
    }
    
private:
};

#endif