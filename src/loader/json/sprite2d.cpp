#include "loader/json/json_loader.h"
#include "go/component.h"
#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/quad_sprite.h"
#include "loader/json/game_object.h"
#include <rapidjson/document.h>

using namespace sprite2d;
using namespace rapidjson;

template<>
camera2d::camera2d(game_object* go, json_loader const& json)
: camera2d(go) {
    //Document const& root = json.internal<Document>();
}

template<>
quad_sprite::quad_sprite(game_object* go, json_loader const& json)
: quad_sprite(go, 0) {
    //Document const& root = json.internal<Document>();
}