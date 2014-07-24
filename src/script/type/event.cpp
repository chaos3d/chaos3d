#include "script/type/def.h"
#include "script/state.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/lua_ref.h"
#include "script/converter_ptr.h"

#include "event/event_listener.h"
#include "event/touch_event.h"

#include <vector>

namespace script {
    class lua_event_listener : public event_listener {
    public:
        lua_event_listener(lua_State* L)
        : _function(L) {
            
        }
        
    private:
        virtual bool on_event(event const& evt) override{
            lua_State* L = _function.parent()->internal();

            _function.push(L);
            type_info* type = nullptr;
            lua_pushlightuserdata(L, (void*)&typeid(evt));
            lua_rawget(L, LUA_REGISTRYINDEX);
            type = (type_info*)lua_touserdata(L, -1);
            if (type == nullptr) {
                type = &class_<event>::type();
            }
            lua_pop(L, 1);
            
            auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
            wrapper->object = (void*)&evt;
            wrapper->type = type;

            object_meta<pure_pointer_tag>::push_metatable(L);
            lua_setmetatable(L, -2);
            
            lua_pushlightuserdata(L, (void*)&typeid(evt));
            bool is_done = true;
            if (lua_pcall(L, 2, 1, 0) != 0) {
                // TODO: error
                printf("error: %s", lua_tostring(L, -1));
            } else {
                is_done = lua_toboolean(L, -1);
            }
            lua_pop(L, 1);
            return !is_done;
        }
        
    private:
        ref _function;
    };
    
    static int c3d_lua_make_handler(lua_State* L) {
        luaL_argcheck(L, lua_isfunction(L, 1), 1, "expect a function");

        converter<lua_event_listener::ptr>::to(L,
                                               lua_event_listener::ptr(new lua_event_listener(L)));
        return 1;
    }
    
    static int c3d_lua_register(lua_State* L) {
        lua_event_listener::ptr listener;
        state* st = nullptr;
        lua_getallocf(L, (void**)&st);

        luaL_argcheck(L, lua_gettop(L) >= 3, 3, "no event specified");
        event_dispatcher& dispatcher = converter<event_dispatcher&>::from(L, 1, nullptr);
        
        if (lua_isfunction(L, 2)) {
            lua_pushvalue(L, 2);
            listener.reset(new lua_event_listener(L));
        } else {
            lua_event_listener& handler = converter<lua_event_listener&>::from(L, 2, nullptr);
            handler.retain();
            listener.reset(&handler);
        }

        std::vector<std::type_index> evts;
        evts.reserve(lua_gettop(L) - 2);
        for (int i = 3, t = lua_gettop(L); i <= t; ++i) {
            auto* type = (std::type_info*) lua_touserdata(L, i);
            luaL_argcheck(L, type != nullptr, i, "not valid event type");
            assert(type != nullptr);
            evts.emplace_back(*type);
        }
        
        dispatcher.register_listener(evts, listener.get());
        converter<lua_event_listener::ptr>::to(L, std::move(listener));
        return 1;
    }
    
    static int c3d_lua_event_where(lua_State *L) {
        touch_event& event = converter<touch_event&>::from(L, 1, nullptr);
        auto pos = event.position();
        lua_pushnumber(L, pos.x());
        lua_pushnumber(L, pos.y());
        return 2;
    }
    
    void def_event(state* st, std::string const& scope) {
        st->import((scope + ".evt").c_str())
        .def("make", c3d_lua_make_handler)
        .import("touch_began", &typeid(touch_began_event))
        .import("touch_moved", &typeid(touch_moved_event))
        .import("touch_ended", &typeid(touch_ended_event))
        .import("touch_cancelled", &typeid(touch_cancelled_event))
        .import_type<touch_began_event>()
        .import_type<touch_moved_event>()
        .import_type<touch_ended_event>()
        .import_type<touch_cancelled_event>()
        ;
        
        class_<event_dispatcher>::type()
        .def("register", c3d_lua_register)
        ;
        
        class_<touch_event>::type()
        .def("which", LUA_BIND_S(touch_event::pointer_t const& (touch_event::*)() const,
                                 &touch_event::which))
        .def("where", c3d_lua_event_where)
        ;
        
        class_<touch_began_event>::type()
        .derive<touch_event>();
        
        class_<touch_moved_event>::type()
        .derive<touch_event>();
        
        class_<touch_ended_event>::type()
        .derive<touch_event>();
        
        class_<touch_cancelled_event>::type()
        .derive<touch_event>();
    }
}
