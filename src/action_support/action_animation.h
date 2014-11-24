#ifndef _CHAOS3D_ACTION_SUPPORT_ACTION_ANIMATION_H
#define _CHAOS3D_ACTION_SUPPORT_ACTION_ANIMATION_H

#include "action/action.h"
#include "common/timer.h"
#include "com/anim/animation.h"
#include <memory>

namespace act {
    
    /// action for animation component
    /// it is similar to the animation controller, to manage the internal
    /// state, calculate/blend animations and apply to the scene node
    class action_animation : public action {
    public:
        typedef timer::time_t time_t;
        
    public:
        action_animation(com::skeleton_animation_clip::ptr const&,
                         com::animation const*, // FIXME: memory manager
                         timer const& = global_timer_base::instance());
        
        virtual bool done() const override;
        
    protected:
        virtual void update() override;
        virtual void on_start() override;
        
    private:
        timer const& _timer;
        time_t _start;      // start time
        time_t _duration;   // the entire duration
        time_t _loop;       // the duration for each piece
        com::skeleton_animation_clip::ptr _clip;    // only one animation for now
        com::animation const* _animation;                 // FIXME: temp solution
    };
}
#endif
