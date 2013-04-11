#include "EngineLoop.h"
#include "lyield/lyield.h"
#include "lua/lua.hpp"

//#include "anim/AnimationManager.h"
//#include "event/EventHandler.h"
//#include "event/Event.h"

struct EngineLoop::Internal {
    lua_State* mainL;
    //AnimationManager* animManager;
    //EventHandler* handler;     // for engine-level events: resume/suspend/crash
};

bool EngineLoop::loop() {
	lua_cpcall(_internal->mainL, lyield_run, NULL);
	return true;
}
