#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/size.hpp>

#include <type_traits>
#include <cassert>
#include <vector>
#include <array>
#include <string>
#include <memory>

#include "common/referenced_count.h"
#include "common/utility.h"
#include "go/component_manager.h"
#include "go/component.h"

class component_meta;

/**
 * the game object is nothing but the connection for all the components.
 * it only has the hierachy info (this may be in the component as well?)
 */
class game_object : public referenced_count{
public:
    enum {Parent = 0, Order = 1, Offset = 2 };
    enum {ComponentSize = 16 }; // before we figure out a better way...

    typedef std::unique_ptr<component, component::component_deleter> component_ptr;
    typedef std::function<void (game_object const&)> iterator_t;
    typedef std::array<component_ptr, ComponentSize> components_t;
    
public:
    game_object(game_object* parent = &root())
    : _first_child(null), _parent(nullptr),
    _next_sibling(null), _pre_sibling(null), _child_size(0),
    _flag(-1U){
        ++ _number_of_objects;
        if(parent)
            parent->add_child(this);
    }
    
    virtual ~game_object();
    
    // tag
    std::string const& tag() const { return _tag; }
    void set_tag(std::string const& tag) { _tag = tag; }

    // clone
    virtual game_object* clone() const;
    
    // search
    game_object* find_by_tag(char const* tag, bool recursive = true) const; // only search in children
    size_t child_size() const { return _child_size; }
    game_object* child_at(int idx) const;
    
	// tree
	game_object& add_child(game_object* child, game_object* after = nullptr);
	game_object& move_upward();
	game_object& move_downward();
	game_object& move_top();
	game_object& move_bottom();
	game_object& remove_all();
	void remove_self();

    game_object* last_child() const;
    game_object* first_child() const { return _first_child; }
    game_object* next_sibling() const { return _next_sibling; }
    game_object* pre_sibling() const { return _pre_sibling; }
    game_object* parent() const { return _parent; }
    
    bool has_parent() const { return _parent != nullptr; }
    
    // transversal
    void pre_order(uint32_t mark, iterator_t const&) const;
    void post_order(iterator_t const&) const;

    
    // get component - fixed version
    template<typename C>
    typename std::enable_if<std::is_base_of<component, C>::value &&
    C::manager_t::component_fixed_t::value, C*>::type get_component(int idx = C::manager_t::component_idx()) const {
        assert(idx != -1); // manager is not initializer properly?
        return static_cast<C*>(_components[idx].get());
    }
    
    // get component - non-fixed version
    template<typename C>
    typename std::enable_if<std::is_base_of<component, C>::value &&
    !C::manager_t::component_fixed_t::value, C*>::type get_component(int start = component_manager::fixed_component()) const {
        typedef typename C::manager_t trait; // manager class is a trait for the component
        if(trait::sealed_t::value) {
            for(auto it = std::next(_components.begin(), start);
                it != _components.end() && it->get() != nullptr; ++it) {
                if(typeid(*it) == typeid(C))
                    return static_cast<C*>(it->get());
            }
        }
        else {
            for(auto it = std::next(_components.begin(), start);
                it != _components.end() && it->get() != nullptr; ++it) {
                if(dynamic_cast<C*>(it->get()) != nullptr)
                    return static_cast<C*>(it->get());
            }
        }
        return nullptr;
    }
    
    // add component - fixed version
    template<typename C, typename... Args>
    typename std::enable_if<std::is_base_of<component, C>::value &&
    C::manager_t::component_fixed_t::value, C*>::type add_component(Args&&... args) {
        assert(C::manager_t::component_idx() != -1); // manager is not initializer properly?
        auto& existed = _components[C::manager_t::component_idx()];
        if(existed.get() == nullptr)
            existed.reset(C::template create<C>(this, std::forward<Args>(args)...));
        return static_cast<C*>(existed.get());
    }
    
    // add component - non-fixed version
    template<typename C, typename... Args>
    typename std::enable_if<std::is_base_of<component, C>::value &&
    !C::manager_t::component_fixed_t::value, C*>::type add_component(Args&&... args) {
        typedef typename C::manager_t trait; // manager class is a trait for the component
        uint32_t existed = component_manager::fixed_component();
        if(trait::sealed_t::value) {
            for(auto it = _components.begin() + component_manager::fixed_component();
                it != _components.end() && it->get() != nullptr; ++it, ++ existed) {
                if(typeid(*it->get()) == typeid(C))
                    return static_cast<C*>(it->get());
            }
        }
        else {
            for(auto it = _components.begin() + component_manager::fixed_component();
                it != _components.end() && it->get() != nullptr; ++it, ++ existed) {
                if(dynamic_cast<C*>(it->get()) != nullptr)
                    return static_cast<C*>(it->get());
            }
        }
        assert(existed < _components.size()); // components overflow...
        _components[existed].reset(C::template create<C>(this, std::forward<Args>(args)...));
        return static_cast<C*>(_components[existed].get());
    }
    
    // change flag
    uint32_t flag() const { return _flag; }
    void reset_flag() { _flag = 0; }
    void reset_flag(uint32_t offset, uint32_t mask = 1U) { _flag &= ~(mask << offset); }
    void set_flag(uint32_t offset, uint32_t mask = 1U) { _flag |= mask << offset; }
    bool is_set(uint32_t offset, uint32_t mask = 1U) const { return (_flag & mask << offset) != 0; }
    void populate_flag(); // populate from the 'Parent'
    
    // --
    // some helper functions
    
    // track number of lived objects
    static uint32_t number_of_objects() { return _number_of_objects; }
    
    // a pre-allocated root object, so the client won't bother when
    // and where to create one. however, you can choose not to use it
    // if you have special requirements
    static game_object& root() {
        static game_object _root(nullptr);
        return _root;
    }
    
    // being set during transversal
    uint32_t mark() const { return _mark; }
    
private:
    // no copy/assignment? use clone instead
    game_object(game_object const&) = delete;
    game_object& operator =(game_object const&) = delete;
    
    game_object *_first_child, *_parent;
    game_object *_pre_sibling, *_next_sibling;
    size_t _child_size;
    
    uint32_t _flag;
    mutable uint32_t _mark;
    
    std::string _tag;
    components_t _components;

    constexpr static game_object* null = __builtin_constant_p((game_object*)0xFF) ? (game_object*)0xFF : (game_object*)0xFF; // diff than nullptr
    static uint32_t _number_of_objects;
};

#endif