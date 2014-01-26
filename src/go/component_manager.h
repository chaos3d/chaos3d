#ifndef _COMPONENT_MANAGER_H
#define _COMPONENT_MANAGER_H

#include "common/singleton.h"
#include "common/utility.h"
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
        // this is becoming a mastery manager... for game objects and everything
        // auto-release could happen here then
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
    // TODO: those two not integrated yet
    virtual void pre_update(std::vector<game_object*> const&) {};
    virtual void post_update(std::vector<game_object*> const&) {};
    
    virtual void set_component_idx(uint32_t idx) = 0;
    virtual void set_component_offset(uint32_t offset) = 0;
    
    // TODO: add other managing functions
    //
    
    // TODO: there could be function that returns a set of sub-jobs
    // that could run concurrently, and thus the manager will
    // distribute to several threads
    //
    
    // helper function to initialize/construct all the component managers
    template<typename Mgrs, typename Tuple = std::tuple<>> // recursive termination, last manager
    static void manager_initializer(Tuple const& param = Tuple(), uint32_t idx = 0, uint32_t flag_bit = 0) {
        typedef typename std::tuple_element<0, Mgrs>::type Mgr;
        auto* mgr = Mgr::template initialize_tuple<Tuple>(param,
                                                          typename tuple_gens<std::tuple_size<Tuple>::value>::type());
        if(Mgr::component_fixed_t::value)
            mgr->set_component_idx(idx);
        if(Mgr::flag_bit_t::value > 0)
            mgr->set_component_offset(flag_bit);
        _fixed_component = idx + (Mgr::component_fixed_t::value ? 1 : 0);
    };
    
    template<typename Mgrs, typename Tuple = std::tuple<>, typename... ParamTuple>
    static typename std::enable_if<(sizeof...(ParamTuple) > 0)>::type
    manager_initializer(Tuple&& first, ParamTuple&&... params,
                        uint32_t idx = 0,
                        uint32_t flag_bit = 0) {
        typedef typename std::tuple_element<sizeof...(ParamTuple), Mgrs>::type Mgr;
        auto* mgr = Mgr::template initialize_tuple<Tuple>(first,
                                                          tuple_gens<std::tuple_size<Tuple>::value>::type());
        if(Mgr::component_fixed_t::value)
            mgr->set_component_idx(idx);
        if(Mgr::flag_bit_t::value > 0)
            mgr->set_component_offset(flag_bit);
        manager_initializer<Mgrs, ParamTuple...>(params..., idx + (Mgr::component_fixed_t::value ? 1 : 0),
                                                 flag_bit + Mgr::flag_bit_t::value);
    };
    
    static managers_t& managers();
    
    static uint32_t fixed_component() { return _fixed_component; }
private:
    static uint32_t _fixed_component;
};

// base class for generic functions
template<class Mgr>
class component_manager_base : public component_manager, public singleton<component_manager_base<Mgr>>{
public:
    // use a certain bits of the flag in the game object,
    // usually to condionally/lazily update values
    //  no flag by default
    typedef std::integral_constant<uint32_t, 0> flag_bit_t;
    
    // fix the index in the component list so to search it is fast
    // (only access the indexed component). this is helpful if the
    // component will be accessed by others or in a certain order
    //  fixed as the manager is initiazlied by default
    typedef std::true_type component_fixed_t;
    
    // whether to check only the type or to try casting when searching
    // in the component list
    //  don't check subclasses by default
    typedef std::true_type sealed_t;
    
public:
    template<typename... Args>
    static component_manager* initialize(Args&&... args) {
        return new Mgr(std::forward<Args>(args)...); // create in the heap
    }
    
    template<typename Tuple, int... S>
    static component_manager* initialize_tuple(Tuple const& params, tuple_seq<S...>) {
        return initialize(std::get<S>(params)...); // create in the heap
    }
    
#if 0
    template<typename ...Args>
    static C* create(game_object* go, Args&&... args) {
        return new C(go, std::forward<Args>(args)...);
    }
#endif
    
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

template<class Mgr> uint32_t component_manager_base<Mgr>::_component_idx = -1;
template<class Mgr> uint32_t component_manager_base<Mgr>::_flag_offset = -1;
#endif