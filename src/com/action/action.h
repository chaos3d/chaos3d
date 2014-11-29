#ifndef _CHAOS3D_COM_ACTION_ACTION_H
#define _CHAOS3D_COM_ACTION_ACTION_H

#include "go/component.h"
#include "action/action.h"
#include <vector>
#include <forward_list>

namespace com {
    class action_mgr;
    
    class action : public component {
    public:
        typedef action_mgr manager_t;
        typedef ::action::ptr ptr;
        
    public:
        action(game_object*);
        
        action& operator=(action const& rhs); /// will not copy actions
        
        action& add_sequence(std::vector<ptr>&& seq);
        action& add_group(std::vector<ptr>&& seq);
        
        ::action* get_action() const { return _action.get(); }
        
    protected:
        virtual void destroy() override;
        
    private:
        ::action::ptr _action;
        bool _mark_for_remove = false; // managed by action_mgr
        
        SIMPLE_CLONE(com::action);
        friend class action_mgr;
    };
    
    class action_mgr : public component_manager_base<action_mgr> {
    public:
        typedef std::false_type component_fixed_t;
        typedef std::unique_ptr<action> action_ptr;
        typedef std::forward_list<action_ptr> actions_t;
        
    protected:
        /// to add the action to the managed list
        action_mgr& add_action(action*);
        
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override {};

    private:
        actions_t _actions;
        
        friend class action;
    };

}
#endif
