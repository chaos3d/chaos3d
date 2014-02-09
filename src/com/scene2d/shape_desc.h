#ifndef _SHAPE_DESC_H
#define _SHAPE_DESC_H

#include <vector>
#include <type_traits>

namespace scene2d {
    class collider2d;
    
    struct point {
        float x, y;
    };
    
    struct mass {
        float x, y;
        float rotate; // rotation inertia
        float weight = 0.f;
    };

    struct shape {
        typedef std::tuple<shape &&> init_t;
        virtual ~shape() {};
        
    private:
        virtual void append_to(collider2d*) const = 0;
        friend class collider2d; // append_to, internal implementation
    };
    
    struct box : public shape{
        float x, y;
        float width, height;
        float angle;
        
        box(float x_, float y_, float w, float h, float angle_ = 0.f)
        : x(x_), y(y_), width(w), height(h), angle(angle_)
        {}

    private:
        virtual void append_to(collider2d*) const override;
    };
    
    struct circle : public shape {
        float x, y;
        float radius;
        
        circle(float x_, float y_, float r)
        : x(x_), y(y_), radius(r)
        {}
        
    private:
        virtual void append_to(collider2d*) const override;
    };
    
    struct polygon : public shape {
        std::vector<point> points;
        
        template<class... Args>
        polygon(Args&&... args)
        : points(std::forward<Args>(args)...)
        {}
        
    private:
        virtual void append_to(collider2d*) const override;
    };
    
    struct chain : public shape {
        std::vector<point> points;
        
        template<class... Args>
        chain(Args&&... args)
        : points(std::forward<Args>(args)...)
        {}
        
    private:
        virtual void append_to(collider2d*) const override;
    };
}

#endif