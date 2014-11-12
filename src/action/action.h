#ifndef _CHAOS3D_ACTION_H
#define _CHAOS3D_ACTION_H

#include <functional>
#include <initializer_list>
#include <cassert>
#include <memory>

class action {
public:
    typedef std::unique_ptr<action> ptr;
    
public:
    // add an action to its children
    // if this action is active, the added one will be
    // excuted immediately, or it will wait until the
    // parent gets triggered (action tree)
    action& push(ptr&&);
    
    // whether it has any child
    bool empty() const { return !_child_head; }
    
    // skip this action (go to end)
    void skip();
    
    // cancel this action (reverse back to beginning)
    void cancel();

    // whether it can be cancelled
    virtual bool cancellable() const;
    
    // create a sequence of actions with a parent action
    // so that it could be added to another sequence
    template<typename C = std::initializer_list<action*> >
    static ptr wrap_sequence(C const& list, bool reversed = false) {
        return ptr(& (new action())->push(sequence(list, reversed)));
    }
    
    // create a sequence of actions (executing sequencially)
    template<typename C = std::initializer_list<ptr> >
    static ptr sequence(C && list, bool reversed = false) {
        if (list.size() == 0)
            return nullptr;
        
        ptr act(list.begin()->release());
        for (auto it = list.begin() + 1;
             it != list.end();
             ++it) {
            act->append(std::move(*it));
        }
        
        // FIXME: link head is changed too?
        if (!reversed)
            act->reverse();
        return act;
    }
    
    // create a group of actions (executing simultaneously)
    template<typename C = std::initializer_list<ptr> >
    static ptr group(C && list) {
        action* act = new action();
        for (auto& it : list) {
            act->push(std::move(it));
        }
        return ptr(act);
    }

    virtual ~action();
    
    /// update the internal state
    virtual void update();
    
    /// check if the action is finished
    virtual bool done() const { return empty(); }

    /// start the action
    /// life cycle:
    ///     action.start();
    ///     action.update();
    ///     if (action.done()) { delete action; }
    void start();
    
protected:
    bool started() const { return _started; }
    
    virtual void on_start();
    virtual void on_end();
    virtual void on_stop(bool skip);
    
    action();
    
private:
    void reverse();
    void append(ptr&&);
    
    ptr _child_head;
    ptr _next_sibling;
    ptr _next;
    bool _started;
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
    
    static ptr from(functor_t const&f,
                                canceller_t const& c = nullptr) {
        return ptr(new action_functor(f,c));
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