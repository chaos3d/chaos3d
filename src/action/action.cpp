#include "action/action.h"

action::action()
: _child_head(nullptr), _next_sibling(nullptr),
_next(nullptr), _started(false)
{
}

action& action::push(ptr&& act) {
    if (!act)
        return *this;
    
    // already pushed to some action tree?
    assert(!act->_next_sibling);
    act->_next_sibling.reset(_child_head.release());
    _child_head.reset(act.release());
    
    // pushing an action to a started one
    // should start it now
    if (started() && !_child_head->started()) // child_head == act
        _child_head->on_start();
    return *this;
}

bool action::cancellable() const{
    for (action* cur = _child_head.get();
        cur != nullptr; cur = cur->_next_sibling.get()){
        for (action* next = cur; next != nullptr;
             next = next->_next.get()) {
            if (!next->cancellable())
                return false;
        }
    }
    return true;
}

void action::cancel() {
    assert(cancellable());
    on_stop(false);
}

void action::skip() {
    on_stop(true);
}

void action::append(ptr&& nt){
    if (!nt)
        return;
    
    assert(nt->_next.get() == nullptr);
    nt->_next.reset(_next.release());
    _next.reset(nt.release());
}

void action::reverse() {
    if (_next.get() != nullptr && _next->_next.get() != nullptr) {
        ptr pre(_next.release());
        ptr nt(pre->_next.release());
        while (nt.get() != nullptr) {
            ptr next(nt->_next.release());
            nt->_next.reset(pre.release());
            pre.reset(nt.release());
            nt.reset(next.release());
        }
        _next.reset(pre.release());
    }
}

void action::on_start() {
    _started = true;
    for (action* child = _child_head.get();
         child != nullptr;
         child = child->_next_sibling.get()) {
        child->on_start();
    }
}

void action::on_end() {
    for (ptr child(_child_head.release());
         child; child.reset(child->_next_sibling.release())) {
        child->on_end();
    }
    _started = false;
}

void action::on_stop(bool skip) {
    assert(false); // FIXME
#if 0
    action* child = _child_head;
    _child_head = null_action;
    
    for (action* next = child->_next_sibling;
        child != null_action;
        next = (child = next)->_next_sibling) {
        child->_next_sibling = nullptr;
        child->on_stop(skip); // stop itself and all its children
        
        // stop the sequence
        for (action* link = child->_next; link;){
            action *del = link;
            link = link->_next;
            
            del->on_stop(skip);
            del->_next = nullptr;
            delete del;
        }
        child->_next = nullptr;
        delete child;
    }
    assert(_child_head == null_action); // shouldn't add any on stop
#endif
}

void action::update() {
    ptr current(_child_head.release());
    
    for (ptr next; current; ) {
        next.reset(current->_next_sibling.release());

        current->update();
        if (current->done()) {
            current->on_end();
            push(std::move(current->_next));
        } else if (!current->_next_sibling) { // if not pushed to somewhere else
            push(std::move(current));
        }
        
        current.reset(next.release());
    }
}

action::~action() {
}

root_action::root_action() {
    // root action will always be active
    on_start();
}

void action_functor::on_start() {
    _func();
    action::on_start();
};
    
void action_functor::on_stop(bool skipping) {
    if (!started())
        on_start();
    action::on_stop(skipping);
    if (!skipping)
        _cancel(false); // cancel, go to beginning
    else if (_cancel)
        _cancel(true); // skip, go to end
}

bool action_functor::cancellable() const {
    return _cancel != nullptr; // only if canceller exists
}
