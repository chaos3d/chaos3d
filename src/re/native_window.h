#ifndef _CHAOS3D_RE_NATIVE_WINDOW_H
#define _CHAOS3D_RE_NATIVE_WINDOW_H

#include <memory>
#include "event/event_dispatcher.h"
#include "common/referenced_count.h"

// thin wrapper for system dependent top-level window
class native_window : public referenced_count, public event_dispatcher {
public:
    typedef std::unique_ptr<native_window, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<native_window const, referenced_count::release_deleter> const_ptr;
    
public:
    virtual void* handle() const = 0;
    ~native_window() {};
};

#endif
