#include "EngineLoop.h"
#include "AutoreleasePool.h"
#include "lyield/lyield.h"
#include "lua/lua.hpp"
#include "platform/lplatform.h"

//#include "io/IOManager.h"

#include <iostream>

//#include "anim/AnimationManager.h"
//#include "event/EventHandler.h"
//#include "event/Event.h"

using namespace std;

struct EngineLoop::Internal {
    Config config;
    lua_State* mainL;
    //AnimationManager* animManager;
    //EventHandler* handler;     // for engine-level events: resume/suspend/crash
};

EngineLoop::EngineLoop(Config const& config){
    _internal = new Internal();
    _internal->config = config;
    
    new AutoreleasePool();
}

EngineLoop::~EngineLoop() {
    delete _internal;
    delete AutoreleasePool::getInstance();
}

void EngineLoop::_forcelink(){
    static int (*__lualibs[])(lua_State*) = {
        luaopen_lyield,
    };
}

bool EngineLoop::startUp() {
    _internal->mainL = lua_open();
    luaL_openlibs(_internal->mainL);
    lua_cpcall(_internal->mainL, luaopen_lplatform, 0);
    
#if 0
    if(IOManager::getInstance() == NULL){
        new IOManager();
    }
    
    DataStream* ds = IOManager::getInstance()->createStreamByPath(_internal->config.bootstrap);
#endif
    // temp code for testing
    //luaL_dostring(_internal->mainL, (std::string("require \"") + _internal->config.bootstrap + "\"").c_str());
    luaL_dostring(_internal->mainL, "require 'bootstrap'");
    printf("%s", lua_tostring(_internal->mainL, -1));
    return true;
}

bool EngineLoop::tearDown() {
    lua_close(_internal->mainL);
    return true;
}

bool EngineLoop::loop() {
	lua_cpcall(_internal->mainL, lyield_run, NULL);
    lua_gc(_internal->mainL, LUA_GCSTEP, 2);
    AutoreleasePool::getInstance()->cleanUp();
	return true;
}
