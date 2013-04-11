#ifndef _ENGINE_LOOP
#define _ENGINE_LOOP

#include "common/common.h"
#include <string>

_CHAOS_BEGIN

class AnimationManager;
class EventHandler;

class EngineLoop {
public:
    struct Config{
        std::string bootstrap;  // bootstrap file
    };
    
    EngineLoop(Config const&);
    
	EventHandler* getHandler();
	virtual bool startUp();
	virtual bool tearDown();
	virtual void collect();
	virtual bool loop();
    
protected:
    AnimationManager* _animManager;
    EventHandler* _handler;     // for engine-level events: resume/suspend/crash
};

_CHAOS_END

#endif