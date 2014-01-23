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
 *
 * note that, a component is not necessory bound to a game
 * object, how the manager to update, handle events, etc..
 * is completely independent. it is a sub-system, a template.
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
    virtual void set_component_idx(uint32_t idx) = 0;
    virtual void set_component_offset(uint32_t offset) = 0;
    
    // TODO: add other managing functions
    //
    
    // TODO: there could be function that returns a set of sub-jobs
    // that could run concurrently, and thus the manager will
    // distribute to several threads
    //
    
    // helper function to initialize/construct all the component managers
    template<typename Mgr>
    static void manager_initializer(uint32_t flag_bit = 0) {
        auto* mgr = Mgr::initialize();
        mgr->set_component_idx(0);
        mgr->set_component_offset(flag_bit);
    };
    
    template<typename Mgr, typename... Others>
    static typename std::enable_if<(sizeof...(Others) > 0)>::type manager_initializer(uint32_t flag_bit = 0) {
        auto* mgr = Mgr::initialize();
        mgr->set_component_idx(sizeof...(Others));
        mgr->set_component_offset(flag_bit);
        manager_initializer<Others...>(flag_bit + Mgr::flag_bit_t::value);
    };
    
    static managers_t& managers();
};

// base class for generic functions
template<class Mgr, class C>
class component_manager_base : public component_manager, public singleton<component_manager_base<Mgr, C>>{
public:
    typedef std::integral_constant<uint32_t, 0> flag_bit_t; // by default, no flag

public:
    static component_manager* initialize() {
        return new Mgr(); // create in the heap
    }
    
    // attributes on the game object
    static uint32_t component_idx() { return _component_idx; }
    static uint32_t flag_offset() { return _flag_offset; }
    
private:
    virtual void set_component_idx(uint32_t idx) override {
        assert(_component_idx == -1); // only set once for now
        _component_idx = idx;
    };
    
    virtual void set_component_offset(uint32_t offset) override {
        assert(_flag_offset == -1); // only set once for now
        _flag_offset = offset;
    };
    
    // once the manager is created, the component index will be fixed
    // we could have a list of component indices later
    static uint32_t _component_idx;
    static uint32_t _flag_offset;
};

template<class Mgr, class C> uint32_t component_manager_base<Mgr, C>::_component_idx = -1;
template<class Mgr, class C> uint32_t component_manager_base<Mgr, C>::_flag_offset = -1;
#endif