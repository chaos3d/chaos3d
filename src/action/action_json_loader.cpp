#include "action/action_json_loader.h"

template<> SPECIFY_LOADER_FOR(quaternionf_anim_kf_t, json_loader) {
    return {true, 0};
}
