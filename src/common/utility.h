#ifndef _UTILITY_H
#define _UTILITY_H

#include <type_traits>
#include <memory>

// one-liner that the function returns the pointer and it will be only used once
//  it is wrapped by a unique_ptr that it'll be released in its destructor
#define WRAP_PTR(exp) std::unique_ptr<std::remove_pointer<decltype(exp)>::type>(exp).get()

// TODO: move this to utility header
#define ATTRIBUTE(type, name) \
    private: type _##name; \
    public: type const& name () const { return _##name; } \
    public: type & name () { return _##name; } \
    public: void set_##name (type const& name) { _##name = name; } 

template<class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// unpacking to a list of numbers...
// usage:
//template<typename... Args>
//void callFunc(Args... args) {
//    func(std::get<typename tuple_gens<sizeof...(Args)>::type()>(args) ...);
//}
template<int ...> struct tuple_seq { };
template<int N, int ...S> struct tuple_gens : tuple_gens<N-1, N-1, S...> { };
template<int ...S> struct tuple_gens<0, S...> { typedef tuple_seq<S...> type; };

// get the index of a type in a tuplle
template<int Index, class Search, class First, class... Types>
struct tuple_get_index
{
    typedef typename tuple_get_index<Index + 1, Search, Types...>::type type;
    static constexpr int index = Index;
};

template<int Index, class Search, class... Types>
struct tuple_get_index<Index, Search, Search, Types...>
{
    typedef tuple_get_index type;
    static constexpr int index = Index;
};

template<class T, class... Types>
T tuple_get(std::tuple<Types...> const& tuple)
{
    return std::get<tuple_get_index<0,T,Types...>::type::index>(tuple);
}
#endif