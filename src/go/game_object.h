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

#include "component.h"

class component;
class transform;

class game_object {
public:
    typedef ::boost::mpl::vector<transform> component_list_type;
    typedef typename ::boost::mpl::end<component_list_type>::type component_list_last;
    typedef typename ::boost::mpl::size<component_list_type>::type component_size;
    typedef std::unique_ptr<component> component_ptr;
    typedef std::vector<component_ptr> components_t;
    
    template<class C>
    struct find_component_t {
        constexpr static bool value = std::is_same<typename boost::mpl::find<component_list_type, C>::type,
        component_list_last>::value == false;
    };
    
    // TODO: maybe component manager traits?
    template<class C>
    struct component_is_dynamic {
        constexpr static bool value = std::is_polymorphic<C>::value == true;
    };
    
public:
    game_object() : _first_child(null), _parent(nullptr),
    _next_sibling(null), _pre_sibling(null){
    }
    
    std::string const& tag() const { return _tag; }
    void set_tag(std::string const& tag) { _tag = tag; }
    
    template<typename C>
    typename std::enable_if<find_component_t<C>::value == true, C>::type*
    find_component() const{
        typedef typename ::boost::mpl::find<component_list_type, C>::type iter;
        size_t idx = iter::pos::value;
        assert(idx >= 0 && idx < component_size::value);
        return static_cast<C*>(_components[idx].get());
    }
    
    template<typename C>
    typename std::enable_if<find_component_t<C>::value == false, C>::type*
    find_component() const {
        int first = component_size::value;
        auto r = std::find_if(_components.begin() + first, _components.end(), [](component_ptr const& rhs) {
            return component_equal<C>(rhs.get());
        });
        return r != _components.end() ? static_cast<C*>(r->get()) : nullptr;
    }
    
    template<class C>
    typename std::enable_if<find_component_t<C>::value == true, C>::type*
    add_component(std::unique_ptr<C>&& com){
        typedef typename ::boost::mpl::find<component_list_type, C>::type iter;
        size_t idx = iter::pos::value;
        assert(idx >= 0 && idx < component_size::value);
        _components[idx].reset(com.release());
        return com.get();
    }
    
    template<class C>
    typename std::enable_if<find_component_t<C>::value == false, C>::type*
    add_component(std::unique_ptr<C>&& com) {
        _components.emplace_back(std::forward<std::unique_ptr<C>>(com));
        return com.get();
    }

    // TODO: a separate component?
    
    // search
    game_object* find_by_tag(char const* tag, bool recursive = true); // only search in children
    size_t child_size() const {
        size_t size = 0;
        for(game_object* child = _first_child;child != null;++size)
            ;
        return size;
    }
    
    game_object* child_at(int idx) const {
        game_object* child = _first_child;
        if(idx < 0)
            idx = child_size() + idx;
        for(;child != null && idx > 0; -- idx)
            child = child->_next_sibling;
        return child;
    }
    
	// tree
	game_object& add_child(game_object* child, game_object* after = nullptr);
	game_object& remove_all();
	game_object& remove_self();
	game_object& move_upward();
	game_object& move_downward();
	game_object& move_top();
	game_object& move_bottom();
	game_object& move_afterward(game_object*);

    //transform?
	//void relocate_To(game_object* parent, game_object* after = 0);

private:
    template<class T>
    typename std::enable_if<component_is_dynamic<T>::value == true, bool>::type
    static component_equal(component* other) {
        return dynamic_cast<T*>(other) != nullptr;
    }

    template<class T>
    typename std::enable_if<component_is_dynamic<T>::value == false, bool>::type
    static component_equal(component* other) {
        return (typeid(T) == typeid(*other));
    }

    // no copy/assignment?
    game_object(game_object const&) = delete;
    game_object& operator =(game_object const&) = delete;
    
    game_object *_first_child, *_parent;
    game_object *_pre_sibling, *_next_sibling;
    
    std::string _tag;

    components_t _components;
    
    constexpr static game_object* null = __builtin_constant_p((game_object*)0xFF) ? (game_object*)0xFF : (game_object*)0xFF; // diff than nullptr
};

game_object* game_object::find_by_tag(char const* tag, bool recursive) {
	game_object *node = nullptr;
	for(auto *child = _first_child;
        child != 0 && node == 0;
        child = child->_next_sibling)
        if(node->tag() == tag)
            return node;

    if(recursive) {
        for(auto *child = _first_child;
            child != 0 && node == 0;
            child = child->_next_sibling)
            node = child->find_by_tag(tag, true);
    }
    return node;
}

#if 0
game_object* child_at(int idx);

// tree
game_object& add_hild( game_object* child, game_object* after = 0 );
game_object& remove_all();
game_object& remove_self();
//void		relocate_To(game_object* parent, game_object* after = 0);
game_object& moveUpward();
game_object& moveDownward();
game_object& moveTop();
game_object& moveBottom();
game_object& moveAfterward(game_object*);
#endif


#endif