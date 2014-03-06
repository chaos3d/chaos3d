#ifndef _CHAOS3D_SCRIPT_CONVERTER_H
#define _CHAOS3D_SCRIPT_CONVERTER_H

#include <type_traits>
#include <liblua/lua/lua.hpp>

namespace script {

    template<class T>
    struct converter {
        typedef typename std::remove_reference<T>::type T0;
        typedef typename std::remove_cv<T0>::type T1;
        
        static typename std::enable_if<std::is_arithmetic<T1>::value, T0>::type
        from(lua_State* L, int idx) {
            return lua_tonumber(L, idx);
        };
        
        static std::enable_if<std::is_arithmetic<T1>::value>
        to(lua_State* L, T&& value) {
            lua_pushnumber(L, value);
        }
        
    };
    
}

#endif