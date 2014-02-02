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
    typedef std::vector<game_object*> goes_t;
    
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
    // note, one can choose not to be managed. (i.e. camera)
    explicit component_manager(bool managed = true);
    virtual ~component_manager();
    
    virtual void update(goes_t const&) = 0;
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
    template<typename Mgrs, typename... ParamTuple>
    static void manager_initializer(ParamTuple const&... params ) {
        return manager_initializer_internal<0, 0, Mgrs, ParamTuple...>(params...);
    }
    
    static managers_t& managers();
    
    static uint32_t fixed_component() { return _fixed_component; }
    
private:
    template<uint32_t _Idx, uint32_t _Bit, typename Mgrs> // recursive termination, last manager
    static void manager_initializer_internal(/*Tuple const& param*/) {
        //typedef typename std::tuple_element<std::tuple_size<Mgrs>::value - 1, Mgrs>::type Mgr;
        _fixed_component = _Idx;// + (Mgr::component_fixed_t::value ? 1 : 0);
    };
    
    template<uint32_t _Idx, uint32_t _Bit, typename Mgrs, typename First, typename... ParamTuple>
    static void manager_initializer_internal(First const&first, ParamTuple const&... params ) {
        constexpr size_t mgrs_size = std::tuple_size<Mgrs>::value;
        typedef typename std::tuple_element<mgrs_size - sizeof...(ParamTuple) - 1, Mgrs>::type Mgr;
        
        auto* mgr = Mgr::template initialize_tuple<First>(first,
                                                          typename tuple_gens<std::tuple_size<First>::value>::type());
        if(Mgr::component_fixed_t::value)
            mgr->set_component_idx(_Idx);
        if(Mgr::flag_bit_t::value > 0)
            mgr->set_component_offset(_Bit);
        
        manager_initializer_internal<_Idx + (Mgr::component_fixed_t::value ? 1 : 0), _Bit + Mgr::flag_bit_t::value,
        Mgrs, ParamTuple...>
        (params...);
    };
    
    static uint32_t _fixed_component;
};

// base class for generic functions
template<class Mgr>
class component_manager_base : public component_manager, public singleton<Mgr>{
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
        return initialize(std::get<S>(params)...);
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
    
protected:
    component_manager_base(component_manager_base const&) = delete;
    explicit component_manager_base(bool managed = true) : component_manager(managed)
    {}
    
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

// a "nil" component manager contains minimum requirements for a
// non-fixed component.
template<typename Sealed = std::true_type>
class nil_component_mgr : public component_manager_base<nil_component_mgr<Sealed> > {
public:
    typedef std::false_type component_fixed_t;
    typedef Sealed sealed_t;
    using component_manager::goes_t;
    
protected:
    nil_component_mgr(): component_manager_base<nil_component_mgr<Sealed>>(false)
    {}
    
    void update(goes_t const&) {};
};

// an "empty" component manager for a fixed component, it does nothing
// but saves the idx and offset if needed
template<typename Tag, typename Sealed = std::false_type>
class empty_component_mgr : public component_manager_base<empty_component_mgr<Tag, Sealed> > {
public:
    typedef std::true_type component_fixed_t;
    typedef Sealed sealed_t;
    using component_manager::goes_t;
    
protected:
    empty_component_mgr(): component_manager_base<empty_component_mgr<Tag, Sealed>>(false)
    {}
    
    void update(goes_t const&) {};
};

#endif