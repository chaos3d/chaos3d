#ifndef _ACTION_LUA_H
#define _ACTION_LUA_H

#include "action/action.h"
#include "io/data_stream.h"

class action_lua : public action {
public:
    action_lua(data_stream::ptr const&);
    
    static action* from(data_stream::ptr const& ptr) {
        return new action_lua(ptr);
    }
private:
    struct internal;
    
};

#endif