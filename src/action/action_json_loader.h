#ifndef _CHAOS3D_ACTION_ACTION_JSON_LOADER_H
#define _CHAOS3D_ACTION_ACTION_JSON_LOADER_H

#include "action/action_keyframe.h"
#include "io/json_loader.h"

// this specifilization may not be needed if the template function is empty
// need to double check with other compilers...
template<> SPECIFY_LOADER_FOR(quaternionf_anim_kf_t, json_loader);

#endif