#ifndef _CHAOS3D_COM_SCENE2D_WORLD_EVENT_H
#define _CHAOS3D_COM_SCENE2D_WORLD_EVENT_H

#include "common/utility.h"
#include "event/event_dispatcher.h"

namespace scene2d {
    class collider2d;
    
    class contact_event : public event {
    public:
        contact_event(collider2d* contact1, collider2d* contact2)
        : _contact1(contact1), _contact2(contact2) {
        }
        
    private:
        typedef collider2d* contact_t;
        ATTRIBUTE(contact_t, contact1, nullptr);
        ATTRIBUTE(contact_t, contact2, nullptr);
    };
    
    class contact_began_event : public contact_event {
    public:
        contact_began_event(collider2d* contact1, collider2d* contact2)
        : contact_event(contact1, contact2) {
        }
    };

    class contact_ended_event : public contact_event {
    public:
        contact_ended_event(collider2d* contact1, collider2d* contact2)
        : contact_event(contact1, contact2) {
        }
    };
}

#endif