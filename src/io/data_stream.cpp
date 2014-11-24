#include "data_stream.h"
#include "common/log.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

DEFINE_LOGGER(data_stream, "io");

namespace script {
    static int c3d_lua_stream_read(lua_State* L) {
        data_stream& stream = converter<data_stream&>::from(L, 1, nullptr);
        
        // TODO: different manner of reading
        luaL_Buffer B;
        luaL_buffinit(L, &B);
        while (!stream.end()) {
            char *buf = luaL_prepbuffer(&B);
            auto n = stream.read(buf, LUAL_BUFFERSIZE);
            if (n > 0)
                luaL_addsize(&B, n);
        };
        luaL_pushresult(&B);
        return 1;
    }
    
    void def_stream(state*, std::string const& scope) {
        class_<data_stream>::type()
        .def("read", &c3d_lua_stream_read)
        ;
    }
}

bool data_stream::reset() {
    // it is equal to set pointer to the start unless there is a special handler
    return seek(0, SeekSet);
};