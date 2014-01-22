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
#include "go/component.h"

#include "sg/transform.h" // or transform_manager.h

class component;

/**
 * the game object is nothing but the connection for all the components.
 * it only has the hierachy info (this may be in the component as well?)
 */
class game_object : public referenced_count{
public:
    typedef ::boost::mpl::vector<com::transform> component_list_type;
    typedef typename ::boost::mpl::end<component_list_type>::type component_list_last;
    typedef typename ::boost::mpl::size<component_list_type>::type component_size;
    typedef std::unique_ptr<component> component_ptr;
    typedef std::vector<component_ptr> components_t;
    typedef std::function<void (game_object const&)> iterator_t;
    
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
    
    enum {Parent = 0x1, Order = 0x2 };
public:
    game_object() : _first_child(null), _parent(nullptr),
    _next_sibling(null), _pre_sibling(null), _child_size(0), _parent_changed(0){
        ++ _number_of_objects;
    }
    
    virtual ~game_object() {
        -- _number_of_objects;
    }
    
    std::string const& tag() const { return _tag; }
    void set_tag(std::string const& tag) { _tag = tag; }

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
    
    //transform?
	//void relocate_To(game_object* parent, game_object* after = 0);

    // transversal
    void pre_order(iterator_t const&) const;
    void post_order(iterator_t const&) const;
    
    com::transform* get_transform() const { return _transform; }
    
    // change flag
    uint16_t parent_changed() const { return _parent_changed; }
    void reset_flag() { _parent_changed = 0; }
    void populate_flag(); // only populate 'Parent' to its immediate children
    
    // --
    // some helper functions
    
    // track number of lived objects
    static uint32_t number_of_objects() { return _number_of_objects; }
    
    // a pre-allocated root object, so the client won't bother when
    // and where to create one. however, you can choose not to use it
    // if you have special requirements
    static game_object& root() {
        static game_object _root;
        return _root;
    }
    
private:
    // no copy/assignment?
    game_object(game_object const&) = delete;
    game_object& operator =(game_object const&) = delete;
    
    game_object *_first_child, *_parent;
    game_object *_pre_sibling, *_next_sibling;
    size_t _child_size;
    
    uint8_t _parent_changed;
    
    std::string _tag;
    com::transform* _transform;

    constexpr static game_object* null = __builtin_constant_p((game_object*)0xFF) ? (game_object*)0xFF : (game_object*)0xFF; // diff than nullptr
    static uint32_t _number_of_objects;
};

#endif