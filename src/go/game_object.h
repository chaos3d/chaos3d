#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>

#include <cassert>
#include <vector>
#include <string>
#include <memory>

class component;

//template<class C1 = _nil_component, class C2 = _nil_component, class C3 = _nil_component,
//    class C4 = _nil_component, class C5 = _nil_component, class C6 = _nil_component,
//    class C7 = _nil_component, class C8 = _nil_component>
template<class ... Cs>
class game_object {
public:
#if 0
    typedef typename ::boost::mpl::remove_if<
    ::boost::mpl::vector<Cs...>,
    ::boost::mpl::equal_to<::boost::mpl::_, _nil_component>
    >::type component_list;
#endif
    typedef ::boost::mpl::vector<Cs ...> component_list_type;
    typedef typename ::boost::mpl::end<component_list_type>::type component_list_last;
    typedef typename ::boost::mpl::size<component_list_type>::type component_size;
    typedef std::vector<component*> component_list;
    
public:
    game_object() : _first_child(nullptr), _parent(nullptr),
    _next_sibling(nullptr), _pre_sibling(nullptr){
        memset(_baked_components, 0, sizeof(_baked_components));
    }
    
    template<class T>
    T& add_component() {
        typedef typename ::boost::mpl::find<component_list_type, T>::type iter;
    }
    
    template<class T>
    T* get_component() {
        typedef typename ::boost::mpl::find<component_list_type, T>::type iter;
        
        typedef typename ::boost::mpl::if_c<::boost::is_same<iter, component_list_last>::value,
            T*, ::boost::mpl::true_
            >::type result;
        return static_cast<T*>(get_component_t(iter::pos::value, result()));
    }

    component* get_component(int idx) {
        assert(idx >= 0 && idx < component_size::value + _components.size());
        if(idx < component_size::value)
            return _baked_components[idx];
        else
            return _components[idx - component_size::value];
    }
    
    std::string const& tag() const { return _tag; }
    void set_tag(std::string const& tag) { _tag = tag; }
    
    // search
    game_object* find_by_tag(char const* tag, bool recursive = true); // only search in children
    game_object* child_at(int idx);
    
	// tree
	game_object& add_hild( game_object* child, game_object* after = 0 );
	game_object& remove_all();
	game_object& remove_self();
	game_object& moveUpward();
	game_object& moveDownward();
	game_object& moveTop();
	game_object& moveBottom();
	game_object& moveAfterward(game_object*);

    //transform?
	//void relocate_To(game_object* parent, game_object* after = 0);
    
private:
    // no copy/assignment?
    game_object(game_object const&);
    game_object& operator =(game_object const&);
    
    component* get_component_t(int idx, ::boost::mpl::true_ const& r = ::boost::mpl::true_()) {
        assert(idx >= 0 && idx < component_size::value);
        return _baked_components[idx];
    }
    
    template<class T>
    T* get_component_t(int _0 = 0, T* _1 = nullptr) {
        int first = component_size::value;
        T* c = nullptr;
        for(component_list::iterator it = _components.begin() + first; it != _components.end(); ++it) {
            //if((c = dynamic_cast<T*>(*it)) != nullptr)
            if(typeid(T) == typeid(*it))
                return c;
        }
        return nullptr;
    }
    
    game_object *_first_child, *_parent;
    game_object *_pre_sibling, *_next_sibling;
    component* _baked_components[component_size::value];
    component_list _components;
    
    std::string _tag;
};

template<class ... Cs>
game_object<Cs...>* game_object<Cs...>::find_by_tag(char const* tag, bool recursive) {
	game_object<Cs...> *node = nullptr;
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