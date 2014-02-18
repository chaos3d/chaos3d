#include "action/action.h"
#include "action_keyframe.h"

static int _no_action = 0x12345678;
action* action::null_action = reinterpret_cast<action*>(&_no_action);

action::action()
: _child_head(null_action), _next_sibling(null_action),
_next(nullptr), _started(false)
{
}

action* action::push(action* act) {
    assert(act != null_action); // should never happen
    
    if(!act)
        return this;
    
    // already pushed to some action tree?
    assert(act->_next_sibling == null_action);
    act->_next_sibling = _child_head;
    _child_head = act;
    
    // pushing an action to a started one
    // should start it now
    if(started() && !act->started())
        act->on_start();
    return this;
}

bool action::cancellable() const{
    for(action* cur = _child_head;
        cur != null_action; cur = cur->_next_sibling){
        for(action* next = cur; next; next = next->_next) {
            if(!next->cancellable())
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

void action::append(action* nt){
    assert(nt->_next == nullptr);
    nt->_next = _next;
    _next = nt;
}

void action::reverse() {
    if(_next != nullptr && _next->_next != nullptr){
        action* pre = _next, *nt = _next->_next;
        pre->_next = nullptr;
        while(nt != nullptr) {
            action* next = nt->_next;
            nt->_next = pre;
            pre = nt;
            nt = next;
        }
        _next = pre;
    }
}

void action::on_start() {
    _started = true;
    for (action* child = _child_head;
         child != null_action;
         child = child->_next_sibling) {
        child->on_start();
    }
}

void action::on_end() {
    action* child = _child_head;
    _child_head = null_action;
    for (action *next = _child_head->_next_sibling;
         child != null_action; next = (child = next)->_next_sibling) {
        child->on_end();
        child->_next_sibling = null_action;
        delete child;
    }
    assert(_child_head == null_action);
    _started = false;
}

void action::on_stop(bool skip) {
    action* child = _child_head;
    _child_head = null_action;
    
    for(action* next = child->_next_sibling;
        child != null_action;
        next = (child = next)->_next_sibling) {
        child->_next_sibling = nullptr;
        child->on_stop(skip); // stop itself and all its children
        
        // stop the sequence
        for(action* link = child->_next; link;){
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
}

void action::update() {
    action* child = _child_head;
    _child_head = null_action;
    
    for(action* next = child->_next_sibling;
        child != null_action;
        next = (child = next)->_next_sibling) {
        child->_next_sibling = nullptr;
        child->update();
        if(child->done()) {
            child->on_end();
            push(child->_next);
            child->_next = nullptr;
            delete child;
        }else if(child->_next_sibling == nullptr) { // if not pushed to somewhere else
            push(child);
        }
    }
}

action::~action() {
    // all children and sequence should be released
    assert(_child_head == null_action);
    assert(_next_sibling == null_action || _next_sibling == nullptr);
    assert(_next == nullptr);
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
    if(!started())
        on_start();
    action::on_stop(skipping);
    if(!skipping)
        _cancel(false); // cancel, go to beginning
    else if(_cancel)
        _cancel(true); // skip, go to end
}

bool action_functor::cancellable() const {
    return _cancel != nullptr; // only if canceller exists
}
