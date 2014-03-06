#ifndef _CHAOS3D_SCRIPT_STATE_H
#define _CHAOS3D_SCRIPT_STATE_H

#include <type_traits>
#include <memory>
#include <liblua/lua/lua.hpp>
#include "common/utility.h"

class data_stream;

namespace script {
    class state;
    
    class coroutine : public std::enable_shared_from_this<coroutine> {
    public:
        typedef std::shared_ptr<coroutine> ptr;
        typedef std::weak_ptr<state> parent_ptr;
        
    public:
        // TODO: move this private/add a deleter for shared_ptr
        ~coroutine();

        coroutine(coroutine&& rhs)
        : _L(rhs._L){
            rhs._L = NULL;
        }
        
        coroutine& operator=(coroutine&& rhs) {
            _L = rhs._L;
            rhs._L = NULL;
            return *this;
        }
        
        coroutine& resume();
        bool is_yielded() const;
        bool is_resumable() const;
        
    private:
        coroutine(coroutine const&) = delete;
        coroutine& operator=(coroutine const&) = delete;
        
        coroutine(lua_State* L, state* parent);
        
        lua_State* _L;
        parent_ptr _parent;
        
        friend class state;
    };

    class state : public std::enable_shared_from_this<state> {
    public:
        typedef std::shared_ptr<state> ptr;
        
    public:
        ~state();
        
        coroutine::ptr load(data_stream*, char const* = nullptr);
        
    private:
        state(bool = true);
        lua_State* _L;
        
        CONSTRUCTOR_FOR_SHARED(state);
    };
}
#endif