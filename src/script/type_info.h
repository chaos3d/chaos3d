#ifndef _CHAOS3D_SCRIPT_TYPE_INFO_H
#define _CHAOS3D_SCRIPT_TYPE_INFO_H

#include <typeinfo>
#include <vector>
#include <unordered_map>

namespace script {
    typedef int (*lua_function_t)(lua_State*);

    // type info for the object in Lua
    class type_info {
    public:
        typedef std::unordered_map<std::string, lua_function_t> meta_t;
        typedef std::vector<type_info const*> bases_t;
        
    public:
        virtual lua_function_t find(std::string const&) const = 0;        
    };
    
    // object data saved in userdata
    struct object_wrapper {
        void* object = nullptr;
        type_info* type = nullptr;
    };
}
#endif