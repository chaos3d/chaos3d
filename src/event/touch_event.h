#ifndef _TOUCH_EVENT_H
#define _TOUCH_EVENT_H

#include <Eigen/Dense>
#include "event/event_dispatcher.h"

class touch_event : public event {
public:
    typedef Eigen::Vector2f vector2f;

public:
    touch_event(vector2f const& position,
                float time,
                uint32_t which)
    : _position(position), _time(time),
    _which(which)
    {}
    
    ATTRIBUTE(vector2f, position);
    ATTRIBUTE(float, time);
    ATTRIBUTE(uint32_t, which);
};

class touch_began_event : public touch_event {
public:
    touch_began_event(vector2f const& position,
                      float time,
                      uint32_t which)
    : touch_event(position, time, which)
    {}
};

class touch_moved_event : public touch_event {
public:
    touch_moved_event(vector2f const& position,
                      float time,
                      uint32_t which)
    : touch_event(position, time, which)
    {}
};

class touch_ended_event : public touch_event {
public:
    touch_ended_event(vector2f const& position,
                      float time,
                      uint32_t which)
    : touch_event(position, time, which)
    {}
};

class touch_cancelled_event : public touch_event {
public:
    touch_cancelled_event(vector2f const &position,
                          float time,
                          uint32_t which)
    : touch_event(position, time, which)
    {}
};

#endif