#include "go/game_object.h"
#include "go/component.h"

struct test_c : public component {
};
struct test_c2 : public component {
};

template class game_object<test_c>;

void test(){
    typedef game_object<test_c> go;
    go _go;
    _go.get_component<test_c>();
    _go.get_component<test_c2>();
}