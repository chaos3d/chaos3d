#ifndef _CHAOS3D_SCRIPT_LUA_REF_H
#define _CHAOS3D_SCRIPT_LUA_REF_H

#include <memory>
#include <liblua/lua/lua.hpp>

namespace script {
    class state;
    class coroutine;
    
    class ref {
    public:
        typedef std::shared_ptr<state> parent_ptr;
        
    public:
        // an invalid/empty ref
        ref() : _ref(-1) {};
        
        // from the object on top of the stack
        ref(state*);
        ref(lua_State*);
        ref(coroutine&);
        
        ref(ref &&rhs)
        : _ref(rhs._ref), _parent(std::move(rhs._parent)) {
            rhs._ref = -1;
        }
        
        ref& operator=(ref &&rhs) {
            _ref = rhs._ref;
            _parent = std::move(rhs._parent);
            rhs._ref = -1;
            return *this;
        }
        
        void push(lua_State* L) const {
            if (_ref > -1)
                lua_getref(L, _ref);
            else
                lua_pushnil(L);
        }
        
        // explicit delete copy constructor to avoid
        // accidental copy/assignment, use copy to
        // explicitly retrieve a new copy
        ref(ref const&) = delete;
        ref& operator=(ref const&) = delete;
        ref copy() const;
               
        ~ref() { reset(); };
        
        // release the ownership and the parent state
        int release();
        
        // reset the internal reference to a new one at the top of stack
        void reset(state* = nullptr);
        
        parent_ptr const& parent() const { return _parent; }
    private:
        int _ref;
        parent_ptr _parent;
    };
}
#endif