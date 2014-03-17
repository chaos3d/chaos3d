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
        
        // import a variable as a type
        template<class C>
        import_scope& import(char const* name, C value) {
            converter<C>::to(get_L(), value);
            return save(name);
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
}
#endif