#ifndef _COMPONENT_H
#define _COMPONENT_H

#if 0
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>
#endif

#include <vector>
#include <cassert>
#include "common/utility.h"

class game_object;

class component {
public:
    component(game_object* go) : _parent(go)
    {}
    
    virtual ~component() {};
    
    // clone to the new game object
    virtual component* clone(game_object*) const = 0;
    
    game_object* parent() const{ return _parent; }
    
    template<typename C, typename ...Args>
    static C* create(game_object* go, Args&&... args) {
        return new C(go, std::forward<Args>(args)...);
    }
    
    virtual void destroy() {
        delete this;
    }
    
private:
    game_object* _parent;
};

#if 0
class component_dirty : public component {
public:
    bool dirty() const { return _dirty; }
    void mark_dirty() { _dirty = true; }
    void clear_dirty() { _dirty = false; }
    
private:
    bool _dirty;
};
#endif
#endif