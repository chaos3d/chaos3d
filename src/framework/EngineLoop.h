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
    virtual ~EngineLoop();
    
	//EventHandler* getHandler();
	virtual bool startUp();
	virtual bool tearDown();
	virtual bool loop();
    
protected:
    virtual void _forcelink();
	//virtual void collect();
    
    class Internal;
    Internal* _internal;
};

_CHAOS_END

#endif