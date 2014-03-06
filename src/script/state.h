#ifndef _CHAOS3D_SCRIPT_STATE_H
#define _CHAOS3D_SCRIPT_STATE_H

#include <type_traits>
#include <memory>
#include <liblua/lua/lua.hpp>
#include "script/lua_ref.h"
#include "common/utility.h"

class data_stream;

namespace script {
    class state;
    
    class coroutine {
    public:
        struct deleter {
            void operator() (coroutine*) const;
        };
        
        typedef std::unique_ptr<coroutine, deleter> ptr;
        typedef std::weak_ptr<state> parent_ptr;
        
    public:
        ~coroutine();

        coroutine(coroutine&& rhs)
        : _L(rhs._L), _co_ref(std::move(rhs._co_ref)) {
            rhs._L = NULL;
        }
        
        coroutine& operator=(coroutine&& rhs) {
            _L = rhs._L;
            rhs._L = NULL;
            _co_ref = std::move(rhs._co_ref);
            return *this;
        }
        
        coroutine& resume();
        bool is_yielded() const;
        bool is_resumable() const;
        
        // for other wrappers, do not use
        lua_State* internal() const { return _L; }
        parent_ptr const& parent() const { return _co_ref.parent(); }
        
    private:
        coroutine(coroutine const&) = delete;
        coroutine& operator=(coroutine const&) = delete;
        
        // create from the thread being on top of the state
        coroutine(state* parent);
        
        lua_State* _L;
        ref _co_ref;
        
        friend class state;
        friend struct deleter;
    };

    class state : public std::enable_shared_from_this<state> {
    public:
        typedef std::shared_ptr<state> ptr;
        
    public:
        ~state();
        
        coroutine load(char const*, char const* = nullptr);
        coroutine load(data_stream*, char const* = nullptr);
        
        // internal ref for other wrappers, do not use
        lua_State* internal() const { return _L; };
        
    private:
        state(bool = true);
        void recycle(coroutine &&);
        coroutine fetch();
        
        lua_State* _L;
        
        CONSTRUCTOR_FOR_SHARED(state);
        
        friend class coroutine;
    };
}
#endif