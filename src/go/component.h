#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>

#include <vector>
#include <cassert>

class game_object_t;

class component {
public:
    component(void* go) : _parent(go)
    {
    }

    void* parent() const { return _parent;}
    
private:
    bool _dirty;
    void* _parent;
};

template<class C>
class component_manager {
    template<class ...Cs>
    friend class component_managers;
    
private:
    // only created from manager's manager
    component_manager() {};
    
public:
    void create(void*) {};
    void update() {};
    
};

template<class C>
class cascading_manager{
public:
};

class transform;
template<>
class component_manager<transform> : public cascading_manager<transform> {
    
};

template<class ...Cs>
class components_visitor {
public:
    void update();
};

template<class ...Cs>
class component_managers {
public:
    typedef boost::mpl::vector<Cs...> component_list_t;
    typedef typename boost::mpl::size<component_list_t>::type component_size;
    
    component_managers() : _managers(component_size::value) {
        
    }
    
    template<class C>
    component_manager<C>& get_manager() {
        typedef typename boost::mpl::find<component_list_t, C>::type iter;
        void*& mgr = _managers[iter::pos::value];
        assert(iter::pos::value >= 0 && iter::pos::value < component_size::value);
        if(mgr == nullptr) {
            mgr = new component_manager<C>();
        }
        return *static_cast<component_manager<C>*>(mgr);
    }
private:
    std::vector<void*> _managers;
};
#endif