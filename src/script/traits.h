#ifndef _CHAOS3D_SCRIPT_TRAITS_H
#define _CHAOS3D_SCRIPT_TRAITS_H

namespace script {
    template<class C>
    using is_number = std::is_arithmetic<C>;

    template<class C>
    struct is_userdata
    : public std::conditional<is_number<C>::value, std::false_type, std::true_type>::type {};

    template<>
    struct is_userdata<char const*> : public std::false_type {};

    template<int N>
    struct is_userdata<char [N]> : public std::false_type {};

    template<>
    struct is_userdata<std::string> : public std::false_type {};

    template<class C>
    struct is_userdata<std::vector<C>> : public std::false_type {};

    template<class C, class... As>
    struct vector_of {};

    template<class C, class... As>
    struct vector_of<std::vector<C, As...>> {
        typedef C type;
    };

    template<class P>
    struct unwrap_ptr {
        typedef P type;
    };

    template<class P, class... Args>
    struct unwrap_ptr<std::unique_ptr<P, Args...>> {
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