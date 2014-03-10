#ifndef _CHAOS2D_APP_SCREEN_H
#define _CHAOS2D_APP_SCREEN_H

#include <deque>
#include <utility>
#include "action/action.h"

class screen_state {
public:
    typedef std::function<void ()> loop_t;
    
public:
    virtual bool done() const = 0;
    virtual void on_enter() {};
    virtual void on_leave() {};
    virtual void on_skip() {}; // when the state is pushed to the queue but ignored
    
    loop_t loop;
};

class screen {
public:
    typedef std::deque<std::auto_ptr<screen_state>> states_t;
    
public:
    virtual ~screen() {};
    
    virtual void loop();
    virtual void on_start() {}; // TODO: may not be needed?
    virtual void on_end() {};
    
    void push_front(screen_state*);
    void push_back(screen_state*);
    void pop_front();
    void clear_state();
    
    root_action& get_action() { return _action; }
    
private:
    states_t _states;
    root_action _action;
};
#endif