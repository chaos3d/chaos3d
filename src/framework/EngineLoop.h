#ifndef _ENGINE_LOOP
#define _ENGINE_LOOP

#include <common/common.h>
#include <common/Singleton.h>
#include <boost/function.hpp>
#include <lua/lua.hpp>
#include <string>
#include <list>

_CHAOS_BEGIN

class AnimationManager;
class EventHandler;

class EngineLoop
    : public Singleton<EngineLoop>  /// it is not necessory to be a singleton, this is convenient to get the client extended instance
{
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

    /// TODO: replace with lThread
    lua_State* getState();
protected:
	//virtual void collect();
    
    class Internal;
    Internal* _internal;
};

_CHAOS_END

#endif