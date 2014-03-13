#ifndef _TOUCH_EVENT_H
#define _TOUCH_EVENT_H

#include <Eigen/Dense>
#include "event/event_dispatcher.h"

class touch_event : public event {
public:
    typedef Eigen::Vector2f vector2f;
    typedef uint64_t pointer_t;

public:
    touch_event(vector2f const& position,
                float time,
                pointer_t which)
    : _position(position), _time(time),
    _which(which)
    {}
    
    ATTRIBUTE(vector2f, position, vector2f());
    ATTRIBUTE(float, time, 0.f);
    ATTRIBUTE(pointer_t, which, 0);
};

class touch_began_event : public touch_event {
public:
    touch_began_event(vector2f const& position,
                      float time,
                      pointer_t which)
    : touch_event(position, time, which)
    {}
};

class touch_moved_event : public touch_event {
public:
    touch_moved_event(vector2f const& position,
                      float time,
                      pointer_t which)
    : touch_event(position, time, which)
    {}
};

class touch_ended_event : public touch_event {
public:
    touch_ended_event(vector2f const& position,
                      float time,
                      pointer_t which)
    : touch_event(position, time, which)
    {}
};

class touch_cancelled_event : public touch_event {
public:
    touch_cancelled_event(vector2f const &position,
                          float time,
                          pointer_t which)
    : touch_event(position, time, which)
    {}
};

#endif