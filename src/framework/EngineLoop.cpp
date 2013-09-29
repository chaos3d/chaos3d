#include "EngineLoop.h"
#include "AutoreleasePool.h"
#include "lyield/lyield.h"
#include "lua/lua.hpp"
#include "platform/lplatform.h"

#include "io/IOManager.h"
#include "io/wrapper_lua.h"
#include "io/DataStream.h"

#include <iostream>

//#include "luabind/engine-bind.h"

//#include "anim/AnimationManager.h"
//#include "event/EventHandler.h"
//#include "event/Event.h"

using namespace std;

struct EngineLoop::Internal {
    Config config;
    lua_State* mainL;
    ListenerList listenerList[2];
    int activeListenerList;
    //AnimationManager* animManager;
    //EventHandler* handler;     // for engine-level events: resume/suspend/crash
};

EngineLoop::EngineLoop(Config const& config){
    _internal = new Internal();
    _internal->config = config;
    _internal->activeListenerList = 0;
    new AutoreleasePool();
}

EngineLoop::~EngineLoop() {
    delete _internal;
    delete AutoreleasePool::getInstance();
}

void EngineLoop::addListener(Listener const& lsn){
    _internal->listenerList[_internal->activeListenerList].push_back(lsn);
}

lua_State* EngineLoop::getState(){
    return _internal->mainL;
}

bool EngineLoop::startUp() {
    lua_State* &L = _internal->mainL;
    
    // TODO: refactor this to lThread => lYield goes to lThread then
    L = lua_open();
    luaL_openlibs(L);
    //lua_cpcall(L, luaopen_lplatform, 0);
    lua_cpcall(L, luaopen_lyield, 0);
    
    // TODO: add -u link flag
    //lua_cpcall(L, luaopen_scene2d, 0);
    //lua_cpcall(L, luaopen_platform, 0);     // link against different platform implementation libs

    //_internal->config.startUp(); // should extend this class
#if 1
    assert(IOManager::getInstance() != NULL);
    DataStream* ds = IOManager::getInstance()->createStreamByPath(_internal->config.bootstrap + ".lua");
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
    int curList = _internal->activeListenerList;
    _internal->activeListenerList = _internal->activeListenerList == 0 ? 1 : 0;
    for(ListenerList::iterator it = _internal->listenerList[curList].begin();
        it != _internal->listenerList[curList].end(); ++it){
        if(!it->first()) {
            _internal->listenerList[_internal->activeListenerList].push_back(*it);
        }else
            it->second();
    }
    _internal->listenerList[curList].clear();
    
	lua_cpcall(_internal->mainL, lyield_run, NULL);
    lua_gc(_internal->mainL, LUA_GCSTEP, 2);
    AutoreleasePool::getInstance()->cleanUp();
	return true;
}
