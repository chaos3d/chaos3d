#ifndef _CHAOS3D_IO_CONVERTER_H
#define _CHAOS3D_IO_CONVERTER_H

#include <cassert>

// specialize this template to inject the conversion
// into the class constructor
template<class Loader, class T>
struct io_converter {
    bool operator() (Loader const&, T&) const {
        assert(false); // not implemented
        return false;
    }
};

#endif