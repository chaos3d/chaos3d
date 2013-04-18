#include "EngineLoop.h"
#include "AutoreleasePool.h"
#include "lyield/lyield.h"
#include "lua/lua.hpp"
#include "platform/lplatform.h"

#include "io/IOManager.h"
#include "io/wrapper_lua.h"
#include "io/DataStream.h"

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
        //luaopen_lyield,
    };
}

bool EngineLoop::startUp() {
    lua_State* &L = _internal->mainL;
    L = lua_open();
    luaL_openlibs(L);
    lua_cpcall(L, luaopen_lplatform, 0);
    lua_cpcall(L, luaopen_lyield, 0);

#if 1
    DataStream* ds = NULL;
    if(IOManager::getInstance() == NULL){
        //TODO: logging error
    }else
        ds = IOManager::getInstance()->createStreamByPath(_internal->config.bootstrap + ".lua");
    if(ds != NULL){
        if(luaio_load(L, ds, ds->where()) != 0){
            // TODO: error log
        }else{
            lua_pushcfunction(L, lyield_resume);
            lua_insert(L, lua_gettop(L)-1);
            lua_pcall(L, 1, LUA_MULTRET, 0);    // push the function 
        }
    }
#else
    // temp code for testing
    //luaL_dostring(_internal->mainL, (std::string("require \"") + _internal->config.bootstrap + "\"").c_str());
    luaL_loadstring(_internal->mainL, "require 'bootstrap'");
    lua_pushcfunction(L, lyield_resume);
    lua_insert(L, lua_gettop(L)-1);
    lua_pcall(L, 1, LUA_MULTRET, 0);
    printf("%s", lua_tostring(_internal->mainL, -1));
#endif
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
