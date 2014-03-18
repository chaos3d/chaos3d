#ifndef _CHAOS3D_SCRIPT_LUA_BIND_H
#define _CHAOS3D_SCRIPT_LUA_BIND_H

#include <tuple>
#include <type_traits>
#include <utility>
#include <liblua/lua/lua.hpp>
#include "common/utility.h"
#include "script/converter.h"
#include "script/converter_ptr.h"
#include "script/type_info.h"

namespace script {
    struct return_to_reference {};

    // out_value policy need external storage?
    // and only applies to lvalue reference (pointers??)
    struct out_value {};
    
    struct policy_helper {
        template<class Policy>
        int apply(lua_State* L, Policy const&);
        
        int apply(lua_State* L, return_to_reference const&) {
            return 0;
        }
    
        template<class P>
        using is_member = std::is_member_function_pointer<void (policy_helper::*)(lua_State*L, P const&)>;
        
        template<class Policy, typename std::enable_if<is_member<Policy>::value>::type* = nullptr>
        int apply_helper(lua_State* L) {
            return apply(L, Policy());
        }
        
        template<class Policy, typename std::enable_if<!is_member<Policy>::value>::type* = nullptr>
        int apply_helper(lua_State* L) {
            return 0;
        }
        
        template<class Policy, class... Policies>
        int apply(lua_State* L) {
            int n = apply_helper<Policy>(L);
            return n + apply<Policies...>(L);
        }

        // termination
        template<class... Policies, typename std::enable_if<sizeof...(Policies) == 0>::type* = nullptr>
        int apply(lua_State* L) { return 0; };
    };
    
    template<class C, bool _Is = true>
    using is_void = std::enable_if<std::is_same<void, C>::value == _Is>;

    template<class... Ts, typename std::enable_if<sizeof...(Ts) == 0>::type* = nullptr>
    constexpr size_t storage_of(size_t = 0) {
        return 0;
    };

    template<class T, class... Ts>
    constexpr size_t storage_of(size_t idx = sizeof...(Ts)) {
        using T0 = typename std::remove_reference<T>::type;
        return idx == 0 ? 0 :
        ((std::is_lvalue_reference<T>::value && std::is_arithmetic<T0>::value
          && !std::is_const<T0>::value
          ? sizeof(T0)
          : 0) + storage_of<Ts...>(idx - 1));
    };
    
    template<class F>
    struct function_wrapper{
        template<class R, class... Policy>
        static int push_results(lua_State* L, R&& result) {
            converter<R>::to(L, std::forward<R>(result));
            return 1 + policy_helper().apply<Policy...>(L);
        }
        
#if 0
        // TODO
        template<class... Rs, class... Policy>
        static int push_results(lua_State* L, std::tuple<Rs...> const& result) {
            converter<R>::to(L, std::forward<R>(result));
            return 1 + policy_helper().apply<Policy...>(L);
        }
#endif
        
        // regular function with returns
        template<class R, class... Args, int ...N, class... Policy, typename is_void<R, false>::type* = nullptr>
        static int wrapper_internal(lua_State* L, R (*ptr)(Args...), tuple_seq<N...>,
                                    Policy const&... policies) {
            char storage[storage_of<Args...>()];
            return push_results<R, Policy...>(L,
                                              ptr(converter<Args>::from(L, N + 1,
                                                                        &storage[storage_of<Args...>(N)])...));
        }
        
        // regular function without returns (void)
        template<class R, class... Args, int ...N, class... Policy, typename is_void<R>::type* = nullptr>
        static int wrapper_internal(lua_State* L, R (*ptr)(Args...), tuple_seq<N...>,
                                    Policy const&... policies) {
            char storage[storage_of<Args...>()];
            ptr(converter<Args>::from(L, N + 1, &storage[storage_of<Args...>(N)])...);
            return policy_helper().apply<Policy...>(L);
        }

        // member function with returns
        template<class R, class C, class... Args, int ...N, class... Policy,
        typename is_void<R, false>::type* = nullptr>
        static int wrapper_internal(lua_State* L, R (C::*ptr)(Args...), tuple_seq<N...>) {
            char storage[storage_of<Args...>()];
            C& obj = converter<C&>::from(L, 1, nullptr); // C will never be a reference
            return push_results<R, Policy...>
            (L, (obj.*ptr)(converter<Args>::from(L, N + 2, &storage[storage_of<Args...>(N)])...));
        }
        
        // member function without returns (void)
        template<class R, class C, class... Args, int ...N, class... Policy,
        typename is_void<R>::type* = nullptr>
        static int wrapper_internal(lua_State* L, R (C::*ptr)(Args...), tuple_seq<N...>) {
            char storage[storage_of<Args...>()];
            C& obj = converter<C&>::from(L, 1, nullptr);
            (obj.*ptr)(converter<Args>::from(L, N + 2, &storage[storage_of<Args...>(N)])...);
            return policy_helper().apply<Policy...>(L);
        }
        
        // const member function with returns
        // this should always have returns
        template<class R, class C, class... Args, int ...N, class... Policy>
        static int wrapper_internal(lua_State* L, R (C::*ptr)(Args...) const, tuple_seq<N...>) {
            char storage[storage_of<Args...>()];
            C& obj = converter<C&>::from(L, 1, nullptr);
            return push_results<R, Policy...>
            (L, (obj.*ptr)(converter<Args>::from(L, N + 2, &storage[storage_of<Args...>(N)])...));
        }
        
        // to deduce function pointer
        template<class R, class... Args, class... Policy>
        static int wrapper(lua_State* L, R (*ptr)(Args...), Policy const&... policies) {
            return wrapper_internal<R, Args...>(L, ptr,
                                                typename tuple_gens<sizeof...(Args)>::type(),
                                                policies...);
        }

        // to deduce member function pointer
        template<class R, class C, class... Args, class... Policy>
        static int wrapper(lua_State* L, R (C::*ptr)(Args...), Policy const&... policies) {
            return wrapper_internal<R, C, Args...>(L, ptr,
                                                   typename tuple_gens<sizeof...(Args)>::type(),
                                                   policies...);
        }
        
        // to deduce const member function pointer
        template<class R, class C, class... Args, class... Policy>
        static int wrapper(lua_State* L, R (C::*ptr)(Args...) const, Policy const&... policies) {
            return wrapper_internal<R, C, Args...>(L, ptr,
                                                   typename tuple_gens<sizeof...(Args)>::type(),
                                                   policies...);
        }
        
        template<F f, class... Policy>
        static int lua_call(lua_State* L) {
            return wrapper(L, f, Policy() ...);
        };
    };
}

#define LUA_GET_BIND(_1,_2,NAME,...) NAME
#define LUA_BIND0(func) (&script::function_wrapper<decltype(func)>::lua_call<func>)
#define LUA_BIND1(func, p) (&script::function_wrapper<decltype(func), p>::lua_call<func>)

#define LUA_GET_BIND_S(_1,_2,_3,NAME,...) NAME
#define LUA_BIND_S0(sig, func) (&script::function_wrapper<sig>::lua_call<func>)
#define LUA_BIND_S1(sig, func, p) (&script::function_wrapper<sig, p>::lua_call<func>)

#define LUA_BIND(...) (LUA_GET_BIND(__VA_ARGS__, LUA_BIND1, LUA_BIND0) (__VA_ARGS__))
#define LUA_BIND_S(...) (LUA_GET_BIND_S(__VA_ARGS__, LUA_BIND_S1, LUA_BIND_S0) (__VA_ARGS__))

#endif