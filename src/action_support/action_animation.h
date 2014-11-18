#ifndef _CHAOS3D_ACTION_SUPPORT_ACTION_ANIMATION_H
#define _CHAOS3D_ACTION_SUPPORT_ACTION_ANIMATION_H

#include "action/action.h"
#include "common/timer.h"
#include <memory>

namespace act {
    
    /// action for animation component
    class action_animation : public action {
    public:
        typedef timer::time_t time_t;
        
    public:
        action_animation(timer const&);
        
        virtual bool done() const override;
        
    protected:
        virtual void update() override;
        virtual void on_start() override;
        
    private:
        struct internal;
        timer const& _timer;
        time_t _start;      // start time
        time_t _duration;   // the entire duration
        time_t _loop;       // the duration for each piece
        std::unique_ptr<internal> _internal;
    };
}
#endif
