#ifndef _CHAOS3D_ACTION_H
#define _CHAOS3D_ACTION_H

#include <functional>
#include <initializer_list>
#include <cassert>

class action {
public:
    action* push(action*);
    bool empty() const { return _child_head == null_action; }
    void skip();
    void cancel();

    virtual bool cancellable() const;
    
protected:
    bool started() const { return _started; }
    
    virtual void on_start();
    virtual void on_end();
    virtual void on_stop(bool skip);
    virtual bool done() const { return empty(); }
    
    virtual void update();
    
    action();
    virtual ~action();
    
private:
    void reverse();
    void append(action*);
    
    action* _child_head;
    action* _next_sibling;
    action* _next;
    bool _started;
        
    static action* null_action;
};

class root_action : public action{
public:
    using action::update;
};
#endif