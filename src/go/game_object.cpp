#include "game_object.h"

class transform : public component {};
class test_1 : public component {};

int test() {
    game_object go;
    go.find_component<transform>();
    go.find_component<test_1>();
    go.add_component(std::unique_ptr<transform>());
    go.add_component(std::unique_ptr<test_1>());
    return 0;
}