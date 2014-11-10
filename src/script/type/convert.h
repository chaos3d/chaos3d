#ifndef _CHAOS3D_SCRIPT_TYPE_CONVERT_H
#define _CHAOS3D_SCRIPT_TYPE_CONVERT_H

#include <Eigen/Dense>
#include <liblua/lua/lua.hpp>
#include "script/converter.h"
#include "action/action_keyframe.h"

namespace script {
    typedef Eigen::Vector2f vector2f;
    typedef Eigen::Vector3f vector3f;
    typedef Eigen::Vector4f vector4f;
    typedef Eigen::AlignedBox2f box2f;
    typedef Eigen::Quaternionf quaternionf;
    typedef Eigen::Matrix4f matrix4f;
    
    template<> struct convert_from_lua<vector2f> {
        typedef std::true_type convertable;
        static vector2f convert(lua_State* L, int idx);
    };

    template<> struct convert_from_lua<vector3f> {
        typedef std::true_type convertable;
        static vector3f convert(lua_State* L, int idx);
    };

    template<> struct convert_from_lua<vector4f> {
        typedef std::true_type convertable;
        static vector4f convert(lua_State* L, int idx);
    };
    
    template<> struct convert_from_lua<box2f> {
        typedef std::true_type convertable;
        static box2f convert(lua_State* L, int idx);
    };
    
}

#endif
