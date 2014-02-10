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
    struct component_deleter {
        void operator() (component* com) const {
            com->destroy();
        }
    };
public:
    component(component const&) = delete;
    
    component(game_object* go) : _parent(go)
    {}
    
    // clone to the new game object
    virtual component* clone(game_object*) const = 0;
    
    game_object* parent() const{ return _parent; }
    
    template<typename C, typename ...Args>
    static C* create(game_object* go, Args&&... args) {
        return new C(go, std::forward<Args>(args)...);
    }
    
    // destroy is needed so we will have a second chance
    // to not delete the object or recycle it
    virtual void destroy() {
        delete this;
    }
 
protected:
    component& operator=(component const&) {
        // doesn't assign game_object
        return *this;
    }
    
    virtual ~component() {};
    
private:
    game_object* _parent;
};

#define SIMPLE_CLONE(type)  protected: virtual type* clone(game_object* go) const override {\
type* com = new type(go); \
*com = *this; \
return com; \
}

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