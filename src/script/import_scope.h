#ifndef _CHAOS3D_SCRIPT_IMPORT_SCOPE_H
#define _CHAOS3D_SCRIPT_IMPORT_SCOPE_H

#include <typeinfo>
#include <vector>
#include <unordered_map>
#include "script/class_type.h"
#include "script/converter.h"
#include "script/converter_ptr.h"

#define LUA_ENUM(s, e)     #e, s::e

namespace script {
    class state;
    class type_info;
    
    // helper class to import symbols into a Lua scope
    class import_scope {
    public:
        // import functions
        import_scope& def(char const* name, lua_function_t func);
        
        template<typename C, typename R = C>
        import_scope& def_singleton_getter(char const* name) {
            return def(name, &singleton_getter<C, R>);
        }
        
        // import a variable as a type
        template<class C>
        import_scope& import(char const* name, C value) {
            converter<C>::to(get_L(), value);
            return save(name);
        }

        template<class C>
        import_scope& import_type() {
            auto* L = get_L();
            lua_pushlightuserdata(L, (void*)&typeid(C));
            lua_pushlightuserdata(L, (void*)&class_<C>::type());
            lua_rawset(L, LUA_REGISTRYINDEX);
            return *this;
        }
#if 0
        template<class C>
        import_scope& def(char const* name, C*&& value) {
            return def(name, new C(std::forward<C*>(value)), &class_<C>::type());
        }
#endif
        
        // pop the scope
        state& end();
        
        ~import_scope() {
            if (_state)
                end();
        };
        
    public:
        template<typename C, typename R = C>
        static int singleton_getter(lua_State* L) {
            converter<R*>::to(L, &C::instance());
            return 1;
        }
    private:
        import_scope(char const*, state&);
        
        // assign the value on the top to the scope
        import_scope& save(char const*);

        lua_State* get_L() const;
        
        import_scope(import_scope&&) = default;
        import_scope& operator=(import_scope&&) = default;
        import_scope(import_scope&) = delete;
        import_scope& operator=(import_scope&) = delete;
        
        state* _state;
        friend class state;
    };
    
    template<>
    import_scope& import_scope::import<std::type_info const*>(char const*, std::type_info const*);
}
#endif