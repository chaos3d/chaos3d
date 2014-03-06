#ifndef _CHAOS3D_SCRIPT_LUA_BIND_H
#define _CHAOS3D_SCRIPT_LUA_BIND_H

#include <type_traits>
#include <liblua/lua/lua.hpp>
#include "common/utility.h"
#include "script/converter.h"

namespace script {
    typedef int (*lua_funciont_t) (lua_State*);
    
    template<class F, class Policies = void>
    struct function_wrapper{
        template<class R, class... Args, int ...N>
        static int wrapper_internal(lua_State* L, R (*ptr)(Args...), tuple_seq<N...>) {
            ptr(converter<Args>::from(L, N)...);
            return 0;
        }
        
        template<class R, class C, class... Args, int ...N>
        static int wrapper_internal(lua_State* L, R (C::*ptr)(Args...), tuple_seq<N...>) {
            return 0;
        }
        
        template<class R, class... Args>
        static int wrapper(lua_State* L, R (*ptr)(Args...)) {
            return wrapper_internal<R, Args...>(L, ptr,
                                                typename tuple_gens<sizeof...(Args)>::type());
        }

        template<class R, class C, class... Args>
        static int wrapper(lua_State* L, R (C::*ptr)(Args...)) {
            return wrapper_internal<R, Args...>(L, ptr,
                                                typename tuple_gens<sizeof...(Args)>::type());
        }
        
        template<F f>
        static int lua_call(lua_State* L) {
            return wrapper(L, f);
        };
    };
    
    template<class R, class... Args>
    auto function_deduce(R (*ptr)(Args...)) -> decltype(ptr) {
        return ptr;//&function_wrapper<R, Args...>::template lua_call;
    }
    
//    template<class F, F _F>
//    lua_funciont_t function_deduce2(){
//        return ptr;//&function_wrapper<R, Args...>::template lua_call;
//    }

    template<class R, class... Args>
    lua_funciont_t function_deduce2(R (*ptr)(Args...)) {
        return nullptr;//&function_wrapper<R, Args...>::template lua_call<>;
    }
    
    
    void def(char const* name, lua_funciont_t f) {
        //function_wrapper<void, f>(nullptr);
    }
}

#define LUA_GET_BIND(_1,_2,NAME,...) NAME
#define LUA_BIND0(func) (&script::function_wrapper<decltype(func)>::lua_call<func>)
#define LUA_BIND1(func, p) (&script::function_wrapper<decltype(func), p>::lua_call<func>)

#define LUA_BIND(...) (LUA_GET_BIND(__VA_ARGS__, LUA_BIND1, LUA_BIND0) (__VA_ARGS__))

#endif