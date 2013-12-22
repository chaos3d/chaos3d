#ifndef _UTILITY_H
#define _UTILITY_H

#include <type_traits>

// one-liner that the function returns the pointer and it will be only used once
//  it is wrapped by a unique_ptr that it'll be released in its destructor
#define WRAP_PTR(exp) std::unique_ptr<std::remove_pointer<decltype(exp)>::type>(exp).get()

#endif