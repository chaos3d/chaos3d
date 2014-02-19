#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <vector>
#include <cassert>
#include <type_traits>
#include "common/utility.h"
#include "io/io_converter.h"

class game_object;

class component;
template<class T>
struct is_component {
    static const bool value = std::is_base_of<component, T>::value;
};

class component {
public:
    struct component_deleter {
        void operator() (component* com) const {
            com->destroy();
        }
    };
    
public:
    component(game_object* go)
    : _parent(go)
    {}
    
    template<class T, class Loader>
    typename std::enable_if<is_component<T>::value, bool>::type load_from(Loader const& loader) {
        return io_converter<Loader, T>() (loader, static_cast<T&>(*this));
    }
    
    // clone to the new game object
    virtual component* clone(game_object*) const = 0;
    
    // the attached game object
    game_object* parent() const{
        return _parent;
    }
    
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
    component(component const&) = delete;
    
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