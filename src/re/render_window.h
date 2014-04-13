#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H

#include "re/render_target.h"
#include "event/event_dispatcher.h"

class render_window : public render_target, public event_dispatcher {
public:
    typedef Eigen::Vector2f window_pos_t;
    
    render_window(target_size_t const& size_,
                  window_pos_t const& pos_ = window_pos_t(0.f,0.f)) :
    render_target(size_), _position(pos_)
    {};
    virtual ~render_window() {};
    
    virtual void *native_handle() = 0;
    window_pos_t const& get_position() const { return _position; };
    //void set_position();
    //void set_size();
    
    virtual void set_title(char const*) {}
    
protected:
    window_pos_t _position;
};


#endif