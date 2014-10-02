#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H

#include "re/render_target.h"
#include "event/event_dispatcher.h"

class render_window : public render_target, public event_dispatcher {
public:
    typedef Eigen::Vector2f window_pos_t;
    
    render_window(void* parent, ///! native parent handle
                  target_size_t const& size_,
                  window_pos_t const& pos_ = window_pos_t(0.f,0.f),
                  float backing_ratio = 1.f)
     : render_target(size_ * backing_ratio), _position(pos_), _backing_ratio(backing_ratio)
    {};
    virtual ~render_window() {};
    
    virtual void *native_handle() = 0;
    
    // this is the position in points
    window_pos_t const& get_position() const { return _position; };
    
    float get_backing_ratio() const { return _backing_ratio; };
    
    virtual window_pos_t convert_to_backing(window_pos_t const& pos) const {
        return pos * get_backing_ratio();
    }

    virtual window_pos_t convert_from_backing(window_pos_t const& pos) const {
        return pos / get_backing_ratio();
    }
    
    //void set_position();
    
protected:
    window_pos_t _position;
    float _backing_ratio;
};


#endif