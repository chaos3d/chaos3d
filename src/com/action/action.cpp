#include "com/action/action.h"

namespace com {
    action::action(game_object* go)
    : component(go), _action(new ::action()) {
        action_mgr::instance().add_action(this);
    }
    
    action& action::operator=(action const& rhs) {
        return *this;
    }
    
    void action::destroy() {
        _mark_for_remove = true;
    }
    
    action& action::add_sequence(std::vector<ptr>&& seq) {
        _action->push(::action::sequence(seq));
        return *this;
    }
    
    action& action::add_group(std::vector<ptr>&& seq) {
        _action->push(::action::group(seq));
        return *this;
    }

    action_mgr& action_mgr::add_action(com::action* act) {
        act->get_action()->start(); // the parent action is always active
        _actions.emplace_front(act);
        return *this;
    }
    
    void action_mgr::pre_update(const goes_t &) {
        _actions.remove_if([] (action_ptr const& act) {
            return act->_mark_for_remove;
        });
        
        for (auto& it : _actions) {
            it->get_action()->update();
        }
    }
}