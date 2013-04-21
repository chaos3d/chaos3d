#ifndef _ENGINE_LOOP
#define _ENGINE_LOOP

#include "common/common.h"
#include "boost/function.hpp"
#include <string>
#include <list>

_CHAOS_BEGIN

class AnimationManager;
class EventHandler;

class EngineLoop {
public:
    typedef boost::function<void ()> Callback;
    typedef boost::function<bool ()> Poller;
    typedef std::pair<Poller, Callback> Listener;
    typedef std::list<Listener> ListenerList;
    
    struct Yield{
    public:
        bool operator()() const{ return true; };
    };
    
    struct Config{
        std::string bootstrap;  // bootstrap file
        Callback startUp;
    };
    
    EngineLoop(Config const&);
    virtual ~EngineLoop();
    
	//EventHandler* getHandler();
	virtual bool startUp();
	virtual bool tearDown();
	virtual bool loop();

    void addListener(Listener const&);
    void addListener(Poller const& poller, Callback const& callback){
        addListener(Listener(poller, callback));
    }
    
protected:
    virtual void _forcelink();
	//virtual void collect();
    
    class Internal;
    Internal* _internal;
};

_CHAOS_END

#endif