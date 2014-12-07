#ifndef _CHAOS3D_SCRIPT_TRAITS_H
#define _CHAOS3D_SCRIPT_TRAITS_H

#include <string>
#include <memory>
#include <vector>
#include <type_traits>
#include "script/lua_ref.h"

namespace script {
    template<class C>
    using is_bool = std::is_same<C, bool>;
    
    template<class C>
    struct is_number
    : public std::conditional<std::is_arithmetic<C>::value && !is_bool<C>::value, std::true_type, std::false_type>::type {};
    
    template<class C>
    struct is_userdata
    : public std::conditional<is_number<C>::value, std::false_type, std::true_type>::type {};

    template<>
    struct is_userdata<bool> : public std::false_type {};
    
    template<>
    struct is_userdata<char const*> : public std::false_type {};

    template<>
    struct is_userdata<ref> : public std::false_type {};

    template<int N>
    struct is_userdata<char [N]> : public std::false_type {};

    template<>
    struct is_userdata<std::string> : public std::false_type {};

    template<class C, class... As>
    struct is_userdata<std::vector<C, As...>> : public std::false_type {};

    template<class P, class... Args>
    struct is_userdata<std::unique_ptr<P, Args...>> : public std::false_type {};
    
    template<class P, class... Args>
    struct is_userdata<std::unique_ptr<P, Args...> const&> : public std::false_type {};
    
    template<class P, class... Args>
    struct is_userdata<std::unique_ptr<P, Args...> const> : public std::false_type {};
    
    template<class P>
    struct is_userdata<std::shared_ptr<P>> : public std::false_type {};
    
    template<class P>
    struct is_userdata<std::shared_ptr<P> const&> : public std::false_type {};
    
    template<class P>
    struct is_userdata<std::shared_ptr<P> const> : public std::false_type {};

    template<class P>
    struct is_userdata<std::weak_ptr<P>> : public std::false_type {};

    template<class C, class... As>
    struct vector_of {};

    template<class C, class... As>
    struct vector_of<std::vector<C, As...>> {
        typedef C type;
    };

    template<class C, class... As>
    struct vector_of<std::list<C, As...>> {
        typedef C type;
    };
    
    template<class P>
    struct unwrap_ptr {
        typedef P type;
    };

    template<class P, class D>
    struct unwrap_ptr<std::unique_ptr<P, D>> {
        typedef P type;
    };

    template<class P>
    struct unwrap_ptr<std::shared_ptr<P>> {
        typedef P type;
    };

    template<class P>
    struct unwrap_ptr<std::weak_ptr<P>> {
        typedef P type;
    };
}

#endif