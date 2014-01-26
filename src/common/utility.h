#ifndef _UTILITY_H
#define _UTILITY_H

#include <type_traits>

// one-liner that the function returns the pointer and it will be only used once
//  it is wrapped by a unique_ptr that it'll be released in its destructor
#define WRAP_PTR(exp) std::unique_ptr<std::remove_pointer<decltype(exp)>::type>(exp).get()

// TODO: move this to utility header
#define ATTRIBUTE(type, name) \
    private: type _##name; \
    public: type const& name () const { return _##name; } \
    public: type & name () { return _##name; } \
    public: void set_##name (type const& name) { _##name = name; } 


// unpacking to a list of numbers...
// usage:
//template<typename... Args>
//void callFunc(Args... args) {
//    func(std::get<typename tuple_gens<sizeof...(Args)>::type()>(args) ...);
//}
template<int ...> struct tuple_seq { };
template<int N, int ...S> struct tuple_gens : tuple_gens<N-1, N-1, S...> { };
template<int ...S> struct tuple_gens<0, S...> { typedef tuple_seq<S...> type; };

#endif