#ifndef _CHAOS3D_ACTION_H
#define _CHAOS3D_ACTION_H

#include <functional>
#include <initializer_list>
#include <cassert>

class action {
public:
    // add an action to its children
    // if this action is active, the added one will be
    // excuted immediately, or it will wait until the
    // parent gets triggered (action tree)
    action* push(action*);
    
    // whether it has any child
    bool empty() const { return _child_head == null_action; }
    
    // skip this action (go to end)
    void skip();
    
    // cancel this action (reverse back to beginning)
    void cancel();

    // whether it can be cancelled
    virtual bool cancellable() const;
    
    // create a sequence of actions with a parent action
    // so that it could be added to another sequence
    template<typename C = std::initializer_list<action*> >
    static action* wrap_sequence(C const& list, bool reversed = false) {
        return (new action())->push(sequence(list, reversed));
    }
    
    // create a sequence of actions (executing sequencially)
    template<typename C = std::initializer_list<action*> >
    static action* sequence(C const& list, bool reversed = false) {
        if (list.size() == 0)
            return nullptr;
        
        action* act = *list.begin();
        for (auto it = list.begin();++it != list.end(); ) {
            if(*it != nullptr)
                act->append(*it);
        }
        
        if (!reversed)
            act->reverse();
        return act;
    }
    
    // create a group of actions (executing simultaneously)
    template<typename C = std::initializer_list<action*> >
    static action* group(C const& list) {
        return (new action())->push(list.begin(), list.end());
    }

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
    using action::empty;
    using action::skip;
    
    root_action();
    
    template<typename C = std::initializer_list<action*> >
    root_action& add_sequence(C const& list) {
        push(action::sequence(list));
        return *this;
    }
    
    template<typename C = std::initializer_list<action*> >
    root_action& add_group(C const& list) {
        push(action::group(list));
        return *this;
    }
};

/// ---
// run the function once at start
// (so it has a chance to add more actions
// to its parent(itself) )
class action_functor : public action {
public:
    typedef std::function<void()> functor_t;
    typedef std::function<void(bool /*skipped*/)> canceller_t;
    
    action_functor(functor_t const& f,
                   canceller_t const& cancel = nullptr)
    : _func(f), _cancel(cancel)
    {}
    
    static action_functor* from(functor_t const&f,
                                canceller_t const& c = nullptr) {
        return new action_functor(f,c);
    }

protected:
    virtual void on_start() override;
    virtual void on_stop(bool skipping) override;
    virtual bool cancellable() const override ;
    
private:
    functor_t _func;
    canceller_t _cancel;
};
#endif