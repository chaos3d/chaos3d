#include "app/application.h"
#include "app/screen.h"

screen& application::get_screen() const {
    static screen _default;
    return _default;
}
