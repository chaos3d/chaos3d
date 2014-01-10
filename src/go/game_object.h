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

#include "common/utility.h"
#include "go/component.h"

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
    // no copy/assignment?
    game_object(game_object const&) = delete;
    game_object& operator =(game_object const&) = delete;
    
    game_object *_first_child, *_parent;
    game_object *_pre_sibling, *_next_sibling;
    
    std::string _tag;

    constexpr static game_object* null = __builtin_constant_p((game_object*)0xFF) ? (game_object*)0xFF : (game_object*)0xFF; // diff than nullptr
};

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