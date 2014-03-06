#ifndef _CHAOS3D_SCRIPT_LUA_REF_H
#define _CHAOS3D_SCRIPT_LUA_REF_H

#include <memory>
#include <liblua/lua/lua.hpp>
#include "script/state.h"

namespace script {
    class state;
    class coroutine;
    
    class ref {
    public:
        typedef std::weak_ptr<state> parent_ptr;
        
    public:
        // an invalid/empty ref
        ref();
        
        // from the object on top of the stack
        ref(state*);
        ref(coroutine&);
        
        ref(ref &&rhs)
        : _ref(rhs._ref), _parent(std::move(rhs._parent)) {
            rhs._ref = -1;
        }
        
        ref& operator=(ref &&rhs) {
            _ref = rhs._ref;
            _parent = std::move(rhs._parent);
            rhs._ref = -1;
        }
        
        // explicit delete copy constructor to avoid
        // accidental copy/assignment, use copy to
        // explicitly retrieve a new copy
        ref(ref const&) = delete;
        ref& operator=(ref const&) = delete;
        
        ref copy() const {
            if (_parent.expired())
                return ref();
            else {
                return ref(_parent.lock().get());
            }
        }
        
    private:
        int _ref;
        parent_ptr _parent;
    };
}
#endif